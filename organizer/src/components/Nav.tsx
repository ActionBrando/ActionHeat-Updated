import Link from "next/link";

export default function Nav({ active }: { active: "dashboard" | "chat" | "capture" }) {
  const link = (href: string, key: string, label: string) => (
    <Link
      href={href}
      className={`px-3 py-1.5 rounded-md text-sm font-medium ${
        active === key ? "bg-slate-900 text-white" : "text-slate-600 hover:bg-slate-200"
      }`}
    >
      {label}
    </Link>
  );

  return (
    <header className="flex items-center justify-between border-b border-slate-200 bg-white px-4 py-3">
      <div className="flex items-center gap-2">
        <span className="text-lg font-bold tracking-tight">📋 Organizer</span>
      </div>
      <nav className="flex items-center gap-1">
        {link("/", "dashboard", "Dashboard")}
        {link("/chat", "chat", "Chat")}
        {link("/capture", "capture", "Quick add")}
        <form action="/api/auth/logout" method="post">
          <button className="ml-2 px-3 py-1.5 rounded-md text-sm text-slate-500 hover:bg-slate-200">
            Sign out
          </button>
        </form>
      </nav>
    </header>
  );
}
