import { NextRequest, NextResponse } from "next/server";
import { isAuthed } from "@/lib/auth";
import { coachFocus } from "@/lib/assistant";

export async function POST(req: NextRequest) {
  if (!(await isAuthed())) return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  const body = await req.json().catch(() => ({}));
  const minutes = typeof body.availableMinutes === "number" ? body.availableMinutes : undefined;
  const energy = typeof body.energy === "string" ? body.energy : undefined;
  const recommendation = await coachFocus(minutes, energy);
  return NextResponse.json({ recommendation });
}
