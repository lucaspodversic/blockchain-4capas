/** Búsqueda de bloque, tx o dirección. */
'use client';
import React, { useState } from 'react';
import { useRouter } from 'next/navigation';

export function SearchBar() {
  const [q, setQ] = useState('');
  const router = useRouter();
  function go(e: React.FormEvent) {
    e.preventDefault();
    if (/^\d+$/.test(q)) router.push(`/explorer/block/${q}`);
    else if (q.startsWith('0x') && q.length === 42) router.push(`/explorer/address/${q}`);
    else if (q.startsWith('0x')) router.push(`/explorer/tx/${q}`);
  }
  return (
    <form onSubmit={go} className="flex gap-2">
      <input
        className="flex-1 rounded border px-3 py-2"
        placeholder="Bloque, tx o dirección"
        value={q}
        onChange={(e) => setQ(e.target.value)}
      />
      <button className="rounded bg-brand px-4 text-white">Buscar</button>
    </form>
  );
}
