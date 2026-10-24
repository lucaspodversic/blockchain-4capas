import assert from 'assert';
import { describe, it } from 'node:test';

// Mirror mínimo de Vault para tests sin bundler
function xor(data, key) {
  const out = new Uint8Array(data.length);
  for (let i = 0; i < data.length; i++) out[i] = data[i] ^ key[i % key.length];
  return out;
}

describe('vault xor', () => {
  it('roundtrip', () => {
    const data = new TextEncoder().encode('secret-seed');
    const key = new TextEncoder().encode('pass');
    const enc = xor(data, key);
    const dec = xor(enc, key);
    assert.deepStrictEqual(Array.from(dec), Array.from(data));
  });
});
