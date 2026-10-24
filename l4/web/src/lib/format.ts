/** Formato de montos, fechas y hashes. */
export function shortHash(h: string, n = 6): string {
  if (!h || h.length < n * 2 + 2) return h;
  return `${h.slice(0, n + 2)}…${h.slice(-n)}`;
}

export function shortAddress(a: string): string {
  return shortHash(a, 4);
}

export function formatAmount(v: string | number, decimals = 2): string {
  const n = typeof v === 'string' ? Number(v) : v;
  if (Number.isNaN(n)) return String(v);
  return n.toLocaleString(undefined, { minimumFractionDigits: decimals, maximumFractionDigits: decimals });
}

export function formatDate(ts: number): string {
  return new Date(ts * 1000).toISOString();
}
