/** Internacionalización (ES/EN). */
import es from '../../../shared/locales/es.json';
import en from '../../../shared/locales/en.json';

const dicts: Record<string, Record<string, string>> = { es, en };
let lang = 'es';

export function setLang(l: string) {
  lang = dicts[l] ? l : 'es';
}

export function t(key: string): string {
  return dicts[lang]?.[key] ?? dicts.es[key] ?? key;
}
