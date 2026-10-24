# Contributing / Contribuciones

¡Gracias por contribuir al monorepo **blockchain-4capas**!

## Estructura

| Capa | Path | Stack |
|------|------|-------|
| L1 | `l1/` | C++20 · CMake · GTest |
| L2 | `l2/` | Rust workspace · `cargo test --workspace` |
| L3 | `l3/contracts`, `l3/sdk-ts`, `l3/indexer` | Rust, Node, Go |
| L4 | `l4/web`, `l4/mobile`, `l4/extension` | Next.js, Flutter, MV3 |

## Reglas

1. **Comentarios en español** en cabeceras de archivo y bloques/funciones no triviales.
2. Mantén cada capa autocontenida: no imports cruzados que rompan el build aislado.
3. Tests obligatorios para lógica de consenso, puente L2 y contratos.
4. No subas secretos, keys ni `.env`.
5. Formatea según el estilo del lenguaje del directorio (`clang-format`, `rustfmt`, `gofmt`, Prettier).

## Flujo

```bash
# Fork + branch
git checkout -b feat/mi-cambio

# Verifica la capa que tocas
cd l1 && cmake -B build && cmake --build build && cd build && ctest
cd l2 && cargo test --workspace
cd l3/contracts && cargo test --workspace
cd l3/sdk-ts && npm test
cd l3/indexer && go test ./...
cd l4/web && npm test

# PR con descripción de la capa afectada y resultados de tests
```

## Licencia

Al contribuir aceptas que el código se publique bajo la licencia del repositorio (MIT, ver `LICENSE`).
