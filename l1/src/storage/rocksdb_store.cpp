// =============================================================================
// rocksdb_store.cpp — Backend RocksDB (producción) con fallback a memory
// Si no se compiló con L1_HAS_ROCKSDB, create_rocksdb_store delega a memory
// y registra un aviso en stderr.
// =============================================================================
#include "kv_store.hpp"
#include <iostream>

#if defined(L1_HAS_ROCKSDB)
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#endif

namespace l1::storage {

#if defined(L1_HAS_ROCKSDB)

class RocksDbStore : public KvStore {
public:
    explicit RocksDbStore(const std::string& path) {
        rocksdb::Options opt;
        opt.create_if_missing = true;
        rocksdb::DB* raw = nullptr;
        auto st = rocksdb::DB::Open(opt, path, &raw);
        if (!st.ok()) throw std::runtime_error(st.ToString());
        db_.reset(raw);
    }

    core::Result<void> put(const core::Bytes& key, const core::Bytes& value) override {
        auto st = db_->Put(rocksdb::WriteOptions(),
            rocksdb::Slice(reinterpret_cast<const char*>(key.data()), key.size()),
            rocksdb::Slice(reinterpret_cast<const char*>(value.data()), value.size()));
        if (!st.ok()) return {core::ErrorCode::StorageError, st.ToString()};
        return core::Result<void>::Ok();
    }

    core::Result<std::optional<core::Bytes>> get(const core::Bytes& key) const override {
        std::string val;
        auto st = db_->Get(rocksdb::ReadOptions(),
            rocksdb::Slice(reinterpret_cast<const char*>(key.data()), key.size()), &val);
        if (st.IsNotFound()) return std::optional<core::Bytes>{};
        if (!st.ok()) return {core::ErrorCode::StorageError, st.ToString()};
        return core::Bytes(val.begin(), val.end());
    }

    core::Result<void> del(const core::Bytes& key) override {
        auto st = db_->Delete(rocksdb::WriteOptions(),
            rocksdb::Slice(reinterpret_cast<const char*>(key.data()), key.size()));
        if (!st.ok()) return {core::ErrorCode::StorageError, st.ToString()};
        return core::Result<void>::Ok();
    }

    bool has(const core::Bytes& key) const override {
        auto r = get(key);
        return r.ok() && r.value().has_value();
    }

    core::Result<void> write_batch(
        const std::vector<std::pair<core::Bytes, core::Bytes>>& puts,
        const std::vector<core::Bytes>& deletes) override {
        rocksdb::WriteBatch batch;
        for (const auto& k : deletes) {
            batch.Delete(rocksdb::Slice(reinterpret_cast<const char*>(k.data()), k.size()));
        }
        for (const auto& [k, v] : puts) {
            batch.Put(
                rocksdb::Slice(reinterpret_cast<const char*>(k.data()), k.size()),
                rocksdb::Slice(reinterpret_cast<const char*>(v.data()), v.size()));
        }
        auto st = db_->Write(rocksdb::WriteOptions(), &batch);
        if (!st.ok()) return {core::ErrorCode::StorageError, st.ToString()};
        return core::Result<void>::Ok();
    }

    void for_each_prefix(const core::Bytes& prefix,
        const std::function<bool(const core::Bytes&, const core::Bytes&)>& fn) const override {
        std::unique_ptr<rocksdb::Iterator> it(db_->NewIterator(rocksdb::ReadOptions()));
        rocksdb::Slice p(reinterpret_cast<const char*>(prefix.data()), prefix.size());
        for (it->Seek(p); it->Valid(); it->Next()) {
            auto k = it->key();
            if (k.size() < prefix.size() ||
                std::memcmp(k.data(), prefix.data(), prefix.size()) != 0) break;
            core::Bytes key(k.data(), k.data() + k.size());
            auto v = it->value();
            core::Bytes val(v.data(), v.data() + v.size());
            if (!fn(key, val)) break;
        }
    }

    core::Result<void> flush() override {
        auto st = db_->Flush(rocksdb::FlushOptions());
        if (!st.ok()) return {core::ErrorCode::StorageError, st.ToString()};
        return core::Result<void>::Ok();
    }

private:
    std::unique_ptr<rocksdb::DB> db_;
};

std::unique_ptr<KvStore> create_rocksdb_store(const std::string& path) {
    return std::make_unique<RocksDbStore>(path);
}

#else

std::unique_ptr<KvStore> create_rocksdb_store(const std::string& path) {
    std::cerr << "[l1] RocksDB no vinculado; usando memory_store para path=" << path
              << " (definir L1_HAS_ROCKSDB + librocksdb para producción)\n";
    (void)path;
    return create_memory_store();
}

#endif

}  // namespace l1::storage
