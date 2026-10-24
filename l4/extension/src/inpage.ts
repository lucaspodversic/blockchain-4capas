/**
 * Proveedor inyectado (`window.l1`) para dapps.
 * Encaja en: API compatible con eth_requestAccounts / eth_sendTransaction simplificada.
 */
type Req = { method: string; params?: unknown[] };

class L1Provider {
  isL1 = true;
  private _id = 1;

  async request(req: Req): Promise<unknown> {
    if (req.method === 'eth_requestAccounts' || req.method === 'eth_accounts') {
      return this._send({ type: 'GET_ACCOUNTS' }).then((r: any) => r.accounts || []);
    }
    if (req.method === 'eth_sendTransaction') {
      return this._send({ type: 'REQUEST_SIGN', payload: req.params?.[0], autoApprove: false });
    }
    if (req.method === 'eth_chainId') return '0x539'; // 1337
    throw new Error(`unsupported: ${req.method}`);
  }

  private _send(payload: unknown): Promise<any> {
    const id = this._id++;
    return new Promise((resolve) => {
      const handler = (ev: MessageEvent) => {
        if (ev.data?.source === 'l1-content' && ev.data.id === id) {
          window.removeEventListener('message', handler);
          resolve(ev.data.response);
        }
      };
      window.addEventListener('message', handler);
      window.postMessage({ source: 'l1-inpage', id, payload }, '*');
    });
  }
}

declare global {
  interface Window {
    l1?: L1Provider;
  }
}

window.l1 = new L1Provider();
window.dispatchEvent(new Event('l1#initialized'));
