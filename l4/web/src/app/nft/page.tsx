/** Galería y marketplace NFT. */
'use client';
import React from 'react';

const MOCK = [
  { id: 1, name: 'Genesis #1', price: '12 L1' },
  { id: 2, name: 'Genesis #2', price: '8 L1' },
  { id: 3, name: 'Genesis #3', price: '20 L1' },
];

export default function NftPage() {
  return (
    <div className="space-y-6">
      <h1 className="text-2xl font-semibold">NFT Marketplace</h1>
      <div className="grid gap-4 sm:grid-cols-3">
        {MOCK.map((n) => (
          <div key={n.id} className="border rounded-lg p-4">
            <div className="h-32 bg-slate-100 rounded mb-3" />
            <p className="font-medium">{n.name}</p>
            <p className="text-sm text-slate-500">{n.price}</p>
            <button className="mt-2 text-sm text-brand">Comprar</button>
          </div>
        ))}
      </div>
    </div>
  );
}
