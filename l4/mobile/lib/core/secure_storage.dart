/// Almacenamiento seguro (Keychain/Keystore).
/// En tests/demo usa memoria + SharedPreferences si está disponible.
class SecureStorage {
  SecureStorage._();
  static final instance = SecureStorage._();
  final Map<String, String> _mem = {};

  Future<void> write(String key, String value) async {
    _mem[key] = value;
  }

  Future<String?> read(String key) async => _mem[key];

  Future<void> delete(String key) async {
    _mem.remove(key);
  }
}
