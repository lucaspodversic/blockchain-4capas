/** Enviar transacciones. */
'use client';
import React, { useState } from 'react';
import { TxModal } from '../../components/TxModal';
import { useWallet } from '../../hooks/useWallet';
import { getRpc } from '../../lib/rpc';

export default function SendPage() {
  const w = useWallet();
  const [to, setTo] = useState('');
  const [amount, setAmount] = useState('0');
  const [txHash, setTxHash] = useState<string | null>(null);
  const [open, setOpen] = useState(false);

  async function submit(e: React.FormEvent) {
    e.preventDefault();
    setOpen(true);
    const rpc = getRpc();
    const hash = await rpc.sendTransaction({ from: w.address, to, value: amount });
    setTxHash(hash);
  }

  return (
    <div className="max-w-md space-y-4">
      <h1 className="text-2xl font-semibold">Enviar</h1>
      <form onSubmit={submit} className="space-y-3">
        <input className="w-full border rounded px-3 py-2" placeholder="0x…" value={to} onChange={(e) => setTo(e.target.value)} />
        <input className="w-full border rounded px-3 py-2" placeholder="Monto" value={amount} onChange={(e) => setAmount(e.target.value)} />
        <button type="submit" className="bg-brand text-white px-4 py-2 rounded">Enviar</button>
      </form>
      <TxModal open={open} hash={txHash} onClose={() => setOpen(false)} />
    </div>
  );
}
