import { prisma } from "./db";

const PRIORITY_BONUS: Record<string, number> = { low: 2, normal: 5, high: 10, urgent: 15 };
const START_POINTS = 5; // initiation reward — the hardest part with ADHD

export function pointsForCompletion(priority: string, estimateMinutes: number | null): number {
  const base = 10;
  const prio = PRIORITY_BONUS[priority] ?? 5;
  const size = estimateMinutes ? Math.min(10, Math.ceil(estimateMinutes / 15)) : 0;
  return base + prio + size;
}

export async function awardStart(taskId: string): Promise<number> {
  await prisma.pointEvent.create({
    data: { kind: "start", points: START_POINTS, taskId, note: "Started a task" },
  });
  return START_POINTS;
}

export async function awardCompletion(
  taskId: string,
  priority: string,
  estimateMinutes: number | null
): Promise<number> {
  const points = pointsForCompletion(priority, estimateMinutes);
  await prisma.pointEvent.create({
    data: { kind: "complete", points, taskId, note: "Completed a task" },
  });
  return points;
}

export function levelForXp(xp: number): { level: number; into: number; nextAt: number } {
  // each level costs 100 XP
  const level = Math.floor(xp / 100) + 1;
  const into = xp % 100;
  return { level, into, nextAt: 100 };
}

function dayKey(d: Date): string {
  return `${d.getFullYear()}-${d.getMonth()}-${d.getDate()}`;
}

export async function getStats() {
  const startOfToday = new Date();
  startOfToday.setHours(0, 0, 0, 0);

  const [allEvents, todayEvents, doneToday] = await Promise.all([
    prisma.pointEvent.findMany({ select: { points: true, kind: true, createdAt: true } }),
    prisma.pointEvent.findMany({ where: { createdAt: { gte: startOfToday } }, select: { points: true } }),
    prisma.task.count({ where: { status: "done", completedAt: { gte: startOfToday } } }),
  ]);

  const xp = allEvents.reduce((s, e) => s + e.points, 0);
  const todayPoints = todayEvents.reduce((s, e) => s + e.points, 0);

  // Streak: consecutive days (ending today or yesterday) with at least one completion.
  const completionDays = new Set(
    allEvents.filter((e) => e.kind === "complete").map((e) => dayKey(new Date(e.createdAt)))
  );
  let streak = 0;
  const cursor = new Date(startOfToday);
  if (!completionDays.has(dayKey(cursor))) cursor.setDate(cursor.getDate() - 1); // allow "yesterday" to keep streak alive
  while (completionDays.has(dayKey(cursor))) {
    streak += 1;
    cursor.setDate(cursor.getDate() - 1);
  }

  return { xp, todayPoints, doneToday, streak, ...levelForXp(xp) };
}
