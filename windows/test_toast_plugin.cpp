#include "test_toast_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <memory>
#include <sstream>

#include <wrl.h>
#include <WebView2.h>
#include <string>
#include <iostream>

namespace test_toast {
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

using namespace Microsoft::WRL;
using Microsoft::WRL::ComPtr;

// static
void TestToastPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "test_toast",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<TestToastPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

TestToastPlugin::TestToastPlugin() {}

TestToastPlugin::~TestToastPlugin() {}



std::string ShowToast(const std::wstring& htmlCode, int durationMs = 2000) {
    // Переменные для WebView2
    HWND hwnd = nullptr;
    ComPtr<ICoreWebView2Environment> webviewEnvironment;
    ComPtr<ICoreWebView2Controller> webviewController;
    ComPtr<ICoreWebView2> webview;

    // Создаем окно для WebView2
    const wchar_t CLASS_NAME[] = L"WebView2ToastWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc; // Указываем оконную процедуру
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        CLASS_NAME,
        L"Toast",
        WS_POPUP,
        0, 0, 300, 100,  // Размеры окна
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
        return "Failed to create window";
    }

    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_COLORKEY);

    // Позиция окна на экране
    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);
    int x = desktopRect.right - 310; // 10px от правого края
    int y = 10;                      // 10px от верхнего края
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, 300, 100, SWP_SHOWWINDOW);

    // Инициализация WebView2
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [&hwnd, &webviewController, &webview, &htmlCode](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    MessageBox(hwnd, L"Failed to create WebView2 environment", L"Error", MB_OK);
                    return result;
                }

                // Создаем контроллер WebView2
                env->CreateCoreWebView2Controller(
                    hwnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [&hwnd, &webviewController, &webview, &htmlCode](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                            if (FAILED(result)) {
                                MessageBox(hwnd, L"Failed to create WebView2 controller", L"Error", MB_OK);
                                return result;
                            }

                            webviewController = controller;
                            webviewController->get_CoreWebView2(&webview);


                            ComPtr<ICoreWebView2Controller2> webviewController2;
                            if (SUCCEEDED(webviewController.As(&webviewController2))) {
                                // Устанавливаем прозрачный фон
                                COREWEBVIEW2_COLOR transparentColor = {0, 0, 0, 0}; // RGBA
                                webviewController2->put_DefaultBackgroundColor(transparentColor);
                            }
                            // Настройка размеров WebView
                            RECT bounds;
                            GetClientRect(hwnd, &bounds);
                            webviewController->put_Bounds(bounds);

                            // Устанавливаем HTML содержимое
                            webview->NavigateToString(htmlCode.c_str());

                            return S_OK;
                        }).Get());

                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        MessageBox(nullptr, L"Failed to initialize WebView2", L"Error", MB_OK);
        return "Failed to initialize WebView2";
    }

    // Таймер для автоматического закрытия окна
    SetTimer(hwnd, 1, durationMs, nullptr);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return "Success";
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TIMER:
        KillTimer(hwnd, 1);
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_LBUTTONDOWN:
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
std::wstring ConvertToUTF16(const std::string& str) {
    if (str.empty()) {
        return {};
    }
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
    std::wstring wstr(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], sizeNeeded);
    return wstr;
}

void TestToastPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
        std::string htmlCode =
    "<!DOCTYPE html>"
    "<html lang=\"ru\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<style>"
    "body { font-family: Arial, sans-serif; }"
    "</style>"
    "<title>Тост</title>"
    "</head>"
    "<body>"
    "<p>Привет, мир! Это тест.</p>"
    "</body>"
    "</html>";
    
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
        
    if (arguments) {
      auto html_entry = arguments->find(flutter::EncodableValue("htmlCode"));
    std::string html = std::get<std::string>(html_entry->second);  
    ShowToast(ConvertToUTF16(html), 5000);
    }

    
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  } else {
    result->NotImplemented();
  }
}

}  // namespace test_toast
