// =============================================================================
// serialize.cpp — Implementación auxiliar de serialización
// =============================================================================
#include "serialize.hpp"

namespace l1::core {

Bytes encode_u64(uint64_t v) {
    Bytes out;
    Encoder e(out);
    e.u64(v);
    return out;
}

Result<uint64_t> decode_u64(const Bytes& b) {
    Decoder d(b);
    return d.u64();
}

}  // namespace l1::core
