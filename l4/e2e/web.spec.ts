/**
 * Tests end-to-end de la web (Playwright).
 * Ejecutar: npx playwright test e2e/web.spec.ts (con servidor next en :3000).
 */
import { test, expect } from '@playwright/test';

test.describe('L4 web', () => {
  test('home muestra dashboard', async ({ page }) => {
    await page.goto('http://127.0.0.1:3000/');
    await expect(page.getByRole('heading', { level: 1 })).toBeVisible();
  });

  test('explorer lista bloques', async ({ page }) => {
    await page.goto('http://127.0.0.1:3000/explorer');
    await expect(page.getByText('Explorer')).toBeVisible();
  });
});
