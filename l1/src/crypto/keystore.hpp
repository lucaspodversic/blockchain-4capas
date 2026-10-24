// =============================================================================
// keystore.hpp — API del almacén cifrado de claves
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"
#include <string>

namespace l1::crypto {

struct Keystore {
    core::Address address{};
    core::PrivateKey private_key{};
};

core::Result<std::string> keystore_encrypt(const core::PrivateKey& sk,
                                           const std::string& password);
core::Result<Keystore> keystore_decrypt(const std::string& json_str,
                                        const std::string& password);
core::Result<void> keystore_save_file(const std::string& path, const core::PrivateKey& sk,
                                      const std::string& password);

}  // namespace l1::crypto
