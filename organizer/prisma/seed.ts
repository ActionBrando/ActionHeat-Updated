import { PrismaClient } from "@prisma/client";

const prisma = new PrismaClient();

async function main() {
  // One seeded user + board (the Phase 1 single-tenant context).
  const user = await prisma.user.upsert({
    where: { email: process.env.OWNER_EMAIL || "owner@local" },
    update: {},
    create: { email: process.env.OWNER_EMAIL || "owner@local", name: "Owner" },
  });

  const board = await prisma.board.upsert({
    where: { slug: "my-organizer" },
    update: { ownerId: user.id },
    create: { name: "My Organizer", slug: "my-organizer", ownerId: user.id },
  });

  const areas = [
    { name: "Business", color: "#2563eb", sortOrder: 0 },
    { name: "Investment Properties", color: "#16a34a", sortOrder: 1 },
    { name: "Life", color: "#9333ea", sortOrder: 2 },
  ];

  for (const a of areas) {
    await prisma.area.upsert({
      where: { boardId_name: { boardId: board.id, name: a.name } },
      update: { color: a.color, sortOrder: a.sortOrder },
      create: { ...a, boardId: board.id },
    });
  }

  console.log(`Seeded board "${board.name}" with areas:`, areas.map((a) => a.name).join(", "));
}

main()
  .then(() => prisma.$disconnect())
  .catch(async (e) => {
    console.error(e);
    await prisma.$disconnect();
    process.exit(1);
  });
