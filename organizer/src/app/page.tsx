"use client";

import { useEffect, useState, useCallback } from "react";
import Nav from "@/components/Nav";

type Area = { id: string; name: string; color: string };
type Task = {
  id: string;
  title: string;
  notes: string | null;
  status: string;
  priority: string;
  dueDate: string | null;
  estimateMinutes: number | null;
  startedAt: string | null;
  area: Area | null;
  children?: Task[];
};
type Stats = {
  xp: number;
  level: number;
  into: number;
  nextAt: number;
  todayPoints: number;
  doneToday: number;
  streak: number;
};

function startOfToday() {
  const d = new Date();
  d.setHours(0, 0, 0, 0);
  return d;
}

function dueLabel(due: string | null): string {
  if (!due) return "";
  const d = new Date(due);
  const today = startOfToday();
  const diff = Math.round((new Date(d.getFullYear(), d.getMonth(), d.getDate()).getTime() - today.getTime()) / 86400000);
  if (diff === 0) return "Today";
  if (diff === 1) return "Tomorrow";
  if (diff === -1) return "Yesterday";
  if (diff < 0) return `${-diff}d overdue`;
  if (diff < 7) return d.toLocaleDateString(undefined, { weekday: "short" });
  return d.toLocaleDateString(undefined, { month: "short", day: "numeric" });
}

function estLabel(m: number | null): string {
  if (!m) return "";
  if (m < 60) return `~${m}m`;
  const h = Math.floor(m / 60);
  const rem = m % 60;
  return rem ? `~${h}h${rem}m` : `~${h}h`;
}

