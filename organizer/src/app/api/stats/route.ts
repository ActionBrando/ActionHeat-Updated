import { NextResponse } from "next/server";
import { isAuthed } from "@/lib/auth";
import { getStats } from "@/lib/gamify";
import { getTenant } from "@/lib/tenant";

export async function GET() {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const { boardId } = await getTenant();
  return NextResponse.json(await getStats(boardId));
}
