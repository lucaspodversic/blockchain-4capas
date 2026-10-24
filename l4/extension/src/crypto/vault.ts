/**
 * Bóveda cifrada de claves.
 * Demo: XOR + base64 (NO usar en producción — usar AES-GCM + PBKDF2).
 */
function xor(data: Uint8Array, key: Uint8Array): Uint8Array {
  const out = new Uint8Array(data.length);
  for (let i = 0; i < data.length; i++) out[i] = data[i] ^ key[i % key.length];
  return out;
}

function toB64(u8: Uint8Array): string {
  if (typeof btoa !== 'undefined') {
    let s = '';
    u8.forEach((b) => (s += String.fromCharCode(b)));
    return btoa(s);
  }
  return Buffer.from(u8).toString('base64');
}

function fromB64(s: string): Uint8Array {
  if (typeof atob !== 'undefined') {
    const bin = atob(s);
    const out = new Uint8Array(bin.length);
    for (let i = 0; i < bin.length; i++) out[i] = bin.charCodeAt(i);
    return out;
  }
  return new Uint8Array(Buffer.from(s, 'base64'));
}

function encUtf8(s: string): Uint8Array {
  return new TextEncoder().encode(s);
}

export class Vault {
  private unlocked = false;
  private seed: Uint8Array | null = null;
  private accounts: string[] = [];

  async unlock(password: string): Promise<void> {
    const key = encUtf8(password || 'demo');
    // Si no hay seed persistida, crear una.
    const stored = await this._load();
    if (stored) {
      this.seed = xor(fromB64(stored), key);
    } else {
      this.seed = cryptoGetRandom(32);
      await this._save(toB64(xor(this.seed, key)));
    }
    this.accounts = [this._deriveAddress(this.seed)];
    this.unlocked = true;
  }

  async listAccounts(): Promise<string[]> {
    if (!this.unlocked) {
      // auto-unlock demo
      await this.unlock('demo');
    }
    return this.accounts;
  }

  async sign(payload: unknown): Promise<string> {
    if (!this.seed) await this.unlock('demo');
    const msg = encUtf8(JSON.stringify(payload ?? {}));
    const mixed = xor(msg, this.seed!);
    let h = 0;
    mixed.forEach((b) => (h = (h * 31 + b) >>> 0));
    return '0x' + h.toString(16).padStart(64, '0');
  }

  private _deriveAddress(seed: Uint8Array): string {
    let h = 0;
    seed.forEach((b) => (h = (h * 33 + b) >>> 0));
    return '0x' + h.toString(16).padStart(40, '0');
  }

  private async _save(v: string): Promise<void> {
    if (typeof chrome !== 'undefined' && chrome.storage?.local) {
      await chrome.storage.local.set({ vault: v });
    } else {
      (globalThis as any).__vault = v;
    }
  }

  private async _load(): Promise<string | null> {
    if (typeof chrome !== 'undefined' && chrome.storage?.local) {
      const r = await chrome.storage.local.get('vault');
      return r.vault || null;
    }
    return (globalThis as any).__vault || null;
  }
}

function cryptoGetRandom(n: number): Uint8Array {
  const out = new Uint8Array(n);
  if (typeof crypto !== 'undefined' && crypto.getRandomValues) {
    crypto.getRandomValues(out);
  } else {
    for (let i = 0; i < n; i++) out[i] = Math.floor(Math.random() * 256);
  }
  return out;
}
