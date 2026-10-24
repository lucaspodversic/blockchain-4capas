/** Construcción y firma de transacciones. */
import type { Transaction } from './types';
import { signMessage } from './wallet';
import { toHex } from './utils';

export function buildTransfer(opts: {
  from: string;
  to: string;
  nonce: number;
  value: bigint;
  chainId: number;
  maxFeePerGas?: bigint;
}): Transaction {
  return {
    from: opts.from,
    to: opts.to,
    nonce: opts.nonce,
    value: opts.value.toString(),
    data: '0x',
    maxFeePerGas: (opts.maxFeePerGas ?? 1n).toString(),
    chainId: opts.chainId,
  };
}

export async function signTx(tx: Transaction, privateKey: string): Promise<Transaction> {
  const payload = JSON.stringify({
    from: tx.from,
    to: tx.to,
    nonce: tx.nonce,
    value: tx.value,
    data: tx.data,
    chainId: tx.chainId,
  });
  const signature = await signMessage(privateKey, payload);
  return { ...tx, signature };
}

export function txToRpcParams(tx: Transaction): unknown {
  return {
    ...tx,
    value: toHex(BigInt(tx.value)),
    maxFeePerGas: toHex(BigInt(tx.maxFeePerGas)),
  };
}
