/// Balances y actividad.
import 'package:flutter/material.dart';
import '../core/rpc_client.dart';
import '../core/wallet_service.dart';
import '../widgets/tx_tile.dart';
import 'send_screen.dart';
import 'receive_screen.dart';
import 'settings_screen.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});
  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  String balance = '…';
  final rpc = RpcClient('http://127.0.0.1:8545');

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load() async {
    final a = WalletService.instance.address ?? '0x0';
    final b = await rpc.call('eth_getBalance', [a, 'latest']);
    setState(() => balance = b?.toString() ?? '0');
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('L1 Wallet'),
        actions: [
          IconButton(icon: const Icon(Icons.settings), onPressed: () {
            Navigator.push(context, MaterialPageRoute(builder: (_) => const SettingsScreen()));
          }),
        ],
      ),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          Text(WalletService.instance.address ?? '', style: const TextStyle(fontFamily: 'monospace', fontSize: 12)),
          const SizedBox(height: 8),
          Text('Balance: $balance', style: Theme.of(context).textTheme.headlineSmall),
          const SizedBox(height: 16),
          Row(children: [
            ElevatedButton(onPressed: () => Navigator.push(context, MaterialPageRoute(builder: (_) => const SendScreen())), child: const Text('Enviar')),
            const SizedBox(width: 8),
            OutlinedButton(onPressed: () => Navigator.push(context, MaterialPageRoute(builder: (_) => const ReceiveScreen())), child: const Text('Recibir')),
          ]),
          const Divider(),
          const TxTile(hash: '0xabc…', subtitle: 'Enviado 5 L1', status: 'ok'),
        ],
      ),
    );
  }
}
