import { NextRequest, NextResponse } from "next/server";
import { captureText } from "@/lib/assistant";

/**
 * Inbound email webhook. Works with providers that POST parsed email, e.g.
 * SendGrid Inbound Parse (multipart form: `from`, `subject`, `text`) or
 * Postmark (JSON: `From`, `Subject`, `TextBody`).
 * Point the provider at: https://YOUR_HOST/api/email?token=YOUR_WEBHOOK_TOKEN
 */
export async function POST(req: NextRequest) {
  const { searchParams } = new URL(req.url);
  if (!process.env.WEBHOOK_TOKEN || searchParams.get("token") !== process.env.WEBHOOK_TOKEN) {
    return new NextResponse("Forbidden", { status: 403 });
  }

  let from = "";
  let subject = "";
  let text = "";

  const contentType = req.headers.get("content-type") || "";
  if (contentType.includes("application/json")) {
    const b = await req.json().catch(() => ({}));
    from = b.from || b.From || "";
    subject = b.subject || b.Subject || "";
    text = b.text || b.TextBody || b.plain || "";
  } else {
    const form = await req.formData();
    from = String(form.get("from") || form.get("From") || "");
    subject = String(form.get("subject") || form.get("Subject") || "");
    text = String(form.get("text") || form.get("TextBody") || form.get("plain") || "");
  }

  const combined = [subject, text].filter(Boolean).join("\n").trim();
  if (!combined) return NextResponse.json({ ok: true, note: "empty email ignored" });

  try {
    const reply = await captureText("email", combined, from);
    return NextResponse.json({ ok: true, reply });
  } catch (err) {
    console.error("Email capture failed", err);
    return NextResponse.json({ ok: false, error: "capture failed" }, { status: 500 });
  }
}
