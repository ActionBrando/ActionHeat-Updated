import { NextRequest, NextResponse } from "next/server";
import { prisma } from "@/lib/db";
import { isAuthed } from "@/lib/auth";
import { awardCompletion, awardStart } from "@/lib/gamify";

export async function GET() {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });

  const [tasks, areas] = await Promise.all([
    prisma.task.findMany({
      where: { status: "open" },
      include: { area: true, project: true },
      orderBy: [{ dueDate: "asc" }, { createdAt: "desc" }],
    }),
    prisma.area.findMany({ orderBy: { sortOrder: "asc" } }),
  ]);

  return NextResponse.json({ tasks, areas });
}

export async function POST(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const body = await req.json();
  if (!body.title) return NextResponse.json({ error: "title required" }, { status: 400 });

  const task = await prisma.task.create({
    data: {
      title: String(body.title),
      notes: body.notes || null,
      areaId: body.areaId || null,
      priority: body.priority || "normal",
      dueDate: body.dueDate ? new Date(body.dueDate) : null,
      source: "web",
    },
    include: { area: true },
  });
  return NextResponse.json({ task });
}

export async function PATCH(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const body = await req.json();
  if (!body.id) return NextResponse.json({ error: "id required" }, { status: 400 });
  const id = String(body.id);

  // "start" action — the initiation reward (the hard part with ADHD)
  if (body.action === "start") {
    const existing = await prisma.task.findUnique({ where: { id } });
    let pointsAwarded = 0;
    if (existing && !existing.startedAt) pointsAwarded = await awardStart(id);
    const task = await prisma.task.update({
      where: { id },
      data: { startedAt: existing?.startedAt ?? new Date() },
      include: { area: true },
    });
    return NextResponse.json({ task, pointsAwarded });
  }

  const data: Record<string, unknown> = {};
  if (body.status) {
    data.status = body.status;
    data.completedAt = body.status === "done" ? new Date() : null;
  }
  if (body.title !== undefined) data.title = body.title;
  if (body.notes !== undefined) data.notes = body.notes;
  if (body.areaId !== undefined) data.areaId = body.areaId || null;
  if (body.priority !== undefined) data.priority = body.priority;
  if (body.dueDate !== undefined) data.dueDate = body.dueDate ? new Date(body.dueDate) : null;
  if (body.estimateMinutes !== undefined) data.estimateMinutes = body.estimateMinutes || null;

  const task = await prisma.task.update({ where: { id }, data, include: { area: true } });

  let pointsAwarded = 0;
  if (body.status === "done") {
    pointsAwarded = await awardCompletion(task.id, task.priority, task.estimateMinutes);
  }
  return NextResponse.json({ task, pointsAwarded });
}

export async function DELETE(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const { searchParams } = new URL(req.url);
  const id = searchParams.get("id");
  if (!id) return NextResponse.json({ error: "id required" }, { status: 400 });
  await prisma.task.delete({ where: { id } });
  return NextResponse.json({ ok: true });
}
