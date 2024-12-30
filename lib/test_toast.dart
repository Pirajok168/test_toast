
import 'test_toast_platform_interface.dart';

class TestToast {
  Future<String?> getPlatformVersion() {
    return TestToastPlatform.instance.getPlatformVersion();
  }
}
