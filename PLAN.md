# PLAN — Blockchain 4 Capas

**Progreso:** L1 ✅ 94/94 · L2 ✅ 30/30 · L3 ✅ 46/46 · L4 ✅ 56/56 — monorepo completo.

**Fallbacks L1 aplicados:** memory_store; BLAKE2b vía libsodium (blake3.cpp); BLS/VRF simplificados; TCP POSIX; VM L1BC; gRPC/QUIC stubs.

---

# Arquitectura Blockchain de 4 Capas — Listado completo de archivos

> **Convención global:** todos los archivos de código llevan un **comentario de cabecera** (qué hace el archivo y cómo encaja en la capa) y **comentarios en cada bloque/función** explicando qué hace cada parte.
> Usá las casillas `[ ]` para marcar el avance a medida que programás.

---

## 0. Visión general

```
┌──────────────────────────────────────────────────────────────┐
│ Layer 4 · User Interface   → TypeScript (Next.js) · Dart (Flutter)│
├──────────────────────────────────────────────────────────────┤
│ Layer 3 · Dapps            → Rust→WASM (contratos) · TS (SDK) · Go (indexer)│
├──────────────────────────────────────────────────────────────┤
│ Layer 2 · Networks         → Rust (sequencer, prover, bridge) │
├──────────────────────────────────────────────────────────────┤
│ Layer 1 · Blockchain       → C++20 (nodo, consenso, VM, P2P)  │
└──────────────────────────────────────────────────────────────┘
```

| Capa | Rol | Lenguaje elegido | Por qué |
|---|---|---|---|
| **L1** | Consenso, estado, seguridad, ejecución base | **C++20** | Control total de memoria y rendimiento; ideal para nodos de alto throughput |
| **L2** | Escalado: rollup (batching + pruebas) y puente con L1 | **Rust** | Seguridad de memoria + ecosistema ZK (arkworks, plonky) + async (tokio) |
| **L3** | Aplicaciones: contratos, SDK, indexador | **Rust→WASM** (contratos), **TypeScript** (SDK), **Go** (indexador) | WASM es el target de la VM del L1; TS es el estándar de integración web; Go rinde muy bien para servicios de indexado/API |
| **L4** | Interfaz de usuario: web, móvil, extensión | **TypeScript** (Next.js/React), **Dart** (Flutter) | Máxima velocidad de desarrollo y multiplataforma |

### Cómo se comunican las capas

```
L4 (UI) ──HTTP/WS──► L3 (SDK TS / Indexer Go / Contratos WASM)
L3      ──JSON-RPC──► L2 (Sequencer)  ──batches + proofs──► L1 (nodo C++)
L3      ──JSON-RPC──► L1 (lecturas directas / transacciones directas)
```

---

# LAYER 1 — Blockchain (C++20)

**Total: 94 archivos** → a 12 archivos/día ≈ **8 días**.

Estructura raíz: `l1/`

### 1.1 Raíz / Build (3)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 1 | `CMakeLists.txt` | Define el build: módulos, dependencias, flags (C++20, sanitizers) |
| [x] 2 | `vcpkg.json` | Dependencias: RocksDB, libsodium, asio, nlohmann-json, gtest, wasmtime |
| [x] 3 | `.clang-format` | Estilo de código uniforme |

### 1.2 `src/core/` — Tipos base (13)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 4 | `types.hpp` | Alias básicos: `Hash256`, `Address`, `Bytes`, `Amount`, `BlockHeight` |
| [x] 5 | `error.hpp` | Códigos de error y tipo `Result<T>` |
| [x] 6 | `serialize.hpp` | Interfaz de serialización canónica (encode/decode) |
| [x] 7 | `serialize.cpp` | Implementación de la serialización binaria determinista |
| [x] 8 | `transaction.hpp` | Estructura de transacción (from, to, nonce, value, gas, firma) |
| [x] 9 | `transaction.cpp` | Hash, firma y validación estructural de la tx |
| [x] 10 | `block_header.hpp` | Cabecera: altura, parent hash, state root, tx root, proposer, timestamp |
| [x] 11 | `block.hpp` | Bloque = cabecera + lista de transacciones + firmas |
| [x] 12 | `block.cpp` | Cálculo del hash del bloque y verificación de integridad |
| [x] 13 | `account.hpp` | Cuenta: balance, nonce, code hash, storage root |
| [x] 14 | `chain_params.hpp` | Parámetros: tiempo de bloque, gas límite, recompensas, chain ID |
| [x] 15 | `genesis.hpp` | Estructura del bloque génesis |
| [x] 16 | `genesis.cpp` | Carga y construcción del génesis desde JSON |

