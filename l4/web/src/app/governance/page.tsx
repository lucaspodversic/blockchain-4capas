/** Votación y propuestas. */
'use client';
import React, { useState } from 'react';
import { useContract } from '../../hooks/useContract';

export default function GovernancePage() {
  const gov = useContract('governance');
  const [desc, setDesc] = useState('');
  return (
    <div className="space-y-4 max-w-lg">
      <h1 className="text-2xl font-semibold">Gobernanza</h1>
      <textarea className="w-full border rounded p-3" rows={3} value={desc} onChange={(e) => setDesc(e.target.value)} placeholder="Descripción de la propuesta" />
      <button className="bg-brand text-white px-4 py-2 rounded" onClick={() => gov.call('propose', [desc])}>Proponer</button>
      <button className="border px-4 py-2 rounded ml-2" onClick={() => gov.call('vote', [0, true])}>Votar a favor #0</button>
    </div>
  );
}
