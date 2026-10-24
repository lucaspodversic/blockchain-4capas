/// Enviar fondos.
import 'package:flutter/material.dart';
import '../core/wallet_service.dart';
import '../widgets/qr_scanner.dart';

class SendScreen extends StatefulWidget {
  const SendScreen({super.key});
  @override
  State<SendScreen> createState() => _SendScreenState();
}

class _SendScreenState extends State<SendScreen> {
  final toCtrl = TextEditingController();
  final amtCtrl = TextEditingController();
  String? sig;

  Future<void> _send() async {
    final payload = '${toCtrl.text}:${amtCtrl.text}';
    final s = await WalletService.instance.sign(payload);
    setState(() => sig = s);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Enviar')),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(children: [
          TextField(controller: toCtrl, decoration: const InputDecoration(labelText: 'Destino')),
          TextField(controller: amtCtrl, decoration: const InputDecoration(labelText: 'Monto'), keyboardType: TextInputType.number),
          const SizedBox(height: 8),
          QrScanner(onScanned: (v) => setState(() => toCtrl.text = v)),
          ElevatedButton(onPressed: _send, child: const Text('Firmar y enviar')),
          if (sig != null) Text('Sig: $sig', style: const TextStyle(fontSize: 10)),
        ]),
      ),
    );
  }
}
