/// Creación/importación de wallet.
import 'package:flutter/material.dart';
import '../core/wallet_service.dart';
import 'home_screen.dart';

class OnboardingScreen extends StatefulWidget {
  const OnboardingScreen({super.key});
  @override
  State<OnboardingScreen> createState() => _OnboardingScreenState();
}

class _OnboardingScreenState extends State<OnboardingScreen> {
  final _seedCtrl = TextEditingController();

  Future<void> _create() async {
    await WalletService.instance.create();
    if (!mounted) return;
    Navigator.of(context).pushReplacement(MaterialPageRoute(builder: (_) => const HomeScreen()));
  }

  Future<void> _import() async {
    await WalletService.instance.importSeed(_seedCtrl.text);
    if (!mounted) return;
    Navigator.of(context).pushReplacement(MaterialPageRoute(builder: (_) => const HomeScreen()));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Bienvenido')),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            ElevatedButton(onPressed: _create, child: const Text('Crear wallet')),
            const SizedBox(height: 16),
            TextField(controller: _seedCtrl, decoration: const InputDecoration(labelText: 'Seed / frase')),
            ElevatedButton(onPressed: _import, child: const Text('Importar')),
          ],
        ),
      ),
    );
  }
}
