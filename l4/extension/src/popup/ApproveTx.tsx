/** Pantalla de aprobación de transacciones. */
import React from 'react';

export function ApproveTx({
  tx,
  onDecide,
}: {
  tx: unknown;
  onDecide: (ok: boolean) => void;
}) {
  return (
    <div>
      <h2 style={{ fontSize: 14 }}>Aprobar transacción</h2>
      <pre style={{ fontSize: 10, whiteSpace: 'pre-wrap' }}>{JSON.stringify(tx, null, 2)}</pre>
      <div style={{ display: 'flex', gap: 8 }}>
        <button onClick={() => onDecide(true)}>Aprobar</button>
        <button onClick={() => onDecide(false)}>Rechazar</button>
      </div>
    </div>
  );
}
