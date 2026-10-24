/** Lógica de la dapp de gobernanza. */
import { encodeCall } from '../../../sdk-ts/src/abi';

export function buildPropose(description: string): string {
  return encodeCall({ name: 'propose', inputs: [{ name: 'description', type: 'string' }] }, [description]);
}

export function buildVote(id: number, support: boolean): string {
  return encodeCall(
    { name: 'vote', inputs: [{ name: 'id', type: 'uint64' }, { name: 'support', type: 'bool' }] },
    [id, support ? 1 : 0],
  );
}
