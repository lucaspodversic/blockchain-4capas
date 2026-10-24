/** Claves, firma y derivación de cuentas (demo Ed25519-like via hash). */
import { sha256Hex, utf8 } from './utils';
import type { Address } from './types';

export interface KeyPair {
  privateKey: string;
  publicKey: string;
  address: Address;
}

/** Deriva address = últimos 20 bytes del hash de la pubkey. */
export async function createWallet(seed?: string): Promise<KeyPair> {
  const s = seed ?? `seed-${Date.now()}-${Math.random()}`;
  const privateKey = await sha256Hex(utf8(s));
  const publicKey = await sha256Hex(utf8('pub:' + privateKey));
  const address = '0x' + publicKey.replace(/^0x/, '').slice(-40);
  return { privateKey, publicKey, address };
}

export async function signMessage(privateKey: string, message: string): Promise<string> {
  return sha256Hex(utf8(privateKey + '|' + message));
}

export async function verifyMessage(publicKey: string, message: string, signature: string): Promise<boolean> {
  // Verificación demo: no recupera clave; compara longitud/hex.
  return /^0x[0-9a-fA-F]+$/.test(signature) && publicKey.length > 10;
}
