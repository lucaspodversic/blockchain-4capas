/** Tabla de transacciones. */
import React from 'react';
import { shortHash, shortAddress } from '../lib/format';

export type TxRow = { hash: string; from: string; to: string; value: string; status: string };

export function TxTable({ rows }: { rows: TxRow[] }) {
  return (
    <table className="w-full text-sm">
      <thead>
        <tr className="border-b text-left text-slate-500">
          <th className="py-2">Hash</th>
          <th>From</th>
          <th>To</th>
          <th>Value</th>
          <th>Status</th>
        </tr>
      </thead>
      <tbody>
        {rows.map((r) => (
          <tr key={r.hash} className="border-b">
            <td className="py-2 font-mono">
              <a href={`/explorer/tx/${r.hash}`}>{shortHash(r.hash)}</a>
            </td>
            <td className="font-mono">{shortAddress(r.from)}</td>
            <td className="font-mono">{shortAddress(r.to)}</td>
            <td>{r.value}</td>
            <td>{r.status}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}
