/**
 * Página de inicio / dashboard.
 * Encaja en: resumen de red L1/L2 y accesos rápidos.
 */
'use client';
import React from 'react';
import { BalanceCard } from '../components/BalanceCard';
import { useChain } from '../hooks/useChain';
import { useWallet } from '../hooks/useWallet';
import { t } from '../lib/i18n';

export default function HomePage() {
  const chain = useChain();
  const wallet = useWallet();
  return (
    <div className="space-y-6">
      <h1 className="text-3xl font-bold">{t('home.title')}</h1>
      <p className="text-slate-600">
        {t('home.network')}: {chain.network} · {t('home.height')}: {chain.height}
      </p>
      <div className="grid gap-4 md:grid-cols-3">
        <BalanceCard label={t('home.balance')} amount={wallet.balance} symbol="L1" />
        <BalanceCard label="TPS" amount={String(chain.tps)} symbol="" />
        <BalanceCard label="L2 batches" amount={String(chain.l2Batches)} symbol="" />
      </div>
    </div>
  );
}
