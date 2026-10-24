/** Interfaz de staking. */
'use client';
import React, { useState } from 'react';
import { useContract } from '../../hooks/useContract';

export default function StakePage() {
  const staking = useContract('staking');
  const [amt, setAmt] = useState('10');
  return (
    <div className="max-w-md space-y-4">
      <h1 className="text-2xl font-semibold">Staking</h1>
      <input className="w-full border rounded px-3 py-2" value={amt} onChange={(e) => setAmt(e.target.value)} />
      <div className="flex gap-2">
        <button className="bg-brand text-white px-4 py-2 rounded" onClick={() => staking.call('stake', [amt])}>Stake</button>
        <button className="border px-4 py-2 rounded" onClick={() => staking.call('claim', [])}>Claim</button>
      </div>
    </div>
  );
}
