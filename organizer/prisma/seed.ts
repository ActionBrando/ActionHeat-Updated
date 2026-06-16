import { PrismaClient } from "@prisma/client";

const prisma = new PrismaClient();

async function main() {
  const areas = [
    { name: "Business", color: "#2563eb", sortOrder: 0 },
    { name: "Investment Properties", color: "#16a34a", sortOrder: 1 },
    { name: "Life", color: "#9333ea", sortOrder: 2 },
  ];

  for (const a of areas) {
    await prisma.area.upsert({
      where: { name: a.name },
      update: { color: a.color, sortOrder: a.sortOrder },
      create: a,
    });
  }

  console.log("Seeded areas:", areas.map((a) => a.name).join(", "));
}

main()
  .then(() => prisma.$disconnect())
  .catch(async (e) => {
    console.error(e);
    await prisma.$disconnect();
    process.exit(1);
  });
