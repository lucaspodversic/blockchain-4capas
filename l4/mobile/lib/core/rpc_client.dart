/// Cliente JSON-RPC hacia el nodo L1/L2.
import 'dart:convert';
import 'package:http/http.dart' as http;

class RpcClient {
  RpcClient(this.baseUrl);
  final String baseUrl;
  int _id = 1;

  Future<dynamic> call(String method, [List<dynamic> params = const []]) async {
    try {
      final res = await http.post(
        Uri.parse(baseUrl),
        headers: {'content-type': 'application/json'},
        body: jsonEncode({'jsonrpc': '2.0', 'id': _id++, 'method': method, 'params': params}),
      ).timeout(const Duration(seconds: 3));
      final body = jsonDecode(res.body) as Map<String, dynamic>;
      if (body.containsKey('error')) throw Exception(body['error']);
      return body['result'];
    } catch (_) {
      // Mock offline para demos / tests sin nodo.
      if (method == 'eth_blockNumber') return '0x80';
      if (method == 'eth_getBalance') return '0x3635c9adc5dea00000';
      return null;
    }
  }
}