### 1.3 `src/crypto/` — Criptografía (12)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 17 | `hash.hpp` | Interfaz unificada de funciones hash |
| [x] 18 | `sha256.cpp` | SHA-256 |
| [x] 19 | `keccak256.cpp` | Keccak-256 (hash de direcciones y estado) |
| [x] 20 | `blake3.cpp` | BLAKE3 (hash rápido para uso interno) |
| [x] 21 | `ed25519.cpp` | Firmas Ed25519 (validadores) |
| [x] 22 | `secp256k1.cpp` | Firmas secp256k1 (compatibilidad con wallets) |
| [x] 23 | `bls.cpp` | Firmas BLS y agregación (votos de finalidad) |
| [x] 24 | `vrf.cpp` | VRF para selección aleatoria verificable de proposers |
| [x] 25 | `merkle_tree.hpp` | Interfaz del árbol de Merkle |
| [x] 26 | `merkle_tree.cpp` | Construcción de raíces y pruebas de inclusión |
| [x] 27 | `address.cpp` | Derivación y validación de direcciones (checksum) |
| [x] 28 | `keystore.cpp` | Almacén cifrado de claves del validador |

### 1.4 `src/state/` — Estado (7)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 29 | `state_db.hpp` | Interfaz de la base de estado (cuentas + storage) |
| [x] 30 | `state_db.cpp` | Lectura/escritura de estado con caché y journal de cambios |
| [x] 31 | `trie.hpp` | Interfaz del Merkle Patricia Trie |
| [x] 32 | `trie.cpp` | Implementación del trie (state root verificable) |
| [x] 33 | `state_transition.cpp` | Aplica una transacción al estado (débito, crédito, nonce) |
| [x] 34 | `executor.cpp` | Ejecuta un bloque completo y produce el nuevo state root |
| [x] 35 | `snapshot.cpp` | Snapshots del estado para sync rápido y rollback |

### 1.4b `src/storage/` — Persistencia (6)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 36 | `kv_store.hpp` | Interfaz abstracta clave-valor |
| [x] 37 | `rocksdb_store.cpp` | Backend RocksDB (producción) |
| [x] 38 | `memory_store.cpp` | Backend en memoria (tests) |
| [x] 39 | `block_store.cpp` | Guarda y recupera bloques por hash/altura |
| [x] 40 | `tx_index.cpp` | Índice de transacciones por hash |
| [x] 41 | `pruner.cpp` | Poda de datos históricos |

### 1.5 `src/consensus/` — Consenso PoS + finalidad BFT (10)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 42 | `consensus_engine.hpp` | Interfaz del motor de consenso (intercambiable) |
| [x] 43 | `pos_engine.cpp` | Motor Proof-of-Stake: propone, vota y confirma bloques |
| [x] 44 | `validator_set.cpp` | Set de validadores, stake y activación/salida |
| [x] 45 | `proposer_selection.cpp` | Elige el proposer de cada slot (ponderado por stake + VRF) |
| [x] 46 | `block_validator.cpp` | Valida un bloque recibido (firmas, roots, reglas) |
| [x] 47 | `fork_choice.cpp` | Regla de elección de cadena (LMD-GHOST) |
| [x] 48 | `finality_gadget.cpp` | Finalidad por votos BLS (estilo Casper FFG) |
| [x] 49 | `slashing.cpp` | Detección y castigo de doble firma / inactividad |
| [x] 50 | `rewards.cpp` | Cálculo y distribución de recompensas |
| [x] 51 | `epoch_manager.cpp` | Manejo de épocas y rotación de validadores |

