import { NextRequest, NextResponse } from "next/server";
import { isAuthed } from "@/lib/auth";
import { captureText } from "@/lib/assistant";

export async function POST(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const body = await req.json();
  const text: string = body.text;
  if (!text) return NextResponse.json({ error: "text required" }, { status: 400 });
  const reply = await captureText("quick", text);
  return NextResponse.json({ reply });
}
