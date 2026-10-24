// =============================================================================
// account.hpp — Cuenta de estado (EOA o contrato)
// Encaja en: StateDB / trie de cuentas. El bytecode se serializa para caché local.
// =============================================================================
#pragma once

#include "types.hpp"
#include "serialize.hpp"
#include "error.hpp"

namespace l1::core {

struct Account {
    Amount balance{0};
    Nonce nonce{0};
    Hash256 code_hash{zero_hash()};   ///< Hash del bytecode (cero = EOA)
    Hash256 storage_root{zero_hash()}; ///< Raíz del trie de storage del contrato
    Bytes code;                        ///< Bytecode en caché (también persistido en StateDB)

    bool is_contract() const {
        return code_hash != zero_hash() || !code.empty();
    }

    /// Serializa cuenta incluyendo bytecode (necesario para VM en memoria).
    Bytes encode() const {
        Bytes out;
        Encoder e(out);
        e.u64(balance);
        e.u64(nonce);
        e.fixed(code_hash);
        e.fixed(storage_root);
        e.bytes_len(code);
        return out;
    }

    static Result<Account> decode(const Bytes& raw) {
        Decoder d(raw);
        Account a;
        auto bal = d.u64(); if (!bal) return bal.error();
        auto n = d.u64(); if (!n) return n.error();
        auto ch = d.fixed<32>(); if (!ch) return ch.error();
        auto sr = d.fixed<32>(); if (!sr) return sr.error();
        a.balance = bal.value();
        a.nonce = n.value();
        a.code_hash = ch.value();
        a.storage_root = sr.value();
        // Compatibilidad: si no hay más bytes, code vacío.
        if (d.remaining() > 0) {
            auto c = d.bytes_len();
            if (!c) return c.error();
            a.code = c.value();
        }
        return a;
    }
};

}  // namespace l1::core
