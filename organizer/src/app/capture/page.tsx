"use client";

import { useEffect, useState } from "react";
import Nav from "@/components/Nav";

export default function CapturePage() {
  const [text, setText] = useState("");
  const [busy, setBusy] = useState(false);
  const [result, setResult] = useState("");

  // Prefill from a PWA share-target (Android "Share to Organizer") or ?text= link.
  useEffect(() => {
    const p = new URLSearchParams(window.location.search);
    const shared = [p.get("title"), p.get("text"), p.get("url")].filter(Boolean).join(" ").trim();
    if (shared) setText(shared);
  }, []);

  async function submit(e: React.FormEvent) {
    e.preventDefault();
    const t = text.trim();
    if (!t || busy) return;
    setBusy(true);
    setResult("");
    const res = await fetch("/api/capture", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ text: t }),
    });
    setBusy(false);
    if (res.ok) {
      const { reply } = await res.json();
      setResult(reply);
      setText("");
    } else {
      setResult("Something went wrong. Please try again.");
    }
  }

  return (
    <main>
      <Nav active="capture" />
      <div className="mx-auto max-w-xl p-4">
        <h1 className="mb-1 text-lg font-semibold">Quick add</h1>
        <p className="mb-4 text-sm text-slate-500">
          Brain-dump anything — one item or a list. The assistant sorts it into tasks, areas, and due dates for you.
        </p>
        <form onSubmit={submit}>
          <textarea
            value={text}
            onChange={(e) => setText(e.target.value)}
            rows={5}
            autoFocus
            placeholder={"e.g. Pay property taxes by the 30th; call accountant about the LLC; pick up dry cleaning tomorrow"}
            className="mb-3 w-full rounded-lg border border-slate-300 p-3 text-sm outline-none focus:border-slate-900"
          />
          <button
            disabled={busy}
            className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white disabled:opacity-50"
          >
            {busy ? "Adding…" : "Add to organizer"}
          </button>
        </form>
        {result && (
          <div className="mt-4 rounded-lg border border-green-200 bg-green-50 p-3 text-sm text-green-800">
            {result}
          </div>
        )}
      </div>
    </main>
  );
}
