import { NextRequest, NextResponse } from "next/server";
import { isAuthed } from "@/lib/auth";
import { breakdownTask } from "@/lib/assistant";
import { prisma } from "@/lib/db";
import { getTenant } from "@/lib/tenant";

export async function POST(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const { boardId } = await getTenant();
  const body = await req.json();
  if (!body.taskId) return NextResponse.json({ error: "taskId required" }, { status: 400 });

  const message = await breakdownTask(String(body.taskId));
  const children = await prisma.task.findMany({
    where: { parentId: String(body.taskId), boardId },
    orderBy: { sortOrder: "asc" },
  });
  return NextResponse.json({ message, children });
}
