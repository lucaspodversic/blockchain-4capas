/** Lógica de la dapp de staking. */
import { encodeCall } from '../../../sdk-ts/src/abi';

export function buildStakeCall(amount: bigint): string {
  return encodeCall({ name: 'stake', inputs: [{ name: 'amount', type: 'uint256' }] }, [amount]);
}

export function buildClaimCall(): string {
  return encodeCall({ name: 'claim', inputs: [] }, []);
}
