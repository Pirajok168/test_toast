import 'package:flutter_test/flutter_test.dart';
import 'package:test_toast/test_toast.dart';
import 'package:test_toast/test_toast_platform_interface.dart';
import 'package:test_toast/test_toast_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockTestToastPlatform
    with MockPlatformInterfaceMixin
    implements TestToastPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final TestToastPlatform initialPlatform = TestToastPlatform.instance;

  test('$MethodChannelTestToast is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelTestToast>());
  });

  test('getPlatformVersion', () async {
    TestToast testToastPlugin = TestToast();
    MockTestToastPlatform fakePlatform = MockTestToastPlatform();
    TestToastPlatform.instance = fakePlatform;

    expect(await testToastPlugin.getPlatformVersion(), '42');
  });
}
