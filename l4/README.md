# Layer 4 — User Interface

- `web/` — Next.js / React / Tailwind (explorer, wallet, dapps UI)
- `mobile/` — Flutter wallet (fuentes completas; `flutter` opcional en CI)
- `extension/` — Manifest V3 wallet
- `shared/` — design tokens + locales ES/EN
- `e2e/` — Playwright + Dart integration specs

```bash
cd web && npm install && npm test
cd ../extension && npm test
# mobile: flutter pub get && flutter test   # si flutter está instalado
```
