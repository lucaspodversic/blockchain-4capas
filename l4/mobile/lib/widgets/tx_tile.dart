/// Elemento de lista de transacción.
import 'package:flutter/material.dart';

class TxTile extends StatelessWidget {
  const TxTile({super.key, required this.hash, required this.subtitle, required this.status});
  final String hash;
  final String subtitle;
  final String status;

  @override
  Widget build(BuildContext context) {
    return ListTile(
      leading: Icon(status == 'ok' ? Icons.check_circle : Icons.hourglass_top),
      title: Text(hash, style: const TextStyle(fontFamily: 'monospace', fontSize: 12)),
      subtitle: Text(subtitle),
      trailing: Text(status),
    );
  }
}
