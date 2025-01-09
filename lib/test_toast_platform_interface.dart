import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'test_toast_method_channel.dart';

abstract class TestToastPlatform extends PlatformInterface {
  /// Constructs a TestToastPlatform.
  TestToastPlatform() : super(token: _token);

  static final Object _token = Object();

  static TestToastPlatform _instance = MethodChannelTestToast();

  /// The default instance of [TestToastPlatform] to use.
  ///
  /// Defaults to [MethodChannelTestToast].
  static TestToastPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [TestToastPlatform] when
  /// they register themselves.
  static set instance(TestToastPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion(String htmlToast) {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