export default function Dashboard() {
  const [tasks, setTasks] = useState<Task[]>([]);
  const [areas, setAreas] = useState<Area[]>([]);
  const [stats, setStats] = useState<Stats | null>(null);
  const [loading, setLoading] = useState(true);
  const [quick, setQuick] = useState("");
  const [quickArea, setQuickArea] = useState("");
  const [focus, setFocus] = useState("");
  const [focusBusy, setFocusBusy] = useState(false);
  const [flash, setFlash] = useState("");
  const [breakingId, setBreakingId] = useState<string | null>(null);

  const loadStats = useCallback(async () => {
    const res = await fetch("/api/stats");
    if (res.ok) setStats(await res.json());
  }, []);

  const load = useCallback(async () => {
    const res = await fetch("/api/tasks");
    if (res.ok) {
      const data = await res.json();
      setTasks(data.tasks);
      setAreas(data.areas);
    }
    setLoading(false);
    loadStats();
  }, [loadStats]);

  useEffect(() => {
    load();
  }, [load]);

  function celebrate(points: number, label: string) {
    if (points <= 0) return;
    setFlash(`+${points} XP · ${label}`);
    setTimeout(() => setFlash(""), 2200);
  }

  async function complete(id: string) {
    const res = await fetch("/api/tasks", {
      method: "PATCH",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ id, status: "done" }),
    });
    if (res.ok) {
      const { pointsAwarded } = await res.json();
      celebrate(pointsAwarded, "done! 🎉");
    }
    load();
  }

  async function start(id: string) {
    setTasks((t) =>
      t.map((x) =>
        x.id === id
          ? { ...x, startedAt: new Date().toISOString() }
          : { ...x, children: x.children?.map((c) => (c.id === id ? { ...c, startedAt: new Date().toISOString() } : c)) }
      )
    );
    const res = await fetch("/api/tasks", {
      method: "PATCH",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ id, action: "start" }),
    });
    if (res.ok) {
      const { pointsAwarded } = await res.json();
      celebrate(pointsAwarded, "started — that's the hard part! 💪");
    }
    loadStats();
  }

  async function breakDown(id: string) {
    setBreakingId(id);
    const res = await fetch("/api/breakdown", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ taskId: id }),
    });
    setBreakingId(null);
    if (res.ok) await load();
  }

  async function addQuick(e: React.FormEvent) {
    e.preventDefault();
    if (!quick.trim()) return;
    const title = quick;
    setQuick("");
    await fetch("/api/tasks", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ title, areaId: quickArea || undefined }),
    });
    load();
  }

  async function whatNow() {
    setFocusBusy(true);
    setFocus("");
    const res = await fetch("/api/focus", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({}),
    });
    setFocusBusy(false);
    if (res.ok) {
      const { recommendation } = await res.json();
      setFocus(recommendation);
    } else {
      setFocus("Couldn't reach the coach — pick the smallest task and do just its first 2 minutes.");
    }
  }

  const today = startOfToday();
  const in7 = new Date(today);
  in7.setDate(in7.getDate() + 7);

  const overdue = tasks.filter((t) => t.dueDate && new Date(t.dueDate) < today);
  const dueToday = tasks.filter(
    (t) => t.dueDate && new Date(t.dueDate) >= today && new Date(t.dueDate).toDateString() === today.toDateString()
  );
  const upcoming = tasks.filter(
    (t) => t.dueDate && new Date(t.dueDate) > today && new Date(t.dueDate) <= in7 && !dueToday.includes(t)
  );
  const noDate = tasks.filter((t) => !t.dueDate);

  const StartOrFocus = ({ t }: { t: Task }) =>
    t.startedAt ? (
      <span className="shrink-0 text-xs font-medium text-blue-600">▶ focusing</span>
    ) : (
      <button
        onClick={() => start(t.id)}
        className="shrink-0 rounded-full bg-blue-50 px-2.5 py-1 text-xs font-medium text-blue-700 hover:bg-blue-100"
        title="Start now (+5 XP for initiating)"
      >
        Start
      </button>
    );

  const ChunkRow = ({ c }: { c: Task }) => (
    <li className="flex items-center gap-2 rounded-md bg-slate-50 px-3 py-1.5">
      <button
        onClick={() => complete(c.id)}
        aria-label="Complete sub-task"
        className="h-4 w-4 shrink-0 rounded-full border-2 border-slate-300 hover:border-green-500 hover:bg-green-50"
      />
      <span className="min-w-0 flex-1 truncate text-sm text-slate-700">{c.title}</span>
      {c.estimateMinutes && (
        <span className="shrink-0 rounded-full bg-indigo-100 px-2 py-0.5 text-xs font-semibold text-indigo-700">
          {estLabel(c.estimateMinutes)}
        </span>
      )}
      <StartOrFocus t={c} />
    </li>
  );

  const Section = ({ title, items, accent }: { title: string; items: Task[]; accent?: string }) => {
    if (items.length === 0) return null;
    return (
      <section className="mb-6">
        <h2 className={`mb-2 text-xs font-semibold uppercase tracking-wide ${accent || "text-slate-500"}`}>
          {title} <span className="text-slate-400">({items.length})</span>
        </h2>
        <ul className="space-y-1">
          {items.map((t) => {
            const kids = t.children ?? [];
            return (
              <li key={t.id} className="rounded-lg border border-slate-200 bg-white px-3 py-2">
                <div className="flex items-center gap-3">
                  <button
                    onClick={() => complete(t.id)}
                    aria-label="Complete"
                    className="h-5 w-5 shrink-0 rounded-full border-2 border-slate-300 hover:border-green-500 hover:bg-green-50"
                  />
                  <div className="min-w-0 flex-1">
                    <div className="truncate text-sm">{t.title}</div>
                    <div className="flex items-center gap-2 text-xs text-slate-400">
                      {t.estimateMinutes ? <span>{estLabel(t.estimateMinutes)}</span> : null}
                      {kids.length > 0 && <span>· {kids.length} steps</span>}
                      {t.notes && <span className="truncate">{t.notes}</span>}
                    </div>
                  </div>
                  {kids.length === 0 && (
                    <button
                      onClick={() => breakDown(t.id)}
                      disabled={breakingId === t.id}
                      className="shrink-0 rounded-full bg-indigo-50 px-2.5 py-1 text-xs font-medium text-indigo-700 hover:bg-indigo-100 disabled:opacity-50"
                      title="Let Claude split this into 15–60 min chunks"
                    >
                      {breakingId === t.id ? "Splitting…" : "Break down"}
                    </button>
                  )}
                  <StartOrFocus t={t} />
                  {t.priority === "urgent" && <span className="text-xs font-semibold text-red-600">!!</span>}
                  {t.priority === "high" && <span className="text-xs font-semibold text-orange-500">!</span>}
                  {t.area && (
                    <span
                      className="shrink-0 rounded-full px-2 py-0.5 text-xs font-medium text-white"
                      style={{ backgroundColor: t.area.color }}
                    >
                      {t.area.name}
                    </span>
                  )}
                  {t.dueDate && (
                    <span
                      className={`shrink-0 text-xs ${new Date(t.dueDate) < today ? "font-semibold text-red-600" : "text-slate-500"}`}
                    >
                      {dueLabel(t.dueDate)}
                    </span>
                  )}
                </div>
                {kids.length > 0 && (
                  <ul className="mt-2 space-y-1 border-l-2 border-indigo-100 pl-3">
                    {kids.map((c) => (
                      <ChunkRow key={c.id} c={c} />
                    ))}
                  </ul>
                )}
              </li>
            );
          })}
        </ul>
      </section>
    );
  };

  return (
    <main>
      <Nav active="dashboard" />
      <div className="mx-auto max-w-2xl p-4">
        {/* Gamification bar */}
        {stats && (
          <div className="mb-4 flex items-center gap-4 rounded-xl border border-slate-200 bg-white px-4 py-3">
            <div className="flex flex-col">
              <span className="text-xs text-slate-500">Level</span>
              <span className="text-lg font-bold leading-none">{stats.level}</span>
            </div>
            <div className="flex-1">
              <div className="mb-1 flex justify-between text-xs text-slate-500">
                <span>{stats.into} / {stats.nextAt} XP</span>
                <span>{stats.todayPoints} today</span>
              </div>
              <div className="h-2 w-full overflow-hidden rounded-full bg-slate-100">
                <div className="h-full bg-emerald-500" style={{ width: `${(stats.into / stats.nextAt) * 100}%` }} />
              </div>
            </div>
            <div className="flex flex-col items-center">
              <span className="text-lg font-bold leading-none">🔥 {stats.streak}</span>
              <span className="text-xs text-slate-500">day{stats.streak === 1 ? "" : "s"}</span>
            </div>
            <div className="flex flex-col items-center">
              <span className="text-lg font-bold leading-none">{stats.doneToday}</span>
              <span className="text-xs text-slate-500">done</span>
            </div>
          </div>
        )}

        {flash && (
          <div className="mb-4 rounded-lg bg-emerald-600 px-4 py-2 text-center text-sm font-medium text-white">
            {flash}
          </div>
        )}

        {/* What now? coach */}
        <div className="mb-4">
          <button
            onClick={whatNow}
            disabled={focusBusy}
            className="w-full rounded-xl bg-indigo-600 px-4 py-3 text-sm font-semibold text-white hover:bg-indigo-700 disabled:opacity-50"
          >
            {focusBusy ? "Thinking it through…" : "🎯 What should I do right now?"}
          </button>
          {focus && (
            <div className="mt-2 whitespace-pre-wrap rounded-xl border border-indigo-200 bg-indigo-50 p-4 text-sm text-indigo-900">
              {focus}
            </div>
          )}
        </div>

        <form onSubmit={addQuick} className="mb-6 flex gap-2">
          <input
            value={quick}
            onChange={(e) => setQuick(e.target.value)}
            placeholder="Add a task…"
            className="flex-1 rounded-md border border-slate-300 px-3 py-2 text-sm outline-none focus:border-slate-900"
          />
          <select
            value={quickArea}
            onChange={(e) => setQuickArea(e.target.value)}
            className="rounded-md border border-slate-300 px-2 py-2 text-sm"
          >
            <option value="">No area</option>
            {areas.map((a) => (
              <option key={a.id} value={a.id}>
                {a.name}
              </option>
            ))}
          </select>
          <button className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white">Add</button>
        </form>

        {loading ? (
          <p className="text-sm text-slate-500">Loading…</p>
        ) : tasks.length === 0 ? (
          <p className="text-sm text-slate-500">Nothing open. Add a task above, or chat with your assistant.</p>
        ) : (
          <>
            <Section title="Overdue" items={overdue} accent="text-red-600" />
            <Section title="Today" items={dueToday} accent="text-slate-700" />
            <Section title="Next 7 days" items={upcoming} />
            <Section title="No due date" items={noDate} />
          </>
        )}
      </div>
    </main>
  );
}
