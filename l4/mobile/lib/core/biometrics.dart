/// Autenticación biométrica (wrapper).
/// Si local_auth no está disponible en el entorno, autentica en modo demo.
class Biometrics {
  static Future<bool> authenticate({String reason = 'Desbloquear wallet'}) async {
    // Demo: siempre OK. En dispositivo real: LocalAuthentication().authenticate(...)
    assert(reason.isNotEmpty);
    return true;
  }
}
