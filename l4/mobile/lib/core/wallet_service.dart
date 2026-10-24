/// Generación de claves y firma (simplificado / demo).
/// Producción: usar secp256k1 + HD wallet.
import 'dart:convert';
import 'dart:math';
import 'secure_storage.dart';

class WalletService {
  WalletService._();
  static final instance = WalletService._();

  String? address;

  Future<bool> hasWallet() async {
    final a = await SecureStorage.instance.read('address');
    address = a;
    return a != null && a.isNotEmpty;
  }

  Future<String> create() async {
    final rnd = Random.secure();
    final bytes = List<int>.generate(20, (_) => rnd.nextInt(256));
    address = '0x' + bytes.map((b) => b.toRadixString(16).padLeft(2, '0')).join();
    await SecureStorage.instance.write('address', address!);
    await SecureStorage.instance.write('seed', base64Encode(List<int>.generate(32, (_) => rnd.nextInt(256))));
    return address!;
  }

  Future<String> importSeed(String seed) async {
    // Derivación mock: hash del seed → dirección.
    var h = 0;
    for (final c in seed.codeUnits) {
      h = (h * 31 + c) & 0xffffffff;
    }
    address = '0x' + h.toRadixString(16).padLeft(40, '0');
    await SecureStorage.instance.write('address', address!);
    await SecureStorage.instance.write('seed', seed);
    return address!;
  }

  Future<String> sign(String payload) async {
    final seed = await SecureStorage.instance.read('seed') ?? '';
    var h = 0;
    for (final c in (seed + payload).codeUnits) {
      h = (h * 33 + c) & 0xffffffff;
    }
    return '0x' + h.toRadixString(16).padLeft(64, '0');
  }
}
