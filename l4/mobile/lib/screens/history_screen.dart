/// Historial de transacciones.
import 'package:flutter/material.dart';
import '../widgets/tx_tile.dart';

class HistoryScreen extends StatelessWidget {
  const HistoryScreen({super.key});
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Historial')),
      body: ListView(children: const [
        TxTile(hash: '0x111…', subtitle: 'Enviado 5 L1', status: 'ok'),
        TxTile(hash: '0x222…', subtitle: 'Recibido 2 L1', status: 'ok'),
        TxTile(hash: '0x333…', subtitle: 'Swap', status: 'pending'),
      ]),
    );
  }
}
