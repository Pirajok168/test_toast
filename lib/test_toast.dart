import 'test_toast_platform_interface.dart';

class TestToast {
  Future<String?> getPlatformVersion(String htmlToast) {
    return TestToastPlatform.instance.getPlatformVersion(htmlToast);
  }
}
