# Linux GamePad Library

Linux でゲームコントローラの情報を取得する C++ ライブラリ

## 概要
 - 本ライブラリは，ゲームコントローラのデバイスファイル (`/dev/input/eventX`) からイベントを取得する
 - コントローラごとのヘッダファイルを提供する

## 特徴
 - ボタンの状態変化 (push / release) の取得
 - スティックの変化範囲の正規化 (range: -1.0f ~ 1.0f)

## requirements
 - C++ 対応コンパイラ (support C++14)
 - cmake
 - make

## インストール
`install.sh` を実行する

以下の処理を実行
 - ライブラリのビルド
 - ビルドしたライブラリのインストール
 - ライブラリキャッシュの更新
 - udev の `.rules` のインストール  
  
## 使い方
### サンプルコード
```cpp
// pad/ の各コントローラ用ライブラリを include
#include "pad/ps5pad.hpp"
#include <cstdio>

// コントローラに関連する処理は "pad" という名前空間に記述
// pad 以下に各コントローラの名前空間
// pad
// -- ps5
// -- procon 
using namespace pad;
using namespace std;

int main() {
  // テンプレートにコントローラに対応したハンドラの型を指定
  // デバイスファイルのパスを引数として初期化
  //   /dev/input/eventX でも可
  GamePad<ps5::PS5Handler> ps5(ps5::evdev_symlink_usb);

  if (!ps5.isConnected()) {
    printf("[ERROR] Failed to connect controller.\n");
    return 1;
  }

  while (ps5.isConnected()) {
    ps5.update(); // イベントの取得，情報を更新

    // 各ボタン・スティックのIDにより値や情報を取得する
    float x = ps5.axisValue(ps5::AxisID::leftX);
    float y = ps5.axisValue(ps5::AxisID::leftY);

    printf("x: %f y: %f\n", x, y);

    if (ps5.pushed(ps5::ButtonID::option))
      break;

    // 1ms ほどのスリープが望ましい
    usleep(1000);
  }

  return 0;
}

```

### コンパイル
```bash
g++ -o main main.cpp -lgamepad
```
`-lgamepad` のリンクオプションにより，共有ライブラリをリンクする必要がある