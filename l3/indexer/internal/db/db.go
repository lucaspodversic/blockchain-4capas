// Package db — capa de acceso a datos.
// Encaja en: fetcher escribe; API lee.
// Producción: Postgres (schema.sql). Tests: SQLite en memoria.
package db

import (
	"database/sql"
	"fmt"
	"os"
	"strings"

	_ "modernc.org/sqlite"
)

type DB struct {
	SQL *sql.DB
}

func Open(dsn string) (*DB, error) {
	if dsn == "" || dsn == ":memory:" || strings.HasPrefix(dsn, "sqlite") {
		sqlDB, err := sql.Open("sqlite", ":memory:")
		if err != nil {
			return nil, err
		}
		d := &DB{SQL: sqlDB}
		if err := d.Migrate(); err != nil {
			return nil, err
		}
		return d, nil
	}
	// Postgres: driver no vinculado aquí; documentado para prod.
	return nil, fmt.Errorf("postgres DSN requiere driver pq en producción; usar sqlite para dev/tests")
}

func (d *DB) Migrate() error {
	schema, err := os.ReadFile("internal/db/schema.sql")
	if err != nil {
		// fallback embebido si cwd distinto
		schema = []byte(`
CREATE TABLE IF NOT EXISTS blocks (
 height INTEGER PRIMARY KEY, hash TEXT, parent_hash TEXT, state_root TEXT, timestamp INTEGER, tx_count INTEGER);
CREATE TABLE IF NOT EXISTS transactions (
 hash TEXT PRIMARY KEY, block_height INTEGER, tx_index INTEGER, from_addr TEXT, to_addr TEXT, value TEXT, status INTEGER);
CREATE TABLE IF NOT EXISTS accounts (
 address TEXT PRIMARY KEY, balance TEXT, nonce INTEGER);
CREATE TABLE IF NOT EXISTS tokens (
 address TEXT PRIMARY KEY, symbol TEXT, decimals INTEGER);
`)
	}
	_, err = d.SQL.Exec(string(schema))
	return err
}

type BlockRow struct {
	Height    int64
	Hash      string
	Parent    string
	StateRoot string
	Timestamp int64
	TxCount   int
}

func (d *DB) InsertBlock(b BlockRow) error {
	_, err := d.SQL.Exec(
		`INSERT OR REPLACE INTO blocks(height,hash,parent_hash,state_root,timestamp,tx_count) VALUES(?,?,?,?,?,?)`,
		b.Height, b.Hash, b.Parent, b.StateRoot, b.Timestamp, b.TxCount,
	)
	return err
}

func (d *DB) GetBlock(height int64) (*BlockRow, error) {
	row := d.SQL.QueryRow(`SELECT height,hash,parent_hash,state_root,timestamp,tx_count FROM blocks WHERE height=?`, height)
	var b BlockRow
	if err := row.Scan(&b.Height, &b.Hash, &b.Parent, &b.StateRoot, &b.Timestamp, &b.TxCount); err != nil {
		return nil, err
	}
	return &b, nil
}

func (d *DB) InsertTx(hash string, height int64, idx int, from, to, value string, status int) error {
	_, err := d.SQL.Exec(
		`INSERT OR REPLACE INTO transactions(hash,block_height,tx_index,from_addr,to_addr,value,status) VALUES(?,?,?,?,?,?,?)`,
		hash, height, idx, from, to, value, status,
	)
	return err
}

func (d *DB) Tip() (int64, error) {
	var h sql.NullInt64
	err := d.SQL.QueryRow(`SELECT MAX(height) FROM blocks`).Scan(&h)
	if err != nil {
		return 0, err
	}
	if !h.Valid {
		return -1, nil
	}
	return h.Int64, nil
}
