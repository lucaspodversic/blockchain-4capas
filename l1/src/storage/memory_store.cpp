// =============================================================================
// memory_store.cpp — Backend KV en memoria (tests y modo efímero)
// =============================================================================
#include "kv_store.hpp"
#include <map>
#include <mutex>

namespace l1::storage {

class MemoryStore : public KvStore {
public:
    core::Result<void> put(const core::Bytes& key, const core::Bytes& value) override {
        std::lock_guard lock(mu_);
        data_[key] = value;
        return core::Result<void>::Ok();
    }

    core::Result<std::optional<core::Bytes>> get(const core::Bytes& key) const override {
        std::lock_guard lock(mu_);
        auto it = data_.find(key);
        if (it == data_.end()) return std::optional<core::Bytes>{};
        return std::optional<core::Bytes>{it->second};
    }

    core::Result<void> del(const core::Bytes& key) override {
        std::lock_guard lock(mu_);
        data_.erase(key);
        return core::Result<void>::Ok();
    }

    bool has(const core::Bytes& key) const override {
        std::lock_guard lock(mu_);
        return data_.count(key) > 0;
    }

    core::Result<void> write_batch(
        const std::vector<std::pair<core::Bytes, core::Bytes>>& puts,
        const std::vector<core::Bytes>& deletes) override {
        std::lock_guard lock(mu_);
        for (const auto& k : deletes) data_.erase(k);
        for (const auto& [k, v] : puts) data_[k] = v;
        return core::Result<void>::Ok();
    }

    void for_each_prefix(const core::Bytes& prefix,
        const std::function<bool(const core::Bytes&, const core::Bytes&)>& fn) const override {
        std::lock_guard lock(mu_);
        for (const auto& [k, v] : data_) {
            if (k.size() < prefix.size()) continue;
            if (!std::equal(prefix.begin(), prefix.end(), k.begin())) continue;
            if (!fn(k, v)) break;
        }
    }

private:
    mutable std::mutex mu_;
    std::map<core::Bytes, core::Bytes> data_;
};

std::unique_ptr<KvStore> create_memory_store() {
    return std::make_unique<MemoryStore>();
}

}  // namespace l1::storage
