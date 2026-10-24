/**
 * Despliega todos los contratos en testnet.
 * Encaja en: usa @l1/sdk para enviar bytecode/constructores.
 */
import { createWallet } from '../../sdk-ts/src/wallet';
import { JsonRpcClient } from '../../sdk-ts/src/client';
import { Contract } from '../../sdk-ts/src/contract';
import * as fs from 'fs';
import * as path from 'path';

async function main() {
  const client = new JsonRpcClient(process.env.RPC_URL || 'http://127.0.0.1:8545');
  const w = await createWallet('deployer-seed');
  const contracts = ['token_ft', 'token_nft', 'dex_amm', 'staking_pool', 'governance_dao', 'multisig', 'lending', 'oracle'];
  const addresses: Record<string, string> = {};
  for (const name of contracts) {
    const bytecode = '0x' + Buffer.from(`L1BC:${name}`).toString('hex');
    addresses[name] = await Contract.deploy(client, bytecode, w.address);
    console.log('deployed', name, addresses[name]);
  }
  const out = path.join(__dirname, 'addresses.json');
  fs.writeFileSync(out, JSON.stringify({ network: 'devnet', addresses }, null, 2));
  console.log('wrote', out);
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
