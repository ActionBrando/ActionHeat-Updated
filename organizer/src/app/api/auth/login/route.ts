import { NextRequest, NextResponse } from "next/server";
import { SESSION_COOKIE, checkPassword, sessionToken } from "@/lib/auth";

export async function POST(req: NextRequest) {
  let password = "";
  const contentType = req.headers.get("content-type") || "";
  if (contentType.includes("application/json")) {
    const body = await req.json().catch(() => ({}));
    password = body.password || "";
  } else {
    const form = await req.formData();
    password = String(form.get("password") || "");
  }

  if (!checkPassword(password)) {
    return NextResponse.json({ error: "Incorrect password" }, { status: 401 });
  }

  const res = NextResponse.json({ ok: true });
  res.cookies.set(SESSION_COOKIE, sessionToken(), {
    httpOnly: true,
    sameSite: "lax",
    secure: process.env.NODE_ENV === "production",
    path: "/",
    maxAge: 60 * 60 * 24 * 30,
  });
  return res;
}
