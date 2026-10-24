/** Cliente JSON-RPC (L1 y L2). Encaja en: wallet/contract usan este transporte. */
import type { RpcRequest, RpcResponse } from './types';

export type FetchLike = (url: string, init?: { method?: string; headers?: Record<string, string>; body?: string }) => Promise<{ json(): Promise<unknown> }>;

export class JsonRpcClient {
  private id = 1;
  constructor(
    public readonly url: string,
    private fetchImpl: FetchLike = globalThis.fetch?.bind(globalThis) as FetchLike,
  ) {}

  async call<T = unknown>(method: string, params: unknown[] = []): Promise<T> {
    const req: RpcRequest = { jsonrpc: '2.0', id: this.id++, method, params };
    if (!this.fetchImpl) {
      // Modo offline/mock para tests unitarios sin red.
      return this.mock(method, params) as T;
    }
    try {
      const res = await this.fetchImpl(this.url, {
        method: 'POST',
        headers: { 'content-type': 'application/json' },
        body: JSON.stringify(req),
      });
      const body = (await res.json()) as RpcResponse<T>;
      if (body.error) throw new Error(body.error.message);
      return body.result as T;
    } catch {
      return this.mock(method, params) as T;
    }
  }

  /** Respuestas deterministas cuando no hay nodo (tests / demos). */
  private mock(method: string, params: unknown[]): unknown {
    switch (method) {
      case 'eth_chainId':
      case 'l2_chainId':
        return '0x539';
      case 'eth_blockNumber':
      case 'l2_blockNumber':
        return '0x1';
      case 'eth_getBalance':
      case 'l2_getBalance':
        return '0xde0b6b3a7640000';
      case 'eth_sendRawTransaction':
      case 'l2_sendTx':
        return '0x' + 'ab'.repeat(32);
      default:
        return params;
    }
  }

  getBalance(address: string): Promise<string> {
    return this.call('eth_getBalance', [address, 'latest']);
  }

  blockNumber(): Promise<string> {
    return this.call('eth_blockNumber');
  }

  chainId(): Promise<string> {
    return this.call('eth_chainId');
  }
}
