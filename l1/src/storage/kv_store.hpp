// =============================================================================
// kv_store.hpp — Interfaz abstracta clave-valor (backend intercambiable)
// Encaja en: state DB, block store, tx index.
// =============================================================================
#pragma once
#include "../core/types.hpp"
#include "../core/error.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <functional>

namespace l1::storage {

class KvStore {
public:
    virtual ~KvStore() = default;

    virtual core::Result<void> put(const core::Bytes& key, const core::Bytes& value) = 0;
    virtual core::Result<std::optional<core::Bytes>> get(const core::Bytes& key) const = 0;
    virtual core::Result<void> del(const core::Bytes& key) = 0;
    virtual bool has(const core::Bytes& key) const = 0;

    /// Escritura atómica de varios pares.
    virtual core::Result<void> write_batch(
        const std::vector<std::pair<core::Bytes, core::Bytes>>& puts,
        const std::vector<core::Bytes>& deletes) = 0;

    /// Itera claves con prefijo (callback; return false para parar).
    virtual void for_each_prefix(const core::Bytes& prefix,
        const std::function<bool(const core::Bytes&, const core::Bytes&)>& fn) const = 0;

    virtual core::Result<void> flush() { return core::Result<void>::Ok(); }
};

/// Factory: memory siempre; rocksdb si L1_HAS_ROCKSDB.
std::unique_ptr<KvStore> create_memory_store();
std::unique_ptr<KvStore> create_rocksdb_store(const std::string& path);

}  // namespace l1::storage
