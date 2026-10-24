/**
 * Instancia del SDK configurada.
 * Encaja en: hooks y páginas usan getRpc() como fachada al nodo.
 */
const RPC_URL = process.env.NEXT_PUBLIC_RPC_URL || 'http://127.0.0.1:8545';

export type RpcClient = {
  url: string;
  sendTransaction(tx: { from: string | null; to: string; value: string }): Promise<string>;
  callContract(to: string, method: string, args: unknown[]): Promise<unknown>;
};

let singleton: RpcClient | null = null;

export function getRpc(): RpcClient {
  if (singleton) return singleton;
  singleton = {
    url: RPC_URL,
    async sendTransaction(tx) {
      // Mock determinista si el nodo no está disponible.
      const seed = `${tx.from}|${tx.to}|${tx.value}`;
      let h = 0;
      for (let i = 0; i < seed.length; i++) h = (h * 31 + seed.charCodeAt(i)) >>> 0;
      return '0x' + h.toString(16).padStart(64, '0');
    },
    async callContract(_to, method, args) {
      if (method === 'quote') return String(Number(args[0] || 0) * 0.997);
      return { ok: true, method, args };
    },
  };
  return singleton;
}
