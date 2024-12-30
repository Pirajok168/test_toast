# The Flutter tooling requires that developers have a version of Visual Studio
# installed that includes CMake 3.14 or later. You should not increase this
# version, as doing so will cause the plugin to fail to compile for some
# customers of the plugin.
cmake_minimum_required(VERSION 3.15)
include(FetchContent)

# Project-level configuration.
set(PROJECT_NAME "test_toast")
project(${PROJECT_NAME} LANGUAGES CXX)

# Explicitly opt in to modern CMake behaviors to avoid warnings with recent
# versions of CMake.
cmake_policy(VERSION 3.14...3.25)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "test_toast_plugin")

# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "test_toast_plugin.cpp"
  "test_toast_plugin.h"
)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  "include/test_toast/test_toast_plugin_c_api.h"
  "test_toast_plugin_c_api.cpp"
  ${PLUGIN_SOURCES}
)


# Microsoft.Web.WebView2
FetchContent_Declare(
  webview2
  URL  https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2903.40
)
FetchContent_Declare(
  wil
  URL  https://www.nuget.org/api/v2/package/Microsoft.Windows.ImplementationLibrary/1.0.240803.1
)
FetchContent_MakeAvailable(webview2 wil)

# Apply a standard set of build settings that are configured in the
# application-level CMakeLists.txt. This can be removed for plugins that want
# full control over build settings.
# apply_standard_settings(${PLUGIN_NAME})

# Symbols are hidden by default to reduce the chance of accidental conflicts
# between plugins. This should not be removed; any symbols that should be
# exported should be explicitly exported with the FLUTTER_PLUGIN_EXPORT macro.
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${webview2_SOURCE_DIR}/build/native/include/"
  "${wil_SOURCE_DIR}/include/"
  )

include_directories(SYSTEM "${wil_SOURCE_DIR}/include/")
include_directories(SYSTEM "${webview2_SOURCE_DIR}/build/native/include/")
link_libraries("${webview2_SOURCE_DIR}/build/native/${PLATFORM}/WebView2LoaderStatic.lib")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PLATFORM "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(PLATFORM "x86")
endif()
target_link_directories(${PLUGIN_NAME} PRIVATE "${webview2_SOURCE_DIR}/build/native/${PLATFORM}/WebView2LoaderStatic.lib")
target_link_libraries(
  ${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin
  "${webview2_SOURCE_DIR}/build/native/${PLATFORM}/WebView2LoaderStatic.lib"
  )


# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(test_toast_bundled_libraries
  ""
  PARENT_SCOPE
)