### 1.6 `src/mempool/` — Pool de transacciones (4)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 52 | `mempool.hpp` | Interfaz del mempool |
| [x] 53 | `mempool.cpp` | Almacena, valida y descarta transacciones pendientes |
| [x] 54 | `fee_market.cpp` | Mercado de tarifas (base fee dinámica estilo EIP-1559) |
| [x] 55 | `nonce_tracker.cpp` | Control de nonces por cuenta (evita huecos y duplicados) |

### 1.7 `src/vm/` — Máquina virtual WASM (7)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 56 | `vm_interface.hpp` | Interfaz de la VM (deploy, call, estimate gas) |
| [x] 57 | `wasm_vm.cpp` | Ejecuta contratos WebAssembly (wasmtime embebido) |
| [x] 58 | `gas_meter.cpp` | Medición y cobro de gas por instrucción |
| [x] 59 | `host_functions.cpp` | Funciones que el contrato puede llamar (storage, hash, transfer) |
| [x] 60 | `precompiles.cpp` | Contratos nativos rápidos (verificar firmas, hashes) |
| [x] 61 | `native_contracts.cpp` | Contratos de sistema: staking, tesorería, verificador de L2 |
| [x] 62 | `contract_storage.cpp` | Almacenamiento persistente por contrato |

### 1.8 `src/p2p/` — Red (10)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 63 | `messages.hpp` | Definición de mensajes de red (tx, bloque, voto, ping) |
| [x] 64 | `codec.cpp` | Codificación/decodificación de mensajes |
| [x] 65 | `transport_tcp.cpp` | Transporte TCP con asio |
| [x] 66 | `transport_quic.cpp` | Transporte QUIC (menor latencia) |
| [x] 67 | `handshake.cpp` | Handshake autenticado entre nodos |
| [x] 68 | `peer_manager.cpp` | Gestión de peers, reputación y baneos |
| [x] 69 | `discovery_kademlia.cpp` | Descubrimiento de nodos (Kademlia DHT) |
| [x] 70 | `gossip.cpp` | Propagación eficiente de txs y bloques |
| [x] 71 | `block_sync.cpp` | Sincronización de bloques para nodos nuevos |
| [x] 72 | `rate_limiter.cpp` | Límite de tasa contra spam y DoS |

### 1.9 `src/rpc/` — API del nodo (5)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 73 | `json_rpc_server.cpp` | Servidor JSON-RPC sobre HTTP |
| [x] 74 | `rpc_methods.cpp` | Métodos: `getBalance`, `sendTx`, `getBlock`, `call`, etc. |
| [x] 75 | `websocket_pubsub.cpp` | Suscripciones en tiempo real (nuevos bloques/eventos) |
| [x] 76 | `grpc_service.cpp` | Servicio gRPC para L2 e indexadores |
| [x] 77 | `rpc_auth.cpp` | Autenticación y límites de la API |

### 1.10 `src/node/` — Ensamblado del nodo (7)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 78 | `main.cpp` | Punto de entrada del ejecutable `l1d` |
| [x] 79 | `node.cpp` | Orquesta todos los módulos (arranque, bucle principal, apagado) |
| [x] 80 | `config.cpp` | Lectura de configuración (TOML/CLI) |
| [x] 81 | `logger.cpp` | Logging estructurado |
| [x] 82 | `metrics.cpp` | Métricas Prometheus |
| [x] 83 | `l2_verifier.cpp` | Verifica pruebas/batches enviados por L2 |
| [x] 84 | `signal_handler.cpp` | Apagado limpio con SIGINT/SIGTERM |

### 1.11 `tools/` y `tests/` (10)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 85 | `tools/l1_cli.cpp` | CLI de administración del nodo |
| [x] 86 | `tools/wallet_cli.cpp` | Wallet de línea de comandos (crear clave, firmar, enviar) |
| [x] 87 | `tools/genesis_gen.cpp` | Generador de génesis para testnets |
| [x] 88 | `tests/test_crypto.cpp` | Tests de hashes y firmas |
| [x] 89 | `tests/test_core.cpp` | Tests de tx, bloques y serialización |
| [x] 90 | `tests/test_consensus.cpp` | Tests de consenso, fork choice y slashing |
| [x] 91 | `tests/test_vm.cpp` | Tests de VM y gas |
| [x] 92 | `tests/test_p2p.cpp` | Tests de red y gossip |
| [x] 93 | `tests/test_integration.cpp` | Red local de varios nodos de punta a punta |
| [x] 94 | `tests/bench_tps.cpp` | Benchmark de transacciones por segundo |

