// =============================================================================
// serialize.hpp — Serialización binaria canónica (little-endian, determinista)
// Encaja en: hashing de txs/bloques, codec P2P y persistencia.
// =============================================================================
#pragma once

#include "types.hpp"
#include "error.hpp"
#include <cstring>
#include <stdexcept>

namespace l1::core {

/// Escritor binario append-only. Orden: little-endian para enteros.
class Encoder {
public:
    explicit Encoder(Bytes& out) : out_(out) {}

    void u8(uint8_t v) { out_.push_back(v); }

    void u16(uint16_t v) {
        out_.push_back(static_cast<uint8_t>(v));
        out_.push_back(static_cast<uint8_t>(v >> 8));
    }

    void u32(uint32_t v) {
        for (int i = 0; i < 4; ++i) out_.push_back(static_cast<uint8_t>(v >> (8 * i)));
    }

    void u64(uint64_t v) {
        for (int i = 0; i < 8; ++i) out_.push_back(static_cast<uint8_t>(v >> (8 * i)));
    }

    void bytes(const uint8_t* data, size_t len) {
        out_.insert(out_.end(), data, data + len);
    }

    void bytes(const Bytes& b) { bytes(b.data(), b.size()); }

    template <size_t N>
    void fixed(const std::array<uint8_t, N>& a) { bytes(a.data(), N); }

    /// Prefijo de longitud u32 + payload (para strings/bytes variables).
    void bytes_len(const Bytes& b) {
        if (b.size() > 0xffffffffu) throw std::runtime_error("bytes too large");
        u32(static_cast<uint32_t>(b.size()));
        bytes(b);
    }

    void str(const std::string& s) {
        Bytes tmp(s.begin(), s.end());
        bytes_len(tmp);
    }

    Bytes& buffer() { return out_; }

private:
    Bytes& out_;
};

/// Lector binario con cursor. Falla con Result si faltan bytes.
class Decoder {
public:
    Decoder(const uint8_t* data, size_t len) : data_(data), len_(len), pos_(0) {}
    explicit Decoder(const Bytes& b) : Decoder(b.data(), b.size()) {}

    size_t remaining() const { return len_ - pos_; }
    size_t position() const { return pos_; }

    Result<uint8_t> u8() {
        if (pos_ + 1 > len_) return {ErrorCode::DeserializationError, "u8 EOF"};
        return data_[pos_++];
    }

    Result<uint16_t> u16() {
        if (pos_ + 2 > len_) return {ErrorCode::DeserializationError, "u16 EOF"};
        uint16_t v = static_cast<uint16_t>(data_[pos_]) |
                     (static_cast<uint16_t>(data_[pos_ + 1]) << 8);
        pos_ += 2;
        return v;
    }

    Result<uint32_t> u32() {
        if (pos_ + 4 > len_) return {ErrorCode::DeserializationError, "u32 EOF"};
        uint32_t v = 0;
        for (int i = 0; i < 4; ++i) v |= static_cast<uint32_t>(data_[pos_ + i]) << (8 * i);
        pos_ += 4;
        return v;
    }

    Result<uint64_t> u64() {
        if (pos_ + 8 > len_) return {ErrorCode::DeserializationError, "u64 EOF"};
        uint64_t v = 0;
        for (int i = 0; i < 8; ++i) v |= static_cast<uint64_t>(data_[pos_ + i]) << (8 * i);
        pos_ += 8;
        return v;
    }

    Result<Bytes> bytes(size_t n) {
        if (pos_ + n > len_) return {ErrorCode::DeserializationError, "bytes EOF"};
        Bytes out(data_ + pos_, data_ + pos_ + n);
        pos_ += n;
        return out;
    }

    template <size_t N>
    Result<std::array<uint8_t, N>> fixed() {
        if (pos_ + N > len_) return {ErrorCode::DeserializationError, "fixed EOF"};
        std::array<uint8_t, N> a{};
        std::memcpy(a.data(), data_ + pos_, N);
        pos_ += N;
        return a;
    }

    Result<Bytes> bytes_len() {
        auto n = u32();
        if (!n) return n.error();
        return bytes(n.value());
    }

    Result<std::string> str() {
        auto b = bytes_len();
        if (!b) return b.error();
        return std::string(b.value().begin(), b.value().end());
    }

private:
    const uint8_t* data_;
    size_t len_;
    size_t pos_;
};

/// Helpers de serialización genéricos usados por tests y codec.
Bytes encode_u64(uint64_t v);
Result<uint64_t> decode_u64(const Bytes& b);

}  // namespace l1::core
