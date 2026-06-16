import Anthropic from "@anthropic-ai/sdk";
import { prisma } from "./db";
import { awardCompletion } from "./gamify";

const MODEL = process.env.ANTHROPIC_MODEL || "claude-opus-4-8";

function client(): Anthropic {
  if (!process.env.ANTHROPIC_API_KEY) {
    throw new Error("ANTHROPIC_API_KEY is not configured");
  }
  return new Anthropic();
}

// ---------------------------------------------------------------------------
// Tool definitions exposed to Claude
// ---------------------------------------------------------------------------

const tools: Anthropic.Tool[] = [
  {
    name: "list_areas",
    description:
      "List the user's life areas (top-level buckets like Business, Investment Properties, Life). Call this first if you are unsure which areas exist.",
    input_schema: { type: "object", properties: {}, additionalProperties: false },
  },
  {
    name: "create_task",
    description:
      "Create a task / to-do item. Infer the most likely area from the wording. Parse any relative dates (today, tomorrow, next Friday) into an ISO 8601 date using the current date provided in the system prompt.",
    input_schema: {
      type: "object",
      properties: {
        title: { type: "string", description: "Short imperative title of the task." },
        notes: { type: "string", description: "Optional extra detail." },
        area: {
          type: "string",
          description: "Name of the life area this belongs to (e.g. 'Business'). Matched case-insensitively; omit if unclear.",
        },
        project: { type: "string", description: "Optional project name within the area." },
        dueDate: { type: "string", description: "Optional due date/time in ISO 8601, e.g. 2026-06-20 or 2026-06-20T17:00:00Z." },
        priority: { type: "string", enum: ["low", "normal", "high", "urgent"] },
        estimateMinutes: {
          type: "integer",
          description:
            "Your best estimate of how many minutes this will take. ALWAYS provide one. If a task feels larger than ~45 minutes, prefer creating smaller sub-tasks instead.",
        },
      },
      required: ["title"],
      additionalProperties: false,
    },
  },
  {
    name: "list_tasks",
    description: "List tasks, optionally filtered. Use this to answer questions about what is due or outstanding.",
    input_schema: {
      type: "object",
      properties: {
        filter: {
          type: "string",
          enum: ["today", "overdue", "upcoming", "open", "all"],
          description: "today = due today; overdue = past due & open; upcoming = next 7 days; open = all not-done; all = everything.",
        },
        area: { type: "string", description: "Optional area name to restrict to." },
      },
      additionalProperties: false,
    },
  },
  {
    name: "complete_task",
    description: "Mark a task done. Provide the task id when known, otherwise a title fragment to match against open tasks.",
    input_schema: {
      type: "object",
      properties: {
        id: { type: "string" },
        titleMatch: { type: "string", description: "Substring of the task title to match (case-insensitive)." },
      },
      additionalProperties: false,
    },
  },
  {
    name: "update_task",
    description: "Update fields of an existing task by id.",
    input_schema: {
      type: "object",
      properties: {
        id: { type: "string" },
        title: { type: "string" },
        notes: { type: "string" },
        area: { type: "string" },
        dueDate: { type: "string", description: "ISO 8601, or empty string to clear." },
        priority: { type: "string", enum: ["low", "normal", "high", "urgent"] },
      },
      required: ["id"],
      additionalProperties: false,
    },
  },
];

// ---------------------------------------------------------------------------
// Tool executors
// ---------------------------------------------------------------------------

async function resolveAreaId(name?: string): Promise<string | null> {
  if (!name) return null;
  const areas = await prisma.area.findMany();
  const match = areas.find((a) => a.name.toLowerCase() === name.toLowerCase());
  if (match) return match.id;
  // create on the fly so capture never silently drops an area
  const created = await prisma.area.create({ data: { name: name.trim() } });
  return created.id;
}

async function resolveProjectId(name: string | undefined, areaId: string | null): Promise<string | null> {
  if (!name) return null;
  const existing = await prisma.project.findFirst({
    where: { name: { equals: name, mode: "insensitive" } },
  });
  if (existing) return existing.id;
  const created = await prisma.project.create({ data: { name: name.trim(), areaId: areaId ?? undefined } });
  return created.id;
}

function parseDate(value?: string): Date | null | undefined {
  if (value === undefined) return undefined;
  if (value === "") return null;
  const d = new Date(value);
  return isNaN(d.getTime()) ? undefined : d;
}

type ToolInput = Record<string, unknown>;

