import type { Config } from 'tailwindcss';
import tokens from '../shared/design_tokens.json';

const config: Config = {
  content: ['./src/**/*.{ts,tsx}'],
  theme: {
    extend: {
      colors: {
        brand: tokens.colors.brand,
        surface: tokens.colors.surface,
        ink: tokens.colors.ink,
      },
      fontFamily: {
        sans: tokens.typography.fontFamily.split(','),
      },
      spacing: tokens.spacing,
    },
  },
  plugins: [],
};
export default config;