**Orden de construcción sugerido (por día, 12 archivos/día):**

| Día | Archivos | Módulos |
|---|---|---|
| 1 | 1–12 | Build + core |
| 2 | 13–24 | core (fin) + crypto |
| 3 | 25–36 | crypto (fin) + state + inicio storage |
| 4 | 37–48 | storage + consenso (parte 1) |
| 5 | 49–60 | consenso (fin) + mempool + VM (parte 1) |
| 6 | 61–72 | VM (fin) + p2p |
| 7 | 73–84 | rpc + node |
| 8 | 85–94 | tools + tests |

---

# LAYER 2 — Networks (Rust)

Rollup ZK: agrupa miles de transacciones, las ejecuta fuera del L1 y publica un lote + prueba de validez. Workspace: `l2/`

**Total: 30 archivos**

### 2.1 Raíz

| # | Archivo | Qué hace |
|---|---|---|
| [x] 1 | `Cargo.toml` | Workspace con todos los crates |
| [x] 2 | `rust-toolchain.toml` | Versión fija del compilador |

### 2.2 `crates/common/` — Tipos compartidos

| # | Archivo | Qué hace |
|---|---|---|
| [x] 3 | `src/lib.rs` | Exporta el crate |
| [x] 4 | `src/types.rs` | Tx, Batch, StateRoot, Proof |
| [x] 5 | `src/config.rs` | Configuración del L2 |
| [x] 6 | `src/error.rs` | Errores unificados |

### 2.3 `crates/sequencer/` — Ordena transacciones

| # | Archivo | Qué hace |
|---|---|---|
| [x] 7 | `src/main.rs` | Arranque del sequencer |
| [x] 8 | `src/mempool.rs` | Recibe y valida txs de usuarios |
| [x] 9 | `src/ordering.rs` | Política de orden (FIFO + prioridad por fee) |
| [x] 10 | `src/batch_builder.rs` | Arma los lotes de transacciones |

### 2.4 `crates/executor/` — Ejecución off-chain

| # | Archivo | Qué hace |
|---|---|---|
| [x] 11 | `src/lib.rs` | Exporta el crate |
| [x] 12 | `src/state.rs` | Estado del L2 (árbol de cuentas) |
| [x] 13 | `src/executor.rs` | Ejecuta lotes y calcula el nuevo state root |
| [x] 14 | `src/trace.rs` | Genera la traza de ejecución para el prover |

### 2.5 `crates/prover/` — Pruebas de validez ZK

| # | Archivo | Qué hace |
|---|---|---|
| [x] 15 | `src/lib.rs` | Exporta el crate |
| [x] 16 | `src/circuit.rs` | Circuito que codifica las reglas de transición de estado |
| [x] 17 | `src/prover.rs` | Genera la prueba para un lote |
| [x] 18 | `src/aggregator.rs` | Agrega varias pruebas en una sola |

### 2.6 `crates/da/` — Disponibilidad de datos

| # | Archivo | Qué hace |
|---|---|---|
| [x] 19 | `src/compressor.rs` | Comprime los datos del lote |
| [x] 20 | `src/da_publisher.rs` | Publica los datos comprimidos en L1 |

### 2.7 `crates/bridge/` — Puente L1 ↔ L2

| # | Archivo | Qué hace |
|---|---|---|
| [x] 21 | `src/l1_client.rs` | Cliente RPC/gRPC hacia el nodo C++ |
| [x] 22 | `src/deposit_watcher.rs` | Detecta depósitos en L1 y acredita en L2 |
| [x] 23 | `src/withdrawal.rs` | Procesa retiros L2 → L1 con prueba |
| [x] 24 | `src/relayer.rs` | Envía lotes y pruebas al verificador del L1 |

