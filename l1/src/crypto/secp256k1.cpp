// =============================================================================
// secp256k1.cpp — ECDSA secp256k1 con OpenSSL
// =============================================================================
#include "secp256k1.hpp"
#include "hash.hpp"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <memory>
#include <cstring>
#include <stdexcept>

namespace l1::crypto {

namespace {
struct EcKeyDeleter { void operator()(EC_KEY* k) const { EC_KEY_free(k); } };
using EcKeyPtr = std::unique_ptr<EC_KEY, EcKeyDeleter>;
}  // namespace

SecpKeyPair secp256k1_generate() {
    EcKeyPtr key(EC_KEY_new_by_curve_name(NID_secp256k1));
    if (!key || EC_KEY_generate_key(key.get()) != 1) {
        throw std::runtime_error("secp256k1 generate failed");
    }
    const BIGNUM* priv = EC_KEY_get0_private_key(key.get());
    SecpKeyPair kp;
    kp.private_key.resize(32);
    BN_bn2binpad(priv, kp.private_key.data(), 32);

    const EC_POINT* pub = EC_KEY_get0_public_key(key.get());
    const EC_GROUP* group = EC_KEY_get0_group(key.get());
    size_t len = EC_POINT_point2oct(group, pub, POINT_CONVERSION_UNCOMPRESSED, nullptr, 0, nullptr);
    kp.public_key.resize(len);
    EC_POINT_point2oct(group, pub, POINT_CONVERSION_UNCOMPRESSED, kp.public_key.data(), len, nullptr);
    return kp;
}

core::Result<core::Bytes> secp256k1_sign(const core::Bytes& priv32, const core::Hash256& digest) {
    if (priv32.size() != 32) return {core::ErrorCode::InvalidArgument, "priv len"};
    EcKeyPtr key(EC_KEY_new_by_curve_name(NID_secp256k1));
    BIGNUM* bn = BN_bin2bn(priv32.data(), 32, nullptr);
    if (!key || EC_KEY_set_private_key(key.get(), bn) != 1) {
        BN_free(bn);
        return {core::ErrorCode::InvalidArgument, "bad priv"};
    }
    // Derivar punto público
    const EC_GROUP* group = EC_KEY_get0_group(key.get());
    EC_POINT* pub = EC_POINT_new(group);
    EC_POINT_mul(group, pub, bn, nullptr, nullptr, nullptr);
    EC_KEY_set_public_key(key.get(), pub);
    EC_POINT_free(pub);
    BN_free(bn);

    ECDSA_SIG* sig = ECDSA_do_sign(digest.data(), 32, key.get());
    if (!sig) return {core::ErrorCode::InternalError, "sign failed"};
    int len = i2d_ECDSA_SIG(sig, nullptr);
    core::Bytes der(static_cast<size_t>(len));
    unsigned char* p = der.data();
    i2d_ECDSA_SIG(sig, &p);
    ECDSA_SIG_free(sig);
    return der;
}

bool secp256k1_verify(const core::Bytes& pub, const core::Hash256& digest, const core::Bytes& sig_der) {
    EcKeyPtr key(EC_KEY_new_by_curve_name(NID_secp256k1));
    if (!key) return false;
    const EC_GROUP* group = EC_KEY_get0_group(key.get());
    EC_POINT* point = EC_POINT_new(group);
    if (EC_POINT_oct2point(group, point, pub.data(), pub.size(), nullptr) != 1) {
        EC_POINT_free(point);
        return false;
    }
    EC_KEY_set_public_key(key.get(), point);
    EC_POINT_free(point);
    const unsigned char* p = sig_der.data();
    ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &p, static_cast<long>(sig_der.size()));
    if (!sig) return false;
    int ok = ECDSA_do_verify(digest.data(), 32, sig, key.get());
    ECDSA_SIG_free(sig);
    return ok == 1;
}

core::Address secp256k1_address(const core::Bytes& uncompressed_pub65) {
    core::Address addr = core::zero_address();
    if (uncompressed_pub65.size() != 65 || uncompressed_pub65[0] != 0x04) return addr;
    auto h = keccak256(uncompressed_pub65.data() + 1, 64);
    std::memcpy(addr.data(), h.data() + 12, 20);
    return addr;
}

}  // namespace l1::crypto
