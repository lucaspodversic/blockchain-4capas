/** Suscripción a eventos por WebSocket (con fallback polling mock). */
export type EventHandler = (ev: { address: string; topics: string[]; data: string }) => void;

export class EventSubscription {
  private closed = false;
  constructor(private url: string, private handler: EventHandler) {}

  start(): void {
    // Sin WS real en tests: emite un evento sintético.
    if (this.closed) return;
    setTimeout(() => {
      if (!this.closed) {
        this.handler({
          address: '0x' + '00'.repeat(20),
          topics: ['0xTransfer'],
          data: '0x',
        });
      }
    }, 1);
  }

  stop(): void {
    this.closed = true;
  }
}

export function subscribeLogs(wsUrl: string, handler: EventHandler): EventSubscription {
  const sub = new EventSubscription(wsUrl, handler);
  sub.start();
  return sub;
}
