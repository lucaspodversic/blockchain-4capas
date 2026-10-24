/// Recibir (QR + dirección).
import 'package:flutter/material.dart';
import '../core/wallet_service.dart';

class ReceiveScreen extends StatelessWidget {
  const ReceiveScreen({super.key});
  @override
  Widget build(BuildContext context) {
    final addr = WalletService.instance.address ?? '';
    return Scaffold(
      appBar: AppBar(title: const Text('Recibir')),
      body: Center(
        child: Column(mainAxisSize: MainAxisSize.min, children: [
          Container(width: 180, height: 180, color: Colors.grey.shade200, child: const Center(child: Text('QR'))),
          const SizedBox(height: 16),
          SelectableText(addr, style: const TextStyle(fontFamily: 'monospace')),
        ]),
      ),
    );
  }
}