async function executeTool(name: string, input: ToolInput, source: string): Promise<string> {
  switch (name) {
    case "list_areas": {
      const areas = await prisma.area.findMany({ orderBy: { sortOrder: "asc" } });
      return JSON.stringify(areas.map((a) => ({ id: a.id, name: a.name })));
    }
    case "create_task": {
      const areaId = await resolveAreaId(input.area as string | undefined);
      const projectId = await resolveProjectId(input.project as string | undefined, areaId);
      const task = await prisma.task.create({
        data: {
          title: String(input.title),
          notes: (input.notes as string) || null,
          areaId: areaId ?? undefined,
          projectId: projectId ?? undefined,
          dueDate: parseDate(input.dueDate as string) ?? undefined,
          priority: (input.priority as string) || "normal",
          estimateMinutes: typeof input.estimateMinutes === "number" ? input.estimateMinutes : undefined,
          source,
        },
        include: { area: true },
      });
      return JSON.stringify({
        ok: true,
        id: task.id,
        title: task.title,
        area: task.area?.name ?? null,
        dueDate: task.dueDate,
        priority: task.priority,
        estimateMinutes: task.estimateMinutes,
      });
    }
    case "list_tasks": {
      const filter = (input.filter as string) || "open";
      const areaId = await resolveAreaId(input.area as string | undefined);
      const now = new Date();
      const startOfDay = new Date(now);
      startOfDay.setHours(0, 0, 0, 0);
      const endOfDay = new Date(now);
      endOfDay.setHours(23, 59, 59, 999);
      const in7 = new Date(startOfDay);
      in7.setDate(in7.getDate() + 7);

      const where: Record<string, unknown> = {};
      if (areaId) where.areaId = areaId;
      if (filter === "today") {
        where.status = "open";
        where.dueDate = { gte: startOfDay, lte: endOfDay };
      } else if (filter === "overdue") {
        where.status = "open";
        where.dueDate = { lt: startOfDay };
      } else if (filter === "upcoming") {
        where.status = "open";
        where.dueDate = { gte: startOfDay, lte: in7 };
      } else if (filter === "open") {
        where.status = "open";
      }

      const tasks = await prisma.task.findMany({
        where,
        include: { area: true },
        orderBy: [{ dueDate: "asc" }, { createdAt: "desc" }],
        take: 50,
      });
      return JSON.stringify(
        tasks.map((t) => ({
          id: t.id,
          title: t.title,
          area: t.area?.name ?? null,
          status: t.status,
          priority: t.priority,
          dueDate: t.dueDate,
          estimateMinutes: t.estimateMinutes,
          startedAt: t.startedAt,
        }))
      );
    }
    case "complete_task": {
      let id = input.id as string | undefined;
      if (!id && input.titleMatch) {
        const t = await prisma.task.findFirst({
          where: { status: "open", title: { contains: String(input.titleMatch), mode: "insensitive" } },
          orderBy: { createdAt: "desc" },
        });
        id = t?.id;
      }
      if (!id) return JSON.stringify({ ok: false, error: "No matching open task found." });
      const task = await prisma.task.update({
        where: { id },
        data: { status: "done", completedAt: new Date() },
      });
      const points = await awardCompletion(task.id, task.priority, task.estimateMinutes);
      return JSON.stringify({ ok: true, id: task.id, title: task.title, status: task.status, pointsAwarded: points });
    }
    case "update_task": {
      const id = String(input.id);
      const areaId = input.area !== undefined ? await resolveAreaId(input.area as string) : undefined;
      const data: Record<string, unknown> = {};
      if (input.title !== undefined) data.title = input.title;
      if (input.notes !== undefined) data.notes = input.notes;
      if (areaId !== undefined) data.areaId = areaId;
      if (input.priority !== undefined) data.priority = input.priority;
      const due = parseDate(input.dueDate as string);
      if (due !== undefined) data.dueDate = due;
      const task = await prisma.task.update({ where: { id }, data });
      return JSON.stringify({ ok: true, id: task.id, title: task.title });
    }
    default:
      return JSON.stringify({ ok: false, error: `Unknown tool ${name}` });
  }
}

// ---------------------------------------------------------------------------
// Agent loop
// ---------------------------------------------------------------------------

