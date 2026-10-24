/** Modal de confirmación y estado de tx. */
'use client';
import React from 'react';

export function TxModal({ open, hash, onClose }: { open: boolean; hash: string | null; onClose: () => void }) {
  if (!open) return null;
  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/40">
      <div className="w-full max-w-sm rounded-xl bg-white p-6 shadow-xl">
        <h2 className="text-lg font-semibold">Transacción</h2>
        <p className="mt-2 text-sm text-slate-600">{hash ? 'Enviada' : 'Firmando…'}</p>
        {hash && <p className="mt-2 font-mono text-xs break-all">{hash}</p>}
        <button className="mt-4 w-full rounded bg-brand py-2 text-white" onClick={onClose}>Cerrar</button>
      </div>
    </div>
  );
}
