/**
 * Puente entre la página y la extensión.
 * Encaja en: inyecta inpage.js y reenvía postMessage ↔ background.
 */
const s = document.createElement('script');
s.src = chrome.runtime.getURL('dist/inpage.js');
s.onload = () => s.remove();
(document.head || document.documentElement).appendChild(s);

window.addEventListener('message', (event) => {
  if (event.source !== window || !event.data || event.data.source !== 'l1-inpage') return;
  chrome.runtime.sendMessage(event.data.payload, (resp) => {
    window.postMessage({ source: 'l1-content', id: event.data.id, response: resp }, '*');
  });
});
