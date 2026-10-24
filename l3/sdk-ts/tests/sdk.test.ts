/** Tests del SDK (TypeScript). Ejecutar vía `npm test` (wrapper mjs) o vitest. */
import assert from 'assert';
import { toHex, isAddress, parseUnits } from '../src/utils';
import { selector } from '../src/abi';
import { JsonRpcClient } from '../src/client';

async function main() {
  assert.strictEqual(toHex(255), '0xff');
  assert.ok(isAddress('0x' + 'ab'.repeat(20)));
  assert.strictEqual(parseUnits('1.5', 18), 1500000000000000000n);
  assert.notStrictEqual(selector('transfer'), selector('approve'));
  const c = new JsonRpcClient('http://127.0.0.1:0', undefined as never);
  // force mock path
  const id = await new JsonRpcClient('http://invalid').chainId();
  assert.ok(id.startsWith('0x'));
  console.log('sdk.test.ts ok');
}
main();
