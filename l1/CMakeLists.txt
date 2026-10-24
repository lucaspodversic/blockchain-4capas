# =============================================================================
# CMakeLists.txt — Build del nodo L1 (C++20)
# Dependencias: OpenSSL, libsodium, nlohmann_json, GTest
# Opcional: RocksDB (L1_HAS_ROCKSDB), wasmtime (no requerido; VM bytecode propia)
# =============================================================================
cmake_minimum_required(VERSION 3.16)
project(l1_blockchain VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(L1_ENABLE_ASAN "AddressSanitizer" OFF)
option(L1_HAS_ROCKSDB "Link RocksDB" OFF)

if(L1_ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)
find_package(nlohmann_json 3.2.0 REQUIRED)

include(FetchContent)
# GTest from system sources if available
set(GTEST_ROOT /usr/src/googletest)
if(EXISTS "${GTEST_ROOT}/CMakeLists.txt")
  add_subdirectory(${GTEST_ROOT} ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
else()
  find_package(GTest REQUIRED)
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${SODIUM_INCLUDE_DIRS}
)

set(L1_CORE_SOURCES
  src/core/serialize.cpp
  src/core/transaction.cpp
  src/core/block.cpp
  src/core/genesis.cpp
)

set(L1_CRYPTO_SOURCES
  src/crypto/sha256.cpp
  src/crypto/keccak256.cpp
  src/crypto/blake3.cpp
  src/crypto/ed25519.cpp
  src/crypto/secp256k1.cpp
  src/crypto/bls.cpp
  src/crypto/vrf.cpp
  src/crypto/merkle_tree.cpp
  src/crypto/address.cpp
  src/crypto/keystore.cpp
)

set(L1_STATE_SOURCES
  src/state/trie.cpp
  src/state/state_db.cpp
  src/state/state_transition.cpp
  src/state/executor.cpp
  src/state/snapshot.cpp
)

set(L1_STORAGE_SOURCES
  src/storage/memory_store.cpp
  src/storage/rocksdb_store.cpp
  src/storage/block_store.cpp
  src/storage/tx_index.cpp
  src/storage/pruner.cpp
)

set(L1_CONSENSUS_SOURCES
  src/consensus/validator_set.cpp
  src/consensus/proposer_selection.cpp
  src/consensus/block_validator.cpp
  src/consensus/fork_choice.cpp
  src/consensus/finality_gadget.cpp
  src/consensus/slashing.cpp
  src/consensus/rewards.cpp
  src/consensus/epoch_manager.cpp
  src/consensus/pos_engine.cpp
)

set(L1_MEMPOOL_SOURCES
  src/mempool/mempool.cpp
  src/mempool/fee_market.cpp
  src/mempool/nonce_tracker.cpp
)

set(L1_VM_SOURCES
  src/vm/gas_meter.cpp
  src/vm/contract_storage.cpp
  src/vm/host_functions.cpp
  src/vm/wasm_vm.cpp
  src/vm/precompiles.cpp
  src/vm/native_contracts.cpp
)

set(L1_P2P_SOURCES
  src/p2p/codec.cpp
  src/p2p/transport_tcp.cpp
  src/p2p/transport_quic.cpp
  src/p2p/handshake.cpp
  src/p2p/peer_manager.cpp
  src/p2p/discovery_kademlia.cpp
  src/p2p/gossip.cpp
  src/p2p/block_sync.cpp
  src/p2p/rate_limiter.cpp
)

set(L1_RPC_SOURCES
  src/rpc/json_rpc_server.cpp
  src/rpc/rpc_methods.cpp
  src/rpc/websocket_pubsub.cpp
  src/rpc/grpc_service.cpp
  src/rpc/rpc_auth.cpp
)

set(L1_NODE_SOURCES
  src/node/node.cpp
  src/node/config.cpp
  src/node/logger.cpp
  src/node/metrics.cpp
  src/node/l2_verifier.cpp
  src/node/signal_handler.cpp
)

add_library(l1_lib STATIC
  ${L1_CORE_SOURCES}
  ${L1_CRYPTO_SOURCES}
  ${L1_STATE_SOURCES}
  ${L1_STORAGE_SOURCES}
  ${L1_CONSENSUS_SOURCES}
  ${L1_MEMPOOL_SOURCES}
  ${L1_VM_SOURCES}
  ${L1_P2P_SOURCES}
  ${L1_RPC_SOURCES}
  ${L1_NODE_SOURCES}
)

target_link_libraries(l1_lib PUBLIC
  OpenSSL::SSL OpenSSL::Crypto
  ${SODIUM_LIBRARIES}
  nlohmann_json::nlohmann_json
  pthread
)

target_compile_options(l1_lib PRIVATE -Wall -Wextra -Wno-unused-parameter)
target_include_directories(l1_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(L1_HAS_ROCKSDB)
  find_package(RocksDB REQUIRED)
  target_link_libraries(l1_lib PUBLIC RocksDB::rocksdb)
  target_compile_definitions(l1_lib PUBLIC L1_HAS_ROCKSDB=1)
endif()

add_executable(l1d src/node/main.cpp)
target_link_libraries(l1d PRIVATE l1_lib)

add_executable(l1_cli tools/l1_cli.cpp)
target_link_libraries(l1_cli PRIVATE pthread)

add_executable(wallet_cli tools/wallet_cli.cpp)
target_link_libraries(wallet_cli PRIVATE l1_lib)

add_executable(genesis_gen tools/genesis_gen.cpp)
target_link_libraries(genesis_gen PRIVATE l1_lib)

enable_testing()

function(l1_test name src)
  add_executable(${name} ${src})
  target_link_libraries(${name} PRIVATE l1_lib gtest gtest_main)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

l1_test(test_crypto tests/test_crypto.cpp)
l1_test(test_core tests/test_core.cpp)
l1_test(test_consensus tests/test_consensus.cpp)
l1_test(test_vm tests/test_vm.cpp)
l1_test(test_p2p tests/test_p2p.cpp)
l1_test(test_integration tests/test_integration.cpp)

add_executable(bench_tps tests/bench_tps.cpp)
target_link_libraries(bench_tps PRIVATE l1_lib)
