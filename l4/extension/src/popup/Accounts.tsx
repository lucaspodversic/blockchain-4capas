/** Gestión de cuentas. */
import React, { useEffect, useState } from 'react';

export function Accounts({ onUnlock }: { onUnlock: () => void }) {
  const [accounts, setAccounts] = useState<string[]>([]);
  const [password, setPassword] = useState('');

  useEffect(() => {
    chrome.runtime?.sendMessage?.({ type: 'GET_ACCOUNTS' }, (r) => {
      setAccounts(r?.accounts || []);
    });
  }, []);

  return (
    <div>
      <p style={{ fontSize: 12 }}>Cuentas</p>
      <ul>
        {accounts.map((a) => (
          <li key={a} style={{ fontFamily: 'monospace', fontSize: 11 }}>{a}</li>
        ))}
      </ul>
      {accounts.length === 0 && (
        <div>
          <input
            type="password"
            placeholder="Contraseña bóveda"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
          />
          <button
            onClick={() => {
              chrome.runtime?.sendMessage?.({ type: 'UNLOCK', password }, () => onUnlock());
            }}
          >
            Desbloquear / Crear
          </button>
        </div>
      )}
    </div>
  );
}
