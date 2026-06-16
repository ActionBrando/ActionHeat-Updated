import { NextRequest, NextResponse } from "next/server";

const SESSION_COOKIE = "organizer_session";

// Paths that must remain reachable without a session.
// Webhooks (/api/sms, /api/email) authenticate themselves with WEBHOOK_TOKEN.
const PUBLIC_PREFIXES = ["/login", "/api/auth", "/api/sms", "/api/email", "/manifest.webmanifest"];

export function middleware(req: NextRequest) {
  const { pathname } = req.nextUrl;

  if (PUBLIC_PREFIXES.some((p) => pathname === p || pathname.startsWith(p + "/") || pathname.startsWith(p))) {
    return NextResponse.next();
  }

  const token = req.cookies.get(SESSION_COOKIE)?.value;
  if (token && token === process.env.SESSION_SECRET) {
    return NextResponse.next();
  }

  if (pathname.startsWith("/api/")) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  const loginUrl = new URL("/login", req.url);
  return NextResponse.redirect(loginUrl);
}

export const config = {
  // Run on everything except Next internals and static files.
  matcher: ["/((?!_next/static|_next/image|favicon.ico|icon.png|icon-192.png|icon-512.png).*)"],
};
