/** Lógica de marketplace NFT. */
import { encodeCall } from '../../../sdk-ts/src/abi';

export function buildList(tokenId: number, price: bigint): string {
  return encodeCall(
    { name: 'list', inputs: [{ name: 'id', type: 'uint64' }, { name: 'price', type: 'uint256' }] },
    [tokenId, price],
  );
}

export function buildBuy(tokenId: number): string {
  return encodeCall({ name: 'buy', inputs: [{ name: 'id', type: 'uint64' }] }, [tokenId]);
}
