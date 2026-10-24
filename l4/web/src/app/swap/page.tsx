/** Interfaz del DEX. */
'use client';
import React, { useState } from 'react';
import { useContract } from '../../hooks/useContract';

export default function SwapPage() {
  const dex = useContract('dex');
  const [amountIn, setAmountIn] = useState('1');
  const [quote, setQuote] = useState<string>('—');

  async function doQuote() {
    const out = await dex.call('quote', [amountIn]);
    setQuote(String(out));
  }

  return (
    <div className="max-w-md space-y-4">
      <h1 className="text-2xl font-semibold">Swap</h1>
      <input className="w-full border rounded px-3 py-2" value={amountIn} onChange={(e) => setAmountIn(e.target.value)} />
      <button className="bg-brand text-white px-4 py-2 rounded" onClick={doQuote}>Cotizar</button>
      <p>Salida estimada: {quote}</p>
      <button className="border px-4 py-2 rounded" onClick={() => dex.call('swap', [amountIn])}>Intercambiar</button>
    </div>
  );
}
