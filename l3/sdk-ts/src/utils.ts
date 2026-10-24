/** Utilidades: formato de montos, hex, direcciones. */
export function toHex(n: number | bigint): string {
  return '0x' + BigInt(n).toString(16);
}

export function fromHex(h: string): bigint {
  const s = h.startsWith('0x') ? h.slice(2) : h;
  return BigInt('0x' + (s || '0'));
}

export function isAddress(a: string): boolean {
  return /^0x[0-9a-fA-F]{40}$/.test(a);
}

export function shortHash(h: string, n = 6): string {
  if (!h) return '';
  return h.slice(0, 2 + n) + '…' + h.slice(-n);
}

export function formatUnits(value: bigint, decimals = 18): string {
  const base = 10n ** BigInt(decimals);
  const whole = value / base;
  const frac = value % base;
  const fracStr = frac.toString().padStart(decimals, '0').replace(/0+$/, '');
  return fracStr ? `${whole}.${fracStr}` : whole.toString();
}

export function parseUnits(s: string, decimals = 18): bigint {
  const [w, f = ''] = s.split('.');
  const frac = (f + '0'.repeat(decimals)).slice(0, decimals);
  return BigInt(w || '0') * 10n ** BigInt(decimals) + BigInt(frac || '0');
}

/** SHA-256 via Web Crypto if available; fallback simple hash for tests. */
export async function sha256Hex(data: Uint8Array): Promise<string> {
  if (typeof crypto !== 'undefined' && crypto.subtle) {
    const dig = await crypto.subtle.digest('SHA-256', data);
    return '0x' + Buffer.from(dig).toString('hex');
  }
  // fallback deterministic (not crypto-grade): FNV-ish
  let h = 2166136261;
  for (const b of data) {
    h ^= b;
    h = Math.imul(h, 16777619);
  }
  return '0x' + (h >>> 0).toString(16).padStart(8, '0').repeat(8).slice(0, 64);
}

export function utf8(s: string): Uint8Array {
  return new TextEncoder().encode(s);
}
