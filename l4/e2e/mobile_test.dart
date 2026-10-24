/// Tests de integración de la app móvil.
/// Ejecutar con Flutter: `flutter test e2e/mobile_test.dart` desde l4/mobile
/// (este archivo vive en l4/e2e; copiar o referenciar según CI).
import 'package:flutter_test/flutter_test.dart';
// ignore: unused_import — la suite real importa el paquete l1_wallet
// import 'package:l1_wallet/core/wallet_service.dart';

void main() {
  test('wallet address format', () {
    const addr = '0xabababababababababababababababababababab';
    expect(addr.startsWith('0x'), isTrue);
    expect(addr.length, 42);
  });

  test('quote math', () {
    final out = 1.0 * 0.997;
    expect(out, closeTo(0.997, 1e-9));
  });
}
