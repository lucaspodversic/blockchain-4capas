/** Estado de la wallet. */
'use client';
import { useCallback, useState } from 'react';
import type { TxRow } from '../components/TxTable';

const MOCK_ADDR = '0x' + 'ab'.repeat(20);

export function useWallet() {
  const [address, setAddress] = useState<string | null>(null);
  const [balance] = useState('1000.00');
  const [history] = useState<TxRow[]>([
    { hash: '0x' + '11'.repeat(32), from: MOCK_ADDR, to: '0x' + '22'.repeat(20), value: '5', status: 'ok' },
  ]);
  const connect = useCallback(() => setAddress(MOCK_ADDR), []);
  const disconnect = useCallback(() => setAddress(null), []);
  return { address, balance, history, connect, disconnect };
}
