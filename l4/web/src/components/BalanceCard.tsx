/** Tarjeta de balance. */
import React from 'react';

export function BalanceCard({ label, amount, symbol }: { label: string; amount: string; symbol: string }) {
  return (
    <div className="rounded-xl border bg-white p-4 shadow-sm">
      <p className="text-xs uppercase tracking-wide text-slate-500">{label}</p>
      <p className="mt-2 text-2xl font-semibold">
        {amount} {symbol}
      </p>
    </div>
  );
}
