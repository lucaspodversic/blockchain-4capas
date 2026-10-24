/// Ajustes (red, idioma, seguridad).
import 'package:flutter/material.dart';
import '../core/biometrics.dart';

class SettingsScreen extends StatefulWidget {
  const SettingsScreen({super.key});
  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  String network = 'Devnet';
  String lang = 'es';
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Ajustes')),
      body: ListView(children: [
        ListTile(
          title: const Text('Red'),
          subtitle: Text(network),
          onTap: () => setState(() => network = network == 'Devnet' ? 'L1 Mainnet' : 'Devnet'),
        ),
        ListTile(
          title: const Text('Idioma'),
          subtitle: Text(lang),
          onTap: () => setState(() => lang = lang == 'es' ? 'en' : 'es'),
        ),
        ListTile(
          title: const Text('Biometría'),
          onTap: () async {
            final ok = await Biometrics.authenticate();
            if (context.mounted) {
              ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(ok ? 'OK' : 'Fail')));
            }
          },
        ),
      ]),
    );
  }
}
