/**
 * Service worker: claves, firma y aprobaciones.
 * Encaja en: recibe mensajes de content/popup; nunca expone la seed.
 */
import { Vault } from './crypto/vault';

const vault = new Vault();

chrome.runtime.onMessage.addListener((msg, _sender, sendResponse) => {
  (async () => {
    if (msg.type === 'GET_ACCOUNTS') {
      sendResponse({ accounts: await vault.listAccounts() });
      return;
    }
    if (msg.type === 'REQUEST_SIGN') {
      // En prod: abrir ApproveTx popup y esperar decisión del usuario.
      const approved = msg.autoApprove === true;
      if (!approved) {
        sendResponse({ error: 'user_rejected' });
        return;
      }
      const sig = await vault.sign(msg.payload);
      sendResponse({ signature: sig });
      return;
    }
    if (msg.type === 'UNLOCK') {
      await vault.unlock(msg.password);
      sendResponse({ ok: true });
      return;
    }
    sendResponse({ error: 'unknown' });
  })();
  return true;
});
