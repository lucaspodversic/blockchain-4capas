/** Inicio del explorer (últimos bloques/txs). */
'use client';
import React from 'react';
import { SearchBar } from '../../components/SearchBar';
import { useChain } from '../../hooks/useChain';
import { shortHash } from '../../lib/format';

export default function ExplorerPage() {
  const chain = useChain();
  const blocks = Array.from({ length: 5 }, (_, i) => ({
    height: chain.height - i,
    hash: `0x${(chain.height - i).toString(16).padStart(64, '0')}`,
    txs: 3 + (i % 4),
  }));
  return (
    <div className="space-y-6">
      <h1 className="text-2xl font-semibold">Explorer</h1>
      <SearchBar />
      <table className="w-full text-sm">
        <thead>
          <tr className="text-left border-b">
            <th className="py-2">Altura</th>
            <th>Hash</th>
            <th>Txs</th>
          </tr>
        </thead>
        <tbody>
          {blocks.map((b) => (
            <tr key={b.height} className="border-b">
              <td className="py-2">
                <a className="text-brand" href={`/explorer/block/${b.height}`}>{b.height}</a>
              </td>
              <td className="font-mono">{shortHash(b.hash)}</td>
              <td>{b.txs}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
