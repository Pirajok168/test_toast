import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'test_toast_platform_interface.dart';

/// An implementation of [TestToastPlatform] that uses method channels.
class MethodChannelTestToast extends TestToastPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('test_toast');

  @override
  Future<String?> getPlatformVersion() async {
    String html = """
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Привет, мир!</title>
    <style>
        body {
            margin: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: transparent;
            color: black;
            font-family: Arial, sans-serif;
        }
        .notification {
            display: flex;
            align-items: center;
            background-color: #3a3a3a;
            padding: 10px 15px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            width: 400px;
        }

        .notification .image-wrapper {
            position: relative;
            width: 50px;
            height: 50px;
            margin-right: 15px;
        }

        .notification img {
            width: 100%;
            height: 100%;
            border-radius: 5px;
            display: none; /* Скрыть изображение до загрузки */
        }

        .loader {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            border: 3px solid #ccc;
            border-top: 3px solid #555;
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }

        @keyframes spin {
            0% {
                transform: rotate(0deg);
            }
            100% {
                transform: rotate(360deg);
            }
        }

        .notification .text {
            display: flex;
            flex-direction: column;
        }

        .notification .text .name {
            font-size: 1.2rem;
            font-weight: bold;
            color: #a4ff00;
            margin-bottom: 5px;
        }

        .notification .text .message {
            font-size: 1rem;
            line-height: 1.4;
        }

        .notification .text .message .highlight {
            color: #a4ff00;
        }
    </style>

    <script>
        document.addEventListener("DOMContentLoaded", function () {
            const img = document.querySelector(".notification img");
            const loader = document.querySelector(".loader");

            img.onload = function () {
                loader.style.display = "none";
                img.style.display = "block";
            };

            img.onerror = function () {
                loader.style.borderTopColor = "#f00"; /* Изменение цвета лоадера при ошибке */
            };
        });
    </script>

</head>
<body>
    <div class="notification">
        <div class="image-wrapper">
            <div class="loader"></div>
            <img src="https://placehold.co/50" alt="Placeholder">
        </div>
        <div class="text">
            <div class="name">Данила</div>
            <div class="message">сейчас играет в<br><span class="highlight">Очке своими пальчиками</span></div>
        </div>
    </div>
</div>
</body>
</html>
  """;

    final version = await methodChannel
        .invokeMethod<String>('getPlatformVersion', {'htmlCode': html});
    return version;
  }
}
