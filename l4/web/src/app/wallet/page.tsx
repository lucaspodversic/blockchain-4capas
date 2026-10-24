/** Vista de la wallet (balances, historial). */
'use client';
import React from 'react';
import { BalanceCard } from '../../components/BalanceCard';
import { TxTable } from '../../components/TxTable';
import { useWallet } from '../../hooks/useWallet';

export default function WalletPage() {
  const w = useWallet();
  return (
    <div className="space-y-6">
      <h1 className="text-2xl font-semibold">Wallet</h1>
      <p className="font-mono text-sm break-all">{w.address || '—'}</p>
      <BalanceCard label="L1" amount={w.balance} symbol="L1" />
      <TxTable rows={w.history} />
    </div>
  );
}
