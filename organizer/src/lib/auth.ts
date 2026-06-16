import { cookies } from "next/headers";

export const SESSION_COOKIE = "organizer_session";

/**
 * The cookie value we set on successful login. For a single-user personal tool
 * the session secret itself is the bearer token — keep SESSION_SECRET private.
 */
export function sessionToken(): string {
  const secret = process.env.SESSION_SECRET;
  if (!secret) throw new Error("SESSION_SECRET is not configured");
  return secret;
}

export function checkPassword(password: string): boolean {
  const expected = process.env.APP_PASSWORD;
  if (!expected) throw new Error("APP_PASSWORD is not configured");
  // constant-time-ish compare
  if (password.length !== expected.length) return false;
  let mismatch = 0;
  for (let i = 0; i < password.length; i++) {
    mismatch |= password.charCodeAt(i) ^ expected.charCodeAt(i);
  }
  return mismatch === 0;
}

/** Server-side guard for API route handlers (pages are gated by middleware). */
export async function isAuthed(): Promise<boolean> {
  const store = await cookies();
  return store.get(SESSION_COOKIE)?.value === sessionToken();
}
