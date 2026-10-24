/** Detalle de bloque. */
'use client';
import React from 'react';
import { useParams } from 'next/navigation';

export default function BlockPage() {
  const params = useParams<{ id: string }>();
  const id = params?.id ?? '0';
  return (
    <div className="space-y-3">
      <h1 className="text-2xl font-semibold">Bloque #{id}</h1>
      <dl className="grid grid-cols-2 gap-2 text-sm">
        <dt>Hash</dt>
        <dd className="font-mono break-all">0x{Number(id).toString(16).padStart(64, '0')}</dd>
        <dt>Parent</dt>
        <dd className="font-mono break-all">0x{(Number(id) - 1).toString(16).padStart(64, '0')}</dd>
        <dt>Tx count</dt>
        <dd>3</dd>
      </dl>
    </div>
  );
}
