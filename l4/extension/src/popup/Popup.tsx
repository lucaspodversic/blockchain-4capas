/** Interfaz del popup. */
import React, { useEffect, useState } from 'react';
import { Accounts } from './Accounts';
import { ApproveTx } from './ApproveTx';

export function Popup() {
  const [view, setView] = useState<'home' | 'approve'>('home');
  const [pending, setPending] = useState<unknown>(null);

  useEffect(() => {
    // Escucha solicitudes de aprobación (simplificado).
    const listener = (msg: any) => {
      if (msg?.type === 'SHOW_APPROVE') {
        setPending(msg.tx);
        setView('approve');
      }
    };
    chrome.runtime.onMessage?.addListener(listener);
    return () => chrome.runtime.onMessage?.removeListener(listener);
  }, []);

  return (
    <div style={{ width: 320, padding: 16, fontFamily: 'system-ui' }}>
      <h1 style={{ fontSize: 16, margin: 0 }}>L1 Wallet</h1>
      {view === 'home' && <Accounts onUnlock={() => undefined} />}
      {view === 'approve' && (
        <ApproveTx
          tx={pending}
          onDecide={(ok) => {
            chrome.runtime.sendMessage({ type: 'APPROVE_RESULT', ok });
            setView('home');
          }}
        />
      )}
    </div>
  );
}

export default Popup;
