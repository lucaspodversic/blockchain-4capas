/// Punto de entrada de la app móvil L4.
/// Encaja en: navega a onboarding o home según haya wallet.
import 'package:flutter/material.dart';
import 'screens/home_screen.dart';
import 'screens/onboarding_screen.dart';
import 'core/wallet_service.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const L1WalletApp());
}

class L1WalletApp extends StatelessWidget {
  const L1WalletApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'L1 Wallet',
      theme: ThemeData(colorSchemeSeed: const Color(0xFF3B82F6), useMaterial3: true),
      home: FutureBuilder<bool>(
        future: WalletService.instance.hasWallet(),
        builder: (ctx, snap) {
          if (!snap.hasData) return const Scaffold(body: Center(child: CircularProgressIndicator()));
          return snap.data! ? const HomeScreen() : const OnboardingScreen();
        },
      ),
    );
  }
}
