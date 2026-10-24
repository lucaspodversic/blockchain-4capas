/** Lógica de la dapp de swap sobre el SDK. */
import { JsonRpcClient } from '../../../sdk-ts/src/client';
import { encodeCall } from '../../../sdk-ts/src/abi';

export async function quoteAndSwap(client: JsonRpcClient, dex: string, amountIn: bigint) {
  const data = encodeCall({ name: 'swap_x_for_y', inputs: [{ name: 'dx', type: 'uint256' }] }, [amountIn]);
  const out = await client.call<string>('eth_call', [{ to: dex, data }, 'latest']);
  return { amountOut: out, data };
}
