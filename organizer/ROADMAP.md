# Roadmap

## Phase 1 — Personal (current)

Single-user app: password login, your own tasks across Business / Investment
Properties / Life, AI chat + SMS/email capture, ADHD focus coach, and
gamification (XP, levels, daily streak, points for starting tasks).

## Phase 2 — Public / multi-user

Goal: turn the single-user tool into a multi-user product with **leaderboards**
and **boards that can be private, shared with a team, or public.**

### 1. Authentication

Replace the single shared password with real accounts. Recommended:

- **Auth.js (NextAuth)** with email magic-link + Google/Apple OAuth — open source,
  no per-seat cost, integrates cleanly with the existing Next.js app and Prisma
  (via `@auth/prisma-adapter`).
- Alternative if you want it turnkey: **Clerk** or **Supabase Auth** (faster to
  ship, hosted UI, free tier, paid above it).

### 2. Multi-tenant data model

The Phase 1 schema is single-tenant. Phase 2 introduces ownership and scoping.
New/changed models:

```
User            id, email, name, image, createdAt
Board           id, name, slug, ownerId,
                visibility: "private" | "team" | "public"
Membership      userId, boardId, role: "owner" | "member" | "viewer"
Area            + boardId            (areas live inside a board)
Project         (inherits board via area)
Task            + boardId, + createdById, + assigneeId?
PointEvent      + userId, + boardId  (enables per-user & per-board scoring)
CaptureRoute    userId, channel ("sms"|"email"), address (phone/from-addr)
                — maps an inbound SMS number / email sender to a user + board
```

Everything task-related is scoped by `boardId`; every query filters by the
caller's board memberships.

### 3. Boards & visibility

- **Private** — only the owner.
- **Team** — owner + invited members (via `Membership`, roles control edit vs view).
- **Public** — read-only at `/b/[slug]`, indexable, no login required to view.

A user can belong to many boards (personal board + a team board + …) and switch
between them. Default personal board is created on signup.

### 4. Leaderboards

Driven entirely by `PointEvent` (already the ledger in Phase 1):

- **Board leaderboard** — sum points per `userId` within a board, by window
  (today / this week / all-time). Shown on team & public boards.
- **Global leaderboard** — opt-in, across users who made their stats public.
- Streaks and "tasks started" make good secondary leaderboards (rewards
  initiation, which is the ADHD-relevant metric).
- Compute on the fly for small N; add a periodically-refreshed
  `LeaderboardSnapshot` table if it gets large.

### 5. Per-user capture (SMS / email)

Today the webhook attributes everything to the single user. In Phase 2 the
inbound `From` (phone) / sender (email) is looked up in `CaptureRoute` to find
the user + their default capture board. Users register their number/email in
settings; unknown senders get a "reply to link your number" flow.

### 6. Gamification extensions (fits "Organizer-game")

- Achievements / badges (first 7-day streak, 100 tasks started, etc.).
- Daily quests ("start 3 tasks today").
- XP multipliers that scale with streak length.
- A "boss task" concept for the big avoided thing, worth bonus XP.

## Migration note (Phase 1 → 2)

The jump is mostly **additive**, not a rewrite:

1. Add `User`, `Board`, `Membership`, `CaptureRoute`; seed one user + one board
   from the existing data.
2. Add nullable `boardId` / `userId` to `Area`, `Task`, `PointEvent`; backfill
   to the seeded board/user; then make them required.
3. Swap auth; gate every query by board membership.

**To minimize rework, Phase 1 can optionally ship the `Board` + `User` seam now**
(one seeded user, one seeded board, `boardId`/`userId` carried on tasks and
point events from day one). That makes Phase 2 almost purely auth + sharing UI.
This is the recommended approach given Phase 2 is planned.