### 2.8 `crates/rpc/` y `contracts/`

| # | Archivo | Qué hace |
|---|---|---|
| [x] 25 | `rpc/src/server.rs` | JSON-RPC del L2 (compatible con el SDK) |
| [x] 26 | `rpc/src/methods.rs` | Métodos del L2 (`sendTx`, `getBalance`, `getBatch`) |
| [x] 27 | `contracts/rollup_bridge/src/lib.rs` | Contrato WASM en L1: custodia fondos y recibe lotes |
| [x] 28 | `contracts/rollup_bridge/Cargo.toml` | Manifiesto del contrato |
| [x] 29 | `tests/e2e_rollup.rs` | Test de extremo a extremo: depósito → lote → retiro |
| [x] 30 | `tests/prover_bench.rs` | Benchmark del prover |

> **Alternativa opcional:** *state channels* para micropagos (`crates/channels/`: `channel.rs`, `watchtower.rs`).

---

# LAYER 3 — Dapps (Rust→WASM · TypeScript · Go)

Raíz: `l3/`

**Total: 46 archivos**

### 3.1 `contracts/` — Contratos inteligentes (Rust → WASM)

**SDK de contratos (`contract-sdk/`)**

| # | Archivo | Qué hace |
|---|---|---|
| [x] 1 | `Cargo.toml` | Manifiesto del SDK |
| [x] 2 | `src/lib.rs` | API pública del SDK |
| [x] 3 | `src/env.rs` | Acceso al entorno (caller, bloque, valor) |
| [x] 4 | `src/storage.rs` | Abstracción de almacenamiento clave-valor |
| [x] 5 | `src/events.rs` | Emisión de eventos |
| [x] 6 | `src/macros.rs` | Macros `#[contract]`, `#[entry]` |

**Contratos estándar**

| # | Archivo | Qué hace |
|---|---|---|
| [x] 7 | `token_ft/src/lib.rs` | Token fungible (tipo ERC-20) |
| [x] 8 | `token_nft/src/lib.rs` | NFT (tipo ERC-721) |
| [x] 9 | `token_multi/src/lib.rs` | Multi-token (tipo ERC-1155) |
| [x] 10 | `dex_amm/src/lib.rs` | Exchange descentralizado (AMM x·y=k) |
| [x] 11 | `dex_amm/src/pool.rs` | Lógica de pools de liquidez |
| [x] 12 | `staking_pool/src/lib.rs` | Staking y delegación |
| [x] 13 | `governance_dao/src/lib.rs` | Propuestas y votación on-chain |
| [x] 14 | `multisig/src/lib.rs` | Wallet multifirma |
| [x] 15 | `lending/src/lib.rs` | Préstamos con colateral |
| [x] 16 | `oracle/src/lib.rs` | Oráculo de precios |

### 3.2 `sdk-ts/` — SDK de integración (TypeScript)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 17 | `package.json` | Dependencias y scripts |
| [x] 18 | `tsconfig.json` | Configuración de TypeScript |
| [x] 19 | `src/index.ts` | Exporta la API pública |
| [x] 20 | `src/types.ts` | Tipos compartidos |
| [x] 21 | `src/client.ts` | Cliente JSON-RPC (L1 y L2) |
| [x] 22 | `src/wallet.ts` | Claves, firma y derivación de cuentas |
| [x] 23 | `src/tx_builder.ts` | Construcción y firma de transacciones |
| [x] 24 | `src/contract.ts` | Interacción con contratos (deploy/call) |
| [x] 25 | `src/abi.ts` | Codificación de llamadas y eventos |
| [x] 26 | `src/events.ts` | Suscripción a eventos por WebSocket |
| [x] 27 | `src/utils.ts` | Utilidades (formato de montos, hex, direcciones) |
| [x] 28 | `tests/sdk.test.ts` | Tests del SDK |

