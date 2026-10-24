/// Intercambio de tokens.
import 'package:flutter/material.dart';

class SwapScreen extends StatefulWidget {
  const SwapScreen({super.key});
  @override
  State<SwapScreen> createState() => _SwapScreenState();
}

class _SwapScreenState extends State<SwapScreen> {
  final amt = TextEditingController(text: '1');
  String quote = '—';
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Swap')),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(children: [
          TextField(controller: amt, decoration: const InputDecoration(labelText: 'Cantidad')),
          ElevatedButton(
            onPressed: () {
              final v = double.tryParse(amt.text) ?? 0;
              setState(() => quote = (v * 0.997).toStringAsFixed(4));
            },
            child: const Text('Cotizar'),
          ),
          Text('Salida: $quote'),
        ]),
      ),
    );
  }
}
