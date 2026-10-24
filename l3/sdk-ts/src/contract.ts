/** Interacción con contratos (deploy/call). */
import type { JsonRpcClient } from './client';
import { encodeCall, type AbiFunction } from './abi';
import type { Address, Hex } from './types';

export class Contract {
  constructor(
    public readonly address: Address,
    public readonly abi: AbiFunction[],
    private client: JsonRpcClient,
  ) {}

  async call(method: string, args: unknown[] = []): Promise<Hex> {
    const fn = this.abi.find((f) => f.name === method);
    if (!fn) throw new Error(`método ABI desconocido: ${method}`);
    const data = encodeCall(fn, args);
    return this.client.call<Hex>('eth_call', [{ to: this.address, data }, 'latest']);
  }

  static async deploy(client: JsonRpcClient, bytecode: Hex, from: Address): Promise<Address> {
    const hash = await client.call<string>('eth_sendTransaction', [{ from, data: bytecode }]);
    // Dirección demo derivada del hash de deploy.
    return '0x' + (hash.replace(/^0x/, '') + '00'.repeat(20)).slice(0, 40);
  }
}
