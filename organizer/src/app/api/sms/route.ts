import { NextRequest, NextResponse } from "next/server";
import { captureText } from "@/lib/assistant";

function xmlEscape(s: string): string {
  return s
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&apos;");
}

function twiml(message: string): NextResponse {
  const body = `<?xml version="1.0" encoding="UTF-8"?><Response><Message>${xmlEscape(message)}</Message></Response>`;
  return new NextResponse(body, { status: 200, headers: { "Content-Type": "text/xml" } });
}

/**
 * Twilio inbound SMS webhook.
 * Configure the number's "A message comes in" webhook to:
 *   https://YOUR_HOST/api/sms?token=YOUR_WEBHOOK_TOKEN   (HTTP POST)
 */
export async function POST(req: NextRequest) {
  const { searchParams } = new URL(req.url);
  if (!process.env.WEBHOOK_TOKEN || searchParams.get("token") !== process.env.WEBHOOK_TOKEN) {
    return new NextResponse("Forbidden", { status: 403 });
  }

  const form = await req.formData();
  const text = String(form.get("Body") || "").trim();
  const from = String(form.get("From") || "");
  if (!text) return twiml("Send me something to add to your organizer.");

  try {
    const reply = await captureText("sms", text, from);
    return twiml(reply);
  } catch (err) {
    console.error("SMS capture failed", err);
    return twiml("Sorry — I couldn't add that just now. Try again shortly.");
  }
}