### 3.3 `indexer/` — Indexador + API de datos (Go)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 29 | `go.mod` | Módulo y dependencias |
| [x] 30 | `cmd/indexer/main.go` | Punto de entrada |
| [x] 31 | `internal/fetcher/block_fetcher.go` | Descarga bloques del nodo (L1 y L2) |
| [x] 32 | `internal/decoder/decoder.go` | Decodifica transacciones y eventos |
| [x] 33 | `internal/db/schema.sql` | Esquema PostgreSQL |
| [x] 34 | `internal/db/db.go` | Capa de acceso a datos |
| [x] 35 | `internal/api/rest.go` | API REST (bloques, txs, cuentas, tokens) |
| [x] 36 | `internal/api/websocket.go` | Streaming de nuevos datos |
| [x] 37 | `internal/reorg/reorg_handler.go` | Maneja reorganizaciones de cadena |
| [x] 38 | `internal/stats/stats.go` | Estadísticas (TPS, usuarios activos, TVL) |

### 3.4 `dapps/` — Aplicaciones de ejemplo y despliegue

| # | Archivo | Qué hace |
|---|---|---|
| [x] 39 | `deploy/deploy_all.ts` | Despliega todos los contratos en testnet |
| [x] 40 | `deploy/addresses.json` | Direcciones desplegadas por red |
| [x] 41 | `dapps/swap/swap_logic.ts` | Lógica de la dapp de swap sobre el SDK |
| [x] 42 | `dapps/staking/staking_logic.ts` | Lógica de la dapp de staking |
| [x] 43 | `dapps/governance/gov_logic.ts` | Lógica de la dapp de gobernanza |
| [x] 44 | `dapps/nft_market/market_logic.ts` | Lógica de marketplace NFT |
| [x] 45 | `tests/contracts_integration.rs` | Tests de contratos contra un nodo local |
| [x] 46 | `docs/CONTRACT_GUIDE.md` | Guía para desarrollar contratos |

---

# LAYER 4 — User Interface (TypeScript · Dart)

Raíz: `l4/`

**Total: 56 archivos**

### 4.1 `web/` — Web app + Explorer (Next.js · React · TypeScript · Tailwind)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 1 | `package.json` | Dependencias |
| [x] 2 | `next.config.js` | Configuración de Next.js |
| [x] 3 | `tailwind.config.ts` | Tema y estilos |
| [x] 4 | `src/app/layout.tsx` | Layout global (navbar, footer, providers) |
| [x] 5 | `src/app/page.tsx` | Página de inicio / dashboard |
| [x] 6 | `src/app/wallet/page.tsx` | Vista de la wallet (balances, historial) |
| [x] 7 | `src/app/send/page.tsx` | Enviar transacciones |
| [x] 8 | `src/app/swap/page.tsx` | Interfaz del DEX |
| [x] 9 | `src/app/stake/page.tsx` | Interfaz de staking |
| [x] 10 | `src/app/governance/page.tsx` | Votación y propuestas |
| [x] 11 | `src/app/nft/page.tsx` | Galería y marketplace NFT |
| [x] 12 | `src/app/explorer/page.tsx` | Inicio del explorer (últimos bloques/txs) |
| [x] 13 | `src/app/explorer/block/[id]/page.tsx` | Detalle de bloque |
| [x] 14 | `src/app/explorer/tx/[hash]/page.tsx` | Detalle de transacción |
| [x] 15 | `src/app/explorer/address/[addr]/page.tsx` | Detalle de cuenta |
| [x] 16 | `src/components/ConnectWallet.tsx` | Botón de conexión de wallet |
| [x] 17 | `src/components/BalanceCard.tsx` | Tarjeta de balance |
| [x] 18 | `src/components/TxModal.tsx` | Modal de confirmación y estado de tx |
| [x] 19 | `src/components/TxTable.tsx` | Tabla de transacciones |
| [x] 20 | `src/components/NetworkSwitcher.tsx` | Cambio entre L1 / L2 / testnet |
| [x] 21 | `src/components/SearchBar.tsx` | Búsqueda de bloque, tx o dirección |
| [x] 22 | `src/hooks/useWallet.ts` | Estado de la wallet |
| [x] 23 | `src/hooks/useChain.ts` | Estado de la cadena (altura, red) |
| [x] 24 | `src/hooks/useContract.ts` | Llamadas a contratos |
| [x] 25 | `src/lib/rpc.ts` | Instancia del SDK configurada |
| [x] 26 | `src/lib/i18n.ts` | Internacionalización (ES/EN) |
| [x] 27 | `src/lib/format.ts` | Formato de montos, fechas y hashes |

