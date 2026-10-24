// =============================================================================
// transaction.cpp — Hash, encode/decode y validación estructural de txs
// =============================================================================
#include "transaction.hpp"
#include "../crypto/hash.hpp"

namespace l1::core {

Bytes Transaction::encode_unsigned() const {
    Bytes out;
    Encoder e(out);
    e.u8(static_cast<uint8_t>(type));
    e.fixed(from);
    e.fixed(to);
    e.u64(nonce);
    e.u64(value);
    e.u64(gas_limit);
    e.u64(max_fee_per_gas);
    e.u64(max_priority_fee);
    e.bytes_len(data);
    e.u64(chain_id);
    return out;
}

Bytes Transaction::encode() const {
    Bytes out = encode_unsigned();
    Encoder e(out);
    e.u8(has_signature ? 1 : 0);
    if (has_signature) e.fixed(signature);
    return out;
}

Result<Transaction> Transaction::decode(const Bytes& raw) {
    Decoder d(raw);
    Transaction tx;
    auto t = d.u8(); if (!t) return t.error();
    tx.type = static_cast<TxType>(t.value());
    auto from = d.fixed<20>(); if (!from) return from.error();
    auto to = d.fixed<20>(); if (!to) return to.error();
    auto nonce = d.u64(); if (!nonce) return nonce.error();
    auto value = d.u64(); if (!value) return value.error();
    auto gas = d.u64(); if (!gas) return gas.error();
    auto fee = d.u64(); if (!fee) return fee.error();
    auto tip = d.u64(); if (!tip) return tip.error();
    auto data = d.bytes_len(); if (!data) return data.error();
    auto cid = d.u64(); if (!cid) return cid.error();
    tx.from = from.value();
    tx.to = to.value();
    tx.nonce = nonce.value();
    tx.value = value.value();
    tx.gas_limit = gas.value();
    tx.max_fee_per_gas = fee.value();
    tx.max_priority_fee = tip.value();
    tx.data = data.value();
    tx.chain_id = cid.value();
    auto hs = d.u8();
    if (hs && hs.value() == 1) {
        auto sig = d.fixed<64>();
        if (!sig) return sig.error();
        tx.signature = sig.value();
        tx.has_signature = true;
    }
    return tx;
}

Hash256 Transaction::signing_hash() const {
    return crypto::keccak256(encode_unsigned());
}

Hash256 Transaction::hash() const {
    return crypto::keccak256(encode());
}

Result<void> Transaction::validate_structure() const {
    if (gas_limit == 0) return {ErrorCode::InvalidTransaction, "gas_limit=0"};
    if (type == TxType::Transfer && !data.empty()) {
        // Permitido: data opcional en transferencias.
    }
    if (type == TxType::Deploy && data.empty()) {
        return {ErrorCode::InvalidTransaction, "deploy sin bytecode"};
    }
    if (type == TxType::Call && to == zero_address()) {
        return {ErrorCode::InvalidTransaction, "call sin destino"};
    }
    if (!has_signature) {
        return {ErrorCode::InvalidSignature, "tx sin firma"};
    }
    return Result<void>::Ok();
}

}  // namespace l1::core
