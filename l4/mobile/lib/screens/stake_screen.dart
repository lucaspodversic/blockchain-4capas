/// Staking.
import 'package:flutter/material.dart';

class StakeScreen extends StatefulWidget {
  const StakeScreen({super.key});
  @override
  State<StakeScreen> createState() => _StakeScreenState();
}

class _StakeScreenState extends State<StakeScreen> {
  final amt = TextEditingController(text: '10');
  String status = '';
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Staking')),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(children: [
          TextField(controller: amt, decoration: const InputDecoration(labelText: 'Monto')),
          ElevatedButton(onPressed: () => setState(() => status = 'staked ${amt.text}'), child: const Text('Stake')),
          Text(status),
        ]),
      ),
    );
  }
}
