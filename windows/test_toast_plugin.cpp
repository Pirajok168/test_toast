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
std::string responseToast = "";
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

void AddRequestLogging(ComPtr<ICoreWebView2>& webview) {
    webview->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);

    EventRegistrationToken token; // Токен для управления событием
    webview->add_WebResourceRequested(
        Callback<ICoreWebView2WebResourceRequestedEventHandler>(
            [](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT {
                ComPtr<ICoreWebView2WebResourceRequest> request;
                if (SUCCEEDED(args->get_Request(&request))) {
                    LPWSTR uri = nullptr;
                    if (SUCCEEDED(request->get_Uri(&uri))) {
                        std::wstring requestUri(uri);
                        
                    
                        int size = WideCharToMultiByte(CP_UTF8, 0, requestUri.c_str(), -1, NULL, 0, NULL, NULL);
                        if (size > 0) {
                            std::string responseToastSizes(size, 0); // Создаем строку нужного размера
                            // Заполняем строку
                            WideCharToMultiByte(CP_UTF8, 0, requestUri.c_str(), -1, &responseToastSizes[0], size, NULL, NULL);
                            responseToast = responseToastSizes;
                        }

                        // Освобождаем строку, выделенную COM
                        CoTaskMemFree(uri);
                    }
                }
                return S_OK;
            })
            .Get(),
        &token); // Токен сохраняется здесь
}



std::string ShowToast(const std::wstring& htmlCode, int durationMs = 2000) {
    HWND hwnd = nullptr;
    ComPtr<ICoreWebView2Environment> webviewEnvironment;
    ComPtr<ICoreWebView2Controller> webviewController;
    ComPtr<ICoreWebView2> webview;

    const wchar_t CLASS_NAME[] = L"WebView2ToastWindow";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc; // Указываем оконную процедуру
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
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

    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);
    int x = desktopRect.right - 310; // 10px от правого края
    int y = 10;                      // 10px от верхнего края
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, 300, 100, SWP_SHOWWINDOW);

    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [&hwnd, &webviewController, &webview, &htmlCode](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    MessageBox(hwnd, L"Failed to create WebView2 environment", L"Error", MB_OK);
                    return result;
                }

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

                              // Добавляем обработчик запросов
                            AddRequestLogging(webview);     
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

                            webview->NavigateToString(htmlCode.c_str());

                            return S_OK;
                        }).Get());

                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        MessageBox(nullptr, L"Failed to initialize WebView2", L"Error", MB_OK);
        return "Failed to initialize WebView2";
    }

    SetTimer(hwnd, 1, durationMs, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return responseToast;
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
  if (method_call.method_name().compare("showToast") == 0) {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
        
    if (arguments) {
      auto html_entry = arguments->find(flutter::EncodableValue("htmlCode"));
    std::string html = std::get<std::string>(html_entry->second);  

     result->Success(flutter::EncodableValue(ShowToast(ConvertToUTF16(html), 5000)));
    }
  } else {
    result->NotImplemented();
  }
}

}  // namespace test_toast
