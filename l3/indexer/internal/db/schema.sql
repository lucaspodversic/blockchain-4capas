-- Esquema PostgreSQL (producción). Tests usan SQLite con tipos afines.
CREATE TABLE IF NOT EXISTS blocks (
    height BIGINT PRIMARY KEY,
    hash TEXT NOT NULL,
    parent_hash TEXT NOT NULL,
    state_root TEXT NOT NULL,
    timestamp BIGINT NOT NULL,
    tx_count INT NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS transactions (
    hash TEXT PRIMARY KEY,
    block_height BIGINT NOT NULL,
    tx_index INT NOT NULL,
    from_addr TEXT NOT NULL,
    to_addr TEXT NOT NULL,
    value TEXT NOT NULL,
    status INT NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS accounts (
    address TEXT PRIMARY KEY,
    balance TEXT NOT NULL DEFAULT '0',
    nonce BIGINT NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS tokens (
    address TEXT PRIMARY KEY,
    symbol TEXT NOT NULL,
    decimals INT NOT NULL DEFAULT 18
);

CREATE INDEX IF NOT EXISTS idx_tx_block ON transactions(block_height);
CREATE INDEX IF NOT EXISTS idx_tx_from ON transactions(from_addr);
