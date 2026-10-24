/// Escáner QR (stub UI; en prod usar mobile_scanner / qr_code_scanner).
import 'package:flutter/material.dart';

class QrScanner extends StatelessWidget {
  const QrScanner({super.key, required this.onScanned});
  final ValueChanged<String> onScanned;

  @override
  Widget build(BuildContext context) {
    return TextButton(
      onPressed: () => onScanned('0x' + 'cd' * 20),
      child: const Text('Escanear QR (demo)'),
    );
  }
}
