/**
 * Layout global — navbar, footer y providers.
 * Encaja en: todas las páginas de la web L4.
 */
import React from 'react';
import { NetworkSwitcher } from '../components/NetworkSwitcher';
import { ConnectWallet } from '../components/ConnectWallet';
import { t } from '../lib/i18n';

export const metadata = {
  title: 'L1 Blockchain — Dashboard',
  description: 'Capa de interfaz del stack blockchain-4capas',
};

export default function RootLayout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="es">
      <body className="min-h-screen bg-surface text-ink antialiased">
        <header className="border-b border-slate-200 px-6 py-4 flex items-center justify-between">
          <nav className="flex gap-4 text-sm font-medium">
            <a href="/">{t('nav.home')}</a>
            <a href="/wallet">{t('nav.wallet')}</a>
            <a href="/swap">{t('nav.swap')}</a>
            <a href="/stake">{t('nav.stake')}</a>
            <a href="/governance">{t('nav.gov')}</a>
            <a href="/nft">{t('nav.nft')}</a>
            <a href="/explorer">{t('nav.explorer')}</a>
          </nav>
          <div className="flex items-center gap-3">
            <NetworkSwitcher />
            <ConnectWallet />
          </div>
        </header>
        <main className="mx-auto max-w-6xl px-6 py-8">{children}</main>
        <footer className="border-t px-6 py-4 text-center text-xs text-slate-500">
          blockchain-4capas · L4 UI
        </footer>
      </body>
    </html>
  );
}
