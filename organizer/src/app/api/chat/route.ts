import { NextRequest, NextResponse } from "next/server";
import { isAuthed } from "@/lib/auth";
import { chatTurn } from "@/lib/assistant";
import { prisma } from "@/lib/db";

export async function POST(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });

  const body = await req.json();
  const message: string = body.message;
  const history: { role: "user" | "assistant"; content: string }[] = Array.isArray(body.history)
    ? body.history
    : [];
  if (!message) return NextResponse.json({ error: "message required" }, { status: 400 });

  const reply = await chatTurn(history, message);

  await prisma.chatMessage.createMany({
    data: [
      { role: "user", content: message },
      { role: "assistant", content: reply },
    ],
  });

  return NextResponse.json({ reply });
}
