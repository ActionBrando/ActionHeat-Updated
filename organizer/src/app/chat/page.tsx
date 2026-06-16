"use client";

import { useState, useRef, useEffect } from "react";
import Nav from "@/components/Nav";

type Msg = { role: "user" | "assistant"; content: string };

export default function ChatPage() {
  const [messages, setMessages] = useState<Msg[]>([
    {
      role: "assistant",
      content:
        "Hi! I'm your organizer assistant and ADHD coach. Brain-dump what's on your plate — e.g. \"renew the Oak St lease next Friday\" — ask me what to prioritize, or say \"help me start\" and I'll break the next task into a tiny first step. (The dashboard's 🎯 button does this too.)",
    },
  ]);
  const [input, setInput] = useState("");
  const [busy, setBusy] = useState(false);
  const endRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    endRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages, busy]);

  async function send(e: React.FormEvent) {
    e.preventDefault();
    const text = input.trim();
    if (!text || busy) return;
    setInput("");
    const history = messages.filter((m, i) => !(i === 0 && m.role === "assistant"));
    const next = [...messages, { role: "user" as const, content: text }];
    setMessages(next);
    setBusy(true);

    const res = await fetch("/api/chat", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ message: text, history }),
    });
    setBusy(false);

    if (res.ok) {
      const { reply } = await res.json();
      setMessages((m) => [...m, { role: "assistant", content: reply }]);
    } else {
      setMessages((m) => [...m, { role: "assistant", content: "Something went wrong. Please try again." }]);
    }
  }

  return (
    <main className="flex h-screen flex-col">
      <Nav active="chat" />
      <div className="mx-auto flex w-full max-w-2xl flex-1 flex-col overflow-hidden p-4">
        <div className="flex-1 space-y-3 overflow-y-auto pb-4">
          {messages.map((m, i) => (
            <div key={i} className={`flex ${m.role === "user" ? "justify-end" : "justify-start"}`}>
              <div
                className={`max-w-[80%] whitespace-pre-wrap rounded-2xl px-4 py-2 text-sm ${
                  m.role === "user" ? "bg-slate-900 text-white" : "border border-slate-200 bg-white"
                }`}
              >
                {m.content}
              </div>
            </div>
          ))}
          {busy && (
            <div className="flex justify-start">
              <div className="rounded-2xl border border-slate-200 bg-white px-4 py-2 text-sm text-slate-400">
                Thinking…
              </div>
            </div>
          )}
          <div ref={endRef} />
        </div>
        <form onSubmit={send} className="flex gap-2 border-t border-slate-200 pt-3">
          <input
            value={input}
            onChange={(e) => setInput(e.target.value)}
            placeholder="Message your organizer…"
            className="flex-1 rounded-full border border-slate-300 px-4 py-2 text-sm outline-none focus:border-slate-900"
          />
          <button
            disabled={busy}
            className="rounded-full bg-slate-900 px-5 py-2 text-sm font-medium text-white disabled:opacity-50"
          >
            Send
          </button>
        </form>
      </div>
    </main>
  );
}