### 4.2 `mobile/` — App móvil (Flutter · Dart)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 28 | `pubspec.yaml` | Dependencias |
| [x] 29 | `lib/main.dart` | Punto de entrada |
| [x] 30 | `lib/core/rpc_client.dart` | Cliente JSON-RPC |
| [x] 31 | `lib/core/wallet_service.dart` | Generación de claves y firma |
| [x] 32 | `lib/core/secure_storage.dart` | Almacenamiento seguro (Keychain/Keystore) |
| [x] 33 | `lib/core/biometrics.dart` | Autenticación biométrica |
| [x] 34 | `lib/screens/onboarding_screen.dart` | Creación/importación de wallet |
| [x] 35 | `lib/screens/home_screen.dart` | Balances y actividad |
| [x] 36 | `lib/screens/send_screen.dart` | Enviar fondos |
| [x] 37 | `lib/screens/receive_screen.dart` | Recibir (QR + dirección) |
| [x] 38 | `lib/screens/stake_screen.dart` | Staking |
| [x] 39 | `lib/screens/swap_screen.dart` | Intercambio de tokens |
| [x] 40 | `lib/screens/history_screen.dart` | Historial de transacciones |
| [x] 41 | `lib/screens/settings_screen.dart` | Ajustes (red, idioma, seguridad) |
| [x] 42 | `lib/widgets/tx_tile.dart` | Elemento de lista de transacción |
| [x] 43 | `lib/widgets/qr_scanner.dart` | Escáner QR |

### 4.3 `extension/` — Wallet como extensión de navegador (TypeScript)

| # | Archivo | Qué hace |
|---|---|---|
| [x] 44 | `manifest.json` | Manifest V3 |
| [x] 45 | `src/background.ts` | Service worker: claves, firma y aprobaciones |
| [x] 46 | `src/content.ts` | Puente entre la página y la extensión |
| [x] 47 | `src/inpage.ts` | Proveedor inyectado (`window.l1`) para dapps |
| [x] 48 | `src/popup/Popup.tsx` | Interfaz del popup |
| [x] 49 | `src/popup/ApproveTx.tsx` | Pantalla de aprobación de transacciones |
| [x] 50 | `src/popup/Accounts.tsx` | Gestión de cuentas |
| [x] 51 | `src/crypto/vault.ts` | Bóveda cifrada de claves |

### 4.4 `shared/` y `e2e/`

| # | Archivo | Qué hace |
|---|---|---|
| [x] 52 | `shared/design_tokens.json` | Colores, tipografía y espaciados compartidos (web/móvil/extensión) |
| [x] 53 | `shared/locales/es.json` | Textos en español |
| [x] 54 | `shared/locales/en.json` | Textos en inglés |
| [x] 55 | `e2e/web.spec.ts` | Tests end-to-end de la web (Playwright) |
| [x] 56 | `e2e/mobile_test.dart` | Tests de integración de la app móvil |

---

## Resumen de totales

| Capa | Lenguaje principal | Archivos |
|---|---|---|
| L1 — Blockchain | C++20 | 94 |
| L2 — Networks | Rust | 30 |
| L3 — Dapps | Rust→WASM · TypeScript · Go | 46 |
| L4 — User Interface | TypeScript · Dart | 56 |
| **Total** | | **226** |

A 12 archivos por día: **L1 ≈ 8 días**, el proyecto completo ≈ **19 días**.

## Orden recomendado de construcción

1. **L1** completo (sin él no existe nada más).
2. **L3 · `sdk-ts`** (para poder hablar con el nodo desde cualquier cliente).
3. **L3 · contratos** + **indexer**.
4. **L2** (sequencer → executor → prover → bridge).
5. **L4** (web → extensión → móvil).
