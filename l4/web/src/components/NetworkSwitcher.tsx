/** Cambio entre L1 / L2 / testnet. */
'use client';
import React from 'react';
import { useChain } from '../hooks/useChain';

const NETS = ['L1 Mainnet', 'L2 Rollup', 'Devnet'] as const;

export function NetworkSwitcher() {
  const chain = useChain();
  return (
    <select
      className="rounded border px-2 py-1 text-sm"
      value={chain.network}
      onChange={(e) => chain.setNetwork(e.target.value)}
    >
      {NETS.map((n) => (
        <option key={n} value={n}>{n}</option>
      ))}
    </select>
  );
}
