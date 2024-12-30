#include "include/test_toast/test_toast_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "test_toast_plugin.h"

void TestToastPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  test_toast::TestToastPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
