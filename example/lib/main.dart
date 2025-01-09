import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:test_toast/test_toast.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  final _testToastPlugin = TestToast();

  @override
  void initState() {
    super.initState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: ElevatedButton(
              onPressed: () async {
                final imageUrl =
                    'https://i.pinimg.com/736x/a2/3e/41/a23e41ed4ddb358f0c3876adfe69388b.jpg';
                final response = await http.get(Uri.parse(imageUrl));
                if (response.statusCode == 200) {
                  final bytes = response.bodyBytes;
                  final base64String = base64Encode(bytes);

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
            overflow: hidden;
        }
        .notification {
            display: flex;
            align-items: center;
            background-color: #3a3a3a;
            padding: 10px 15px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            width: 400px;
            position: relative;
            transform: translateX(100%); /* Начальная позиция справа */
            animation: slide-in 0.5s forwards; /* Анимация выдвижения */
        }

        @keyframes slide-in {
            0% {
                transform: translateX(100%); /* Начальная позиция за пределами экрана */
            }
            100% {
                transform: translateX(0); /* Конечная позиция в видимой области */
            }
        }
        .notification:hover {
            background-color: rgba(100, 250, 200, 1); /* Изменение фона при наведении */
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
        }
        .notification .text {
            display: flex;
            flex-direction: column;
            cursor: default; /* Устанавливаем курсор по умолчанию */
            user-select: none; /* Убираем возможность выделения текста */
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

</head>
<body>
    <div class="notification">
        <div class="image-wrapper">
            <img src="data:image/jpg;base64,$base64String" alt="Изображение">
             
        </div>
        <div class="text">
            <div class="name">Данила</div>
            <div class="message">сейчас играет в<br><span class="highlight">ДОТАААА</span></div>
        </div>
    </div>
</div>
</body>
</html>
  """;

                  _platformVersion =
                      await _testToastPlugin.getPlatformVersion(html) ?? "";

                  setState(() {});
                }
              },
              child: Text(_platformVersion)),
        ),
      ),
    );
  }
}
