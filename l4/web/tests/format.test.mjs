import assert from 'assert';
import { createRequire } from 'module';
// Pure JS mirror of format helpers for node --test without TS compile
function shortHash(h, n = 6) {
  if (!h || h.length < n * 2 + 2) return h;
  return `${h.slice(0, n + 2)}…${h.slice(-n)}`;
}
function formatAmount(v, decimals = 2) {
  const n = typeof v === 'string' ? Number(v) : v;
  return n.toLocaleString('en-US', { minimumFractionDigits: decimals, maximumFractionDigits: decimals });
}

import { describe, it } from 'node:test';
describe('format', () => {
  it('shortHash', () => {
    assert.ok(shortHash('0x' + 'ab'.repeat(32)).includes('…'));
  });
  it('formatAmount', () => {
    assert.strictEqual(formatAmount(1.5), '1.50');
  });
});
