#ifndef FLUTTER_PLUGIN_TEST_TOAST_PLUGIN_H_
#define FLUTTER_PLUGIN_TEST_TOAST_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <memory>

namespace test_toast {

class TestToastPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  TestToastPlugin();

  virtual ~TestToastPlugin();

  // Disallow copy and assign.
  TestToastPlugin(const TestToastPlugin&) = delete;
  TestToastPlugin& operator=(const TestToastPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace test_toast

#endif  // FLUTTER_PLUGIN_TEST_TOAST_PLUGIN_H_
