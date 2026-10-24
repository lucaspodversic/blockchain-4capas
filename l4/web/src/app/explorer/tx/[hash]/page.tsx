/** Detalle de transacción. */
'use client';
import React from 'react';
import { useParams } from 'next/navigation';

export default function TxPage() {
  const params = useParams<{ hash: string }>();
  return (
    <div className="space-y-3">
      <h1 className="text-2xl font-semibold">Transacción</h1>
      <p className="font-mono break-all text-sm">{params?.hash}</p>
      <p>Status: success · Gas used: 21000</p>
    </div>
  );
}
