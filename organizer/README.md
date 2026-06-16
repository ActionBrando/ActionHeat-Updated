# Daily Organizer

A personal organizer for managing your **business, investment properties, and life** — with an
AI chat assistant, a task dashboard, and remote capture by **SMS** and **email**. Built as a
single Next.js app that deploys to the cloud (Vercel + a hosted Postgres) and installs on your
phone as a PWA.

## What you get

- **Dashboard** (`/`) — tasks grouped by Overdue / Today / Next 7 days / No date, with quick-add.
- **Chat** (`/chat`) — talk to a Claude-powered assistant that creates, completes, and lists tasks
  using tool calls. e.g. _"remind me to renew the Oak St lease next Friday"_ or _"what's overdue?"_
- **Quick add** (`/capture`) — brain-dump free text; the assistant sorts it into tasks, areas, and
  due dates. Also the target for the phone **Share** sheet (PWA share target).
- **SMS capture** (`/api/sms`) — text a Twilio number; the message is parsed into tasks and you get
  a confirmation reply.
- **Email capture** (`/api/email`) — forward/send email to an inbound-parse address; it becomes tasks.

### Built for ADHD

This is tuned for poor time management, prioritization, and task initiation:

- **"What should I do right now?"** — one button on the dashboard asks the coach to pick the single
  best task (by due date, impact, and quick-win value) and break it into a **2-minute first step**,
  so there's no decision to make and almost no barrier to starting.
- **Time estimates** — the assistant attaches a realistic `~Xm` estimate to every task and breaks
  anything over ~45 minutes into smaller sub-tasks you can finish in one sitting.
- **Gamification** — XP and levels, a daily **🔥 streak**, and a points-for-progress bar. Crucially,
  you earn XP for **Start**ing a task (+5), not only finishing it — because initiation is the hard
  part. Completing awards more, scaled by priority and size.

Everything funnels through one pipeline (`src/lib/assistant.ts`), so adding a channel is just a thin
adapter that calls `captureText()`.

## Architecture

| Piece            | Choice                                              |
| ---------------- | --------------------------------------------------- |
| App framework    | Next.js 15 (App Router, TypeScript)                 |
| Database         | PostgreSQL via Prisma                               |
| AI               | Anthropic Claude (`claude-opus-4-8` by default) with tool use |
| Auth             | Single-user password → signed session cookie + middleware |
| Messaging in     | Twilio (SMS), any inbound-email webhook (SendGrid/Postmark) |
| Install          | PWA (installable, Android share target)             |

Data model (`prisma/schema.prisma`): **Area** (Business / Investment Properties / Life) → **Project**
→ **Task**, plus **CaptureLog** (audit of remote captures) and **ChatMessage** (chat history).

---

## Local development

```bash
cd organizer
cp .env.example .env        # then fill in the values (see below)
npm install
npm run db:push             # create tables in your DATABASE_URL
npm run db:seed             # seed the three default areas
npm run dev                 # http://localhost:3000
```

You need, at minimum: `DATABASE_URL`, `APP_PASSWORD`, `SESSION_SECRET`, `ANTHROPIC_API_KEY`.
For a free local Postgres, use Neon (below) or Docker (`postgres:16`).

---

## Deploy (cloud + hosted DB)

### 1. Database — Neon (free tier)

1. Create a project at [neon.tech](https://neon.tech).
2. Copy the pooled connection string into `DATABASE_URL` (it ends with `?sslmode=require`).

### 2. App — Vercel

1. Push this repo and import it in Vercel. Set the project **Root Directory** to `organizer`.
2. Add the environment variables from `.env.example` in **Project → Settings → Environment Variables**:
   `DATABASE_URL`, `APP_PASSWORD`, `SESSION_SECRET`, `ANTHROPIC_API_KEY`, `WEBHOOK_TOKEN`
   (and the optional Twilio vars).
3. Deploy. The build runs `prisma generate && next build`.
4. After the first deploy, create the tables and seed once. Easiest from your machine with the
   production `DATABASE_URL` exported:
   ```bash
   DATABASE_URL="<neon-url>" npm run db:push
   DATABASE_URL="<neon-url>" npm run db:seed
   ```

Generate the secrets with:
```bash
openssl rand -hex 32   # SESSION_SECRET
openssl rand -hex 24   # WEBHOOK_TOKEN
```

### 3. SMS — Twilio

1. Buy an SMS-capable number in the Twilio console.
2. Under the number's **Messaging → "A message comes in"**, set:
   - Webhook: `https://YOUR_HOST/api/sms?token=YOUR_WEBHOOK_TOKEN`
   - Method: **HTTP POST**
3. Text the number. You'll get a confirmation reply via TwiML — no Twilio credentials needed for
   inbound. (The `TWILIO_*` vars are only for future outbound/proactive messages.)

> Security: the webhook is gated by `?token=...`. For stricter protection, add Twilio request
> signature validation in `src/app/api/sms/route.ts`.

### 4. Email — inbound parse (optional)

Point any inbound-email provider at `https://YOUR_HOST/api/email?token=YOUR_WEBHOOK_TOKEN`:
- **SendGrid Inbound Parse** posts multipart form fields `from`, `subject`, `text`.
- **Postmark** posts JSON `From`, `Subject`, `TextBody`.

Both shapes are handled.

### 5. iMessage (note)

iMessage has **no official API**. To send blue-bubble messages to the organizer you'd need either:
- a **Mac running 24/7** with a bridge such as [BlueBubbles](https://bluebubbles.app/), or
- a **paid service** like LoopMessage or Sendblue.

Either one would post into `/api/sms` (or a new `/api/imessage` adapter) and reuse the same
`captureText()` pipeline. SMS via Twilio is the reliable, no-Mac path and is what's wired up here.

### 6. Install on your phone (PWA)

Open the deployed URL on your phone → **Add to Home Screen**. On Android you'll also get an
"Organizer" entry in the system **Share** sheet that drops shared text/links into Quick add.

---

## Project layout

```
organizer/
  prisma/schema.prisma      data model
  prisma/seed.ts            seeds default areas
  src/lib/db.ts             Prisma client singleton
  src/lib/auth.ts           password + session helpers
  src/lib/assistant.ts      Claude tool definitions, agent loop, capture pipeline
  src/middleware.ts         auth gate (pages + APIs; webhooks excluded)
  src/app/                  pages (dashboard, chat, capture, login) + API routes
  src/components/Nav.tsx
  public/manifest.webmanifest  PWA + share target
```

## Notes & next steps

- The assistant model defaults to `claude-opus-4-8`; override with `ANTHROPIC_MODEL`.
- Chat history is persisted (`ChatMessage`) but the chat UI keeps state per session; wire it to
  load history on mount if you want continuity across reloads.
- Good follow-ups: recurring tasks, calendar sync, a per-area view page, and proactive morning
  SMS digests (the `TWILIO_*` vars are already reserved for outbound).
