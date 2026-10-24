// =============================================================================
// error.hpp — Códigos de error y Result<T> monádico ligero
// Encaja en: manejo uniforme de fallos en core, crypto, state, consenso, etc.
// =============================================================================
#pragma once

#include <string>
#include <stdexcept>
#include <utility>
#include <variant>
#include <optional>

namespace l1::core {

/// Códigos de error tipados del nodo L1.
enum class ErrorCode {
    Ok = 0,
    InvalidArgument,
    NotFound,
    AlreadyExists,
    SerializationError,
    DeserializationError,
    InvalidSignature,
    InvalidHash,
    InvalidNonce,
    InsufficientBalance,
    InsufficientGas,
    GasLimitExceeded,
    InvalidBlock,
    InvalidTransaction,
    ConsensusFailure,
    ForkChoiceError,
    StorageError,
    NetworkError,
    Timeout,
    Unauthorized,
    VmTrap,
    VmOutOfGas,
    ContractError,
    ConfigError,
    InternalError,
};

/// Mensaje humano asociado a un código.
inline const char* error_code_str(ErrorCode c) {
    switch (c) {
        case ErrorCode::Ok: return "ok";
        case ErrorCode::InvalidArgument: return "invalid_argument";
        case ErrorCode::NotFound: return "not_found";
        case ErrorCode::AlreadyExists: return "already_exists";
        case ErrorCode::SerializationError: return "serialization_error";
        case ErrorCode::DeserializationError: return "deserialization_error";
        case ErrorCode::InvalidSignature: return "invalid_signature";
        case ErrorCode::InvalidHash: return "invalid_hash";
        case ErrorCode::InvalidNonce: return "invalid_nonce";
        case ErrorCode::InsufficientBalance: return "insufficient_balance";
        case ErrorCode::InsufficientGas: return "insufficient_gas";
        case ErrorCode::GasLimitExceeded: return "gas_limit_exceeded";
        case ErrorCode::InvalidBlock: return "invalid_block";
        case ErrorCode::InvalidTransaction: return "invalid_transaction";
        case ErrorCode::ConsensusFailure: return "consensus_failure";
        case ErrorCode::ForkChoiceError: return "fork_choice_error";
        case ErrorCode::StorageError: return "storage_error";
        case ErrorCode::NetworkError: return "network_error";
        case ErrorCode::Timeout: return "timeout";
        case ErrorCode::Unauthorized: return "unauthorized";
        case ErrorCode::VmTrap: return "vm_trap";
        case ErrorCode::VmOutOfGas: return "vm_out_of_gas";
        case ErrorCode::ContractError: return "contract_error";
        case ErrorCode::ConfigError: return "config_error";
        case ErrorCode::InternalError: return "internal_error";
    }
    return "unknown";
}

/// Error con código + mensaje opcional.
struct Error {
    ErrorCode code{ErrorCode::InternalError};
    std::string message;

    Error() = default;
    Error(ErrorCode c, std::string msg = {}) : code(c), message(std::move(msg)) {}

    std::string to_string() const {
        if (message.empty()) return error_code_str(code);
        return std::string(error_code_str(code)) + ": " + message;
    }
};

/// Result<T>: o bien un valor T, o un Error.
template <typename T>
class Result {
public:
    Result(T value) : data_(std::move(value)) {}
    Result(Error err) : data_(std::move(err)) {}
    Result(ErrorCode c, std::string msg = {}) : data_(Error{c, std::move(msg)}) {}

    bool ok() const { return std::holds_alternative<T>(data_); }
    explicit operator bool() const { return ok(); }

    T& value() & { return std::get<T>(data_); }
    const T& value() const& { return std::get<T>(data_); }
    T&& value() && { return std::get<T>(std::move(data_)); }

    const Error& error() const { return std::get<Error>(data_); }

    /// Extrae el valor o lanza std::runtime_error (solo para tests/CLI).
    T unwrap() const {
        if (!ok()) throw std::runtime_error(error().to_string());
        return std::get<T>(data_);
    }

private:
    std::variant<T, Error> data_;
};

/// Especialización para void — solo éxito o error.
template <>
class Result<void> {
public:
    Result() : err_(std::nullopt) {}
    Result(Error err) : err_(std::move(err)) {}
    Result(ErrorCode c, std::string msg = {}) : err_(Error{c, std::move(msg)}) {}

    bool ok() const { return !err_.has_value(); }
    explicit operator bool() const { return ok(); }
    const Error& error() const { return *err_; }

    static Result Ok() { return Result(); }

private:
    std::optional<Error> err_;
};

}  // namespace l1::core
