import { prisma } from "./db";

/**
 * Phase 1 is single-user, but the schema carries boardId/userId so Phase 2
 * (multi-user, shared/public boards, leaderboards) is additive rather than a
 * rewrite. This resolves the one default user + board, creating them on first
 * use. In Phase 2 this is replaced by the authenticated user's board context.
 */
export async function getTenant(): Promise<{ userId: string; boardId: string }> {
  let board = await prisma.board.findFirst({ orderBy: { createdAt: "asc" }, include: { owner: true } });

  if (!board) {
    const user =
      (await prisma.user.findFirst()) ??
      (await prisma.user.create({
        data: { email: process.env.OWNER_EMAIL || "owner@local", name: "Owner" },
      }));
    board = await prisma.board.create({
      data: { name: "My Organizer", slug: "my-organizer", ownerId: user.id },
      include: { owner: true },
    });
  }

  let userId = board.ownerId;
  if (!userId) {
    const user =
      (await prisma.user.findFirst()) ??
      (await prisma.user.create({
        data: { email: process.env.OWNER_EMAIL || "owner@local", name: "Owner" },
      }));
    await prisma.board.update({ where: { id: board.id }, data: { ownerId: user.id } });
    userId = user.id;
  }

  return { userId, boardId: board.id };
}
