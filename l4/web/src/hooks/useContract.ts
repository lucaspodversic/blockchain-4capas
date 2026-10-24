/** Llamadas a contratos. */
'use client';
import { useMemo } from 'react';
import { getRpc } from '../lib/rpc';

const ADDR: Record<string, string> = {
  dex: '0x00000000000000000000000000000000000000d1',
  staking: '0x0000000000000000000000000000000000000051',
  governance: '0x0000000000000000000000000000000000000061',
};

export function useContract(name: string) {
  return useMemo(() => {
    const to = ADDR[name] || '0x0';
    return {
      address: to,
      async call(method: string, args: unknown[]) {
        const rpc = getRpc();
        return rpc.callContract(to, method, args);
      },
    };
  }, [name]);
}
