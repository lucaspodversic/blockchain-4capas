// =============================================================================
// codec.hpp — API de codec P2P
// =============================================================================
#pragma once
#include "messages.hpp"
#include "../core/error.hpp"

namespace l1::p2p {
core::Bytes encode_message(const Message& msg);
core::Result<Message> decode_message(const core::Bytes& frame);
}  // namespace l1::p2p
