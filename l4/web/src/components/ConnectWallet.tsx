/** Botón de conexión de wallet. */
'use client';
import React from 'react';
import { useWallet } from '../hooks/useWallet';
import { shortAddress } from '../lib/format';

export function ConnectWallet() {
  const w = useWallet();
  if (w.address) {
    return (
      <button className="rounded-full border px-3 py-1 text-sm" onClick={w.disconnect}>
        {shortAddress(w.address)}
      </button>
    );
  }
  return (
    <button className="rounded-full bg-brand text-white px-3 py-1 text-sm" onClick={w.connect}>
      Conectar
    </button>
  );
}
