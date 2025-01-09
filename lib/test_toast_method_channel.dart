import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'test_toast_platform_interface.dart';

/// An implementation of [TestToastPlatform] that uses method channels.
class MethodChannelTestToast extends TestToastPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('test_toast');

  @override
  Future<String?> getPlatformVersion(String htmlToast) async {
    final version = await methodChannel
        .invokeMethod<String>('showToast', {'htmlCode': htmlToast});
    return version;
  }
}
