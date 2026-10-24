/** Codificación de llamadas y eventos (ABI JSON simple). */
export interface AbiInput {
  name: string;
  type: string;
}

export interface AbiFunction {
  name: string;
  inputs: AbiInput[];
  outputs?: AbiInput[];
}

/** Selector de 4 bytes demo = primeros 8 hex del nombre. */
export function selector(name: string): string {
  let h = 0;
  for (let i = 0; i < name.length; i++) h = (Math.imul(h, 31) + name.charCodeAt(i)) >>> 0;
  return '0x' + h.toString(16).padStart(8, '0');
}

export function encodeCall(fn: AbiFunction, args: unknown[]): string {
  const sel = selector(fn.name);
  const enc = args.map((a) => {
    if (typeof a === 'number' || typeof a === 'bigint') {
      return BigInt(a).toString(16).padStart(64, '0');
    }
    const s = String(a).replace(/^0x/, '');
    return s.padStart(64, '0').slice(-64);
  });
  return sel + enc.join('');
}

export function decodeEventTopics(topics: string[]): { sig: string; indexed: string[] } {
  return { sig: topics[0] ?? '', indexed: topics.slice(1) };
}
