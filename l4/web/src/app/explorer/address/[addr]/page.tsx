/** Detalle de cuenta. */
'use client';
import React from 'react';
import { useParams } from 'next/navigation';
import { TxTable } from '../../../../components/TxTable';

export default function AddressPage() {
  const params = useParams<{ addr: string }>();
  return (
    <div className="space-y-4">
      <h1 className="text-2xl font-semibold">Cuenta</h1>
      <p className="font-mono break-all text-sm">{params?.addr}</p>
      <p>Balance: 1000 L1 · Nonce: 5</p>
      <TxTable rows={[{ hash: '0xabc', from: params?.addr || '', to: '0xbbb', value: '10', status: 'ok' }]} />
    </div>
  );
}