function systemPrompt(extra: string): string {
  const today = new Date().toISOString();
  return [
    "You are the user's personal daily organizer assistant and a supportive ADHD coach. You help manage tasks across their business, investment properties, and personal life.",
    `The current date/time is ${today} (UTC). Resolve relative dates against this.`,
    "When the user describes something to do, create a task for it using the create_task tool. Infer the area and a due date when implied, and ALWAYS include a realistic time estimate (estimateMinutes).",
    "ADHD support: keep tasks small and concrete. If something would take more than ~45 minutes or has multiple steps, break it into smaller sub-tasks the user can start in one sitting. Frame the very first step as something tiny and unambiguous to lower the barrier to starting.",
    "Keep replies short, warm, and encouraging — confirm what you did in one or two sentences, and celebrate progress. Do not narrate tool calls or lecture.",
    extra,
  ]
    .filter(Boolean)
    .join("\n\n");
}

async function runAgent(
  system: string,
  history: Anthropic.MessageParam[],
  source: string
): Promise<{ text: string; messages: Anthropic.MessageParam[] }> {
  const anthropic = client();
  const messages: Anthropic.MessageParam[] = [...history];

  for (let i = 0; i < 8; i++) {
    const response = await anthropic.messages.create({
      model: MODEL,
      max_tokens: 16000,
      system,
      tools,
      messages,
    });

    messages.push({ role: "assistant", content: response.content });

    if (response.stop_reason !== "tool_use") {
      const text = response.content
        .filter((b): b is Anthropic.TextBlock => b.type === "text")
        .map((b) => b.text)
        .join("\n")
        .trim();
      return { text, messages };
    }

    const toolResults: Anthropic.ToolResultBlockParam[] = [];
    for (const block of response.content) {
      if (block.type === "tool_use") {
        let result: string;
        try {
          result = await executeTool(block.name, block.input as ToolInput, source);
        } catch (err) {
          result = JSON.stringify({ ok: false, error: (err as Error).message });
        }
        toolResults.push({ type: "tool_result", tool_use_id: block.id, content: result });
      }
    }
    messages.push({ role: "user", content: toolResults });
  }

  return { text: "I wasn't able to finish that — please try rephrasing.", messages };
}

// ---------------------------------------------------------------------------
// Public entry points
// ---------------------------------------------------------------------------

/** A single turn of the web chat assistant. `history` is prior turns. */
export async function chatTurn(
  history: { role: "user" | "assistant"; content: string }[],
  userText: string
): Promise<string> {
  const messages: Anthropic.MessageParam[] = history.map((m) => ({
    role: m.role,
    content: m.content,
  }));
  messages.push({ role: "user", content: userText });
  const { text } = await runAgent(systemPrompt(""), messages, "chat");
  return text;
}

/**
 * Process a free-text capture from a remote channel (SMS, email, quick-add).
 * Returns a short confirmation suitable for replying back to the user.
 */
export async function captureText(
  channel: "sms" | "email" | "quick",
  rawText: string,
  fromAddr?: string
): Promise<string> {
  const extra =
    "This message arrived from a remote capture channel (text/email/quick-add). Turn it into one or more tasks. Reply with a brief confirmation of what you added, e.g. 'Added: Call the plumber (Investment Properties, due tomorrow).' If there is nothing actionable, say so briefly.";
  const messages: Anthropic.MessageParam[] = [{ role: "user", content: rawText }];
  const { text } = await runAgent(systemPrompt(extra), messages, channel);

  await prisma.captureLog.create({
    data: { channel, rawText, fromAddr: fromAddr || null, summary: text },
  });

  return text || "Got it.";
}

/**
 * ADHD focus coach: pick the single best task to do right now and make starting easy.
 * Returns a short recommendation (markdown-ish plain text).
 */
export async function coachFocus(availableMinutes?: number, energy?: string): Promise<string> {
  const extra = [
    "The user has ADHD and struggles with prioritization and task initiation. They are asking what to focus on RIGHT NOW.",
    "Use list_tasks (today, overdue, then open) to see what's on their plate, then recommend EXACTLY ONE task to start.",
    "Respond in this shape, briefly:",
    "1) The one task to do now (and which area), with its time estimate.",
    "2) One sentence on why it's the right pick (due date / impact / quick win).",
    "3) A 2-minute first step — concrete and tiny — so they can start immediately without deciding anything else.",
    "Be warm and momentum-building. Do not list everything; the whole point is to remove the choice for them.",
  ].join(" ");

  const ctx: string[] = ["What should I focus on right now?"];
  if (availableMinutes) ctx.push(`I have about ${availableMinutes} minutes.`);
  if (energy) ctx.push(`My energy is ${energy}.`);

  const messages: Anthropic.MessageParam[] = [{ role: "user", content: ctx.join(" ") }];
  const { text } = await runAgent(systemPrompt(extra), messages, "chat");
  return text || "Pick the smallest open task and do just the first 2 minutes of it.";
}
