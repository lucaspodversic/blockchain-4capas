/** Tipos compartidos del SDK. Encaja en: client, wallet, tx_builder. */
export type Hex = `0x${string}` | string;
export type Address = string;
export type Hash = string;

export interface Transaction {
  from: Address;
  to: Address;
  nonce: number;
  value: string; // decimal or hex
  data: Hex;
  maxFeePerGas: string;
  chainId: number;
  signature?: Hex;
}

export interface BlockHeader {
  height: number;
  hash: Hash;
  parentHash: Hash;
  stateRoot: Hash;
  timestamp: number;
}

export interface RpcRequest {
  jsonrpc: '2.0';
  id: number | string;
  method: string;
  params: unknown[];
}

export interface RpcResponse<T = unknown> {
  jsonrpc: '2.0';
  id: number | string;
  result?: T;
  error?: { code: number; message: string };
}
