/** Estado de la cadena (altura, red). */
'use client';
import { useState } from 'react';

export function useChain() {
  const [network, setNetwork] = useState('Devnet');
  const [height] = useState(128);
  const [tps] = useState(42);
  const [l2Batches] = useState(7);
  return { network, setNetwork, height, tps, l2Batches };
}
