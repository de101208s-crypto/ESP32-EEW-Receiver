# ESP32 EEW Receiver

ESP32とOLEDディスプレイを使用した、緊急地震速報（EEW）受信モニターです。

Wi-Fi経由でEEWデータを取得し、震源情報・最大予測震度・マグニチュードなどをOLEDへ表示します。

LEDとブザーによる警報通知、速報更新管理、PLUM法表示、最終報処理、キャンセル処理に対応しています。

※本機は公式の緊急地震速報端末ではありません。
防災判断には気象庁など公式情報をご利用ください。

---

# Features

## Main Features

- 緊急地震速報（EEW）受信
- Wi-Fiによる自動データ取得
- OLEDリアルタイム表示
- 最大予測震度表示
- マグニチュード表示
- 震源GSコード表示
- LED警報表示
- ブザー警報
- PLUM法対応
- FINAL報対応
- キャンセル報対応
- EEW更新管理
- 全国震源GSコード変換

---

# System Overview

この受信機は以下の流れで動作します。

Wolfx JMA EEW API
|
↓
ESP32 Wi-Fi通信
|
↓
JSONデータ解析
|
↓
EEW情報判定
|
↓
OLED表示
|
↓
LED・ブザー警報


---

# Hardware

## 使用部品

|部品|型番・製品名|用途|
|-|-|-|
|ESP32開発ボード|Freenove ESP32 Dev Board Kit|制御・Wi-Fi通信|
|OLEDディスプレイ|ARCELIS SSD1306 0.96インチ OLED|情報表示|
|LED|ELPA LED 5mm HK-LED5H|警報表示|
|ブザー|電磁パッシブブザー 16Ω|警報音出力|
|ブレッドボード|一般的なブレッドボード|試作|
|ジャンパーワイヤ|ジャンパーワイヤ|配線|
|USBケーブル|ESP32用USBケーブル|給電・書き込み|

---

# Wiring

## OLED（I2C）

OLEDはI2C通信で接続します。

|OLED|ESP32|
|-|-|
|VCC|3.3V|
|GND|GND|
|SDA|GPIO21|
|SCL|GPIO22|

---

## LED

|LED|ESP32|
|-|-|
|アノード（＋）|GPIO16|
|カソード（−）|GND|

※LEDには電流制限用抵抗を使用してください。

---

## Buzzer

|ブザー|ESP32|
|-|-|
|＋|GPIO17|
|−|GND|

---

# Assembly

## 組み立て手順

1. ESP32開発ボードをブレッドボードへ取り付けます。

2. OLEDをGPIO21・GPIO22へ接続します。

3. LEDをGPIO16へ接続します。

4. ブザーをGPIO17へ接続します。

5. 配線を確認します。

6. USBケーブルでESP32へ電源を供給します。

---

# Software

## 開発環境

以下の環境で開発できます。

- Arduino IDE
- Arduino Cloud

ESP32ボード環境をインストールしてください。

---

# Required Libraries

必要なライブラリ：


Adafruit GFX Library
Adafruit SSD1306
ArduinoJson


標準搭載ライブラリ：


WiFi
WiFiClientSecure
HTTPClient
Wire


---

# Setup

## Wi-Fi設定

プログラム内の以下を変更してください。
```cpp
const char* ssid = "SSID";
const char* password = "PASSWORD";
変更後、ESP32へ書き込みます。

Initial Boot

ESP32起動後、初期表示を行います。

表示：

BOOT

その後Wi-Fi接続を開始します。

Wi-Fi接続成功

表示：

WiFi OK

Waiting...

この状態でEEW情報を待機します。

Wi-Fi接続失敗

表示：

NO WIFI

Wi-Fiが復旧するまで再接続処理を行います。

Data Source

使用API：

https://api.wolfx.jp/jma_eew.json

取得したJSONデータを解析して表示します。

取得項目：

項目	用途
EventID	地震イベント識別
Serial	速報番号
Hypocenter	震源地
MaxIntensity	最大予測震度
Magnitude	マグニチュード
isWarn	警報判定
isFinal	最終報判定
isCancel	キャンセル判定
isAssumption	PLUM判定
# EEW Processing Details

本機は約1秒間隔でEEW APIを確認し、最新の緊急地震速報情報を取得します。

取得したJSONデータを解析し、必要な情報を抽出して表示・警報処理を行います。

---

# EEW Data Flow

EEW受信時の処理：


EEW JSON取得
|
↓
データ解析
|
↓
EventID確認
|
↓
Serial確認
|
↓
更新判定
|
↓
表示更新
|
↓
警報判定


---

# EventID and Serial Management

緊急地震速報は、1つの地震について複数回更新されます。

例：


EventID: 123456

Serial 1
↓
Serial 2
↓
Serial 3
↓
Final


本機では以下の変数を使用して管理します。

```cpp
lastEventID
lastSerial
EventID判定

EventIDが変化した場合：

新しい地震

として処理します。

例：

123456
↓
123457

新規EEWとして認識します。

Serial判定

同じ地震でも速報番号が増加した場合：

続報

として処理します。

例：

Serial 5
↓
Serial 6

情報更新として認識します。

EEW Display

EEWを受信するとOLEDへ情報を表示します。

表示例：

EEW!#25

6+
IWO     M6.9
Display Format
1行目

速報種類とSerial番号を表示します。

通常速報

表示：

EEW!#25

通常の緊急地震速報です。

PLUM情報

表示：

PLUM#25

PLUM法による情報の場合に表示します。

最終報

表示：

FINAL#25

最終的な解析結果の場合に表示します。

Intensity Display

気象庁の震度表記をOLED表示用に変換します。

気象庁表記	表示
5弱	5-
5強	5+
6弱	6-
6強	6+
表示できない場合

以下の場合：

データなし
不明
対象なし

表示：

?

になります。

Magnitude Display

マグニチュードを表示します。

例：

M6.9

取得できない場合：

M?

になります。

Warning System
警報条件

以下の条件で警報動作を行います。

updated && isWarn

条件：

新しいEEW情報
または速報更新
警報状態

の場合にLED・ブザーを動作させます。

Buzzer Operation

警報音は周波数スイープ方式です。

動作：

1000Hz
 ↓
2200Hz
 ↓
1000Hz

これを3回繰り返します。

LED Operation

使用GPIO：

GPIO16

状態：

状態	LED
通常待機	消灯
EEW警報	点灯
PLUM Processing

PLUM法による情報を検出した場合：

表示：

PLUM#番号

になります。

通常のEEW情報と区別することで、
どの方式による速報なのか確認できます。

FINAL Processing

最終報を受信した場合：

表示：

FINAL#番号

になります。

最終的な速報更新として扱います。

Cancel Processing

EEW発表後、取り消し情報を受信する場合があります。

キャンセル検出：

isCancel = true

の場合：

表示：

CANCEL
Cancel時の処理

実行内容：

ブザー停止
LED消灯
EventID初期化
Serial初期化

その後、次のEEW情報を待機します。

No Duplicate Alert

同じ速報を何度も警報しないように、
更新状態を確認しています。

管理：

lastEventID
lastSerial

により、

同じ地震
同じ速報番号

の重複処理を防止します。
# GS Code System

本機では震源名を3文字のGSコードへ変換して表示します。

OLEDは表示できる文字数に制限があるため、
長い震源名を短いコードへ変換することで、
限られた画面内でも確認しやすくしています。

---

# GS Code Processing

処理の流れ：


EEWから震源名取得
|
↓
GSコードテーブル検索
|
↓
3文字コード表示


---

例：


岩手県沖

↓

IWO


表示例：


6+

IWO M6.9


---

# GS Code Table

本機では全国の震源地域を登録しています。

例：

|震源名|GSコード|
|-|-|
|根室半島南東沖|NMU|
|北海道東方沖|HEO|
|釧路沖|KRO|
|十勝沖|TOK|
|浦河沖|URK|
|青森県東方沖|AOE|
|岩手県沖|IWO|
|三陸沖|SAN|
|宮城県沖|MYO|
|福島県沖|FKS|
|茨城県沖|IBR|
|東京湾|TYO|
|東京都23区|T23|
|大阪府北部|OSK|
|奈良県|NAR|
|兵庫県南東部|HSN|
|日向灘|HNG|
|沖縄本島近海|OKA|

---

登録されていない震源の場合：


???


を表示します。

---

# GS Code Management

GSコードはプログラム内のテーブルで管理しています。

形式：

```cpp
{
 "震源名",
 "コード"
}

例：

{
 "岩手県沖",
 "IWO"
}

新しい震源を追加する場合は、
このテーブルへ追加します。

Operation Example
通常待機
WiFi OK

Waiting...
EEW受信
EEW!#25

6+
IWO     M6.9

動作：

EEWデータ取得
新規情報判定
OLED更新
警報判定
LED・ブザー動作
続報受信

例：

EEW!#10
 ↓
EEW!#11
 ↓
FINAL#12

Serial番号の更新に合わせて表示を更新します。

キャンセル受信

表示：

CANCEL

動作：

警報停止
情報リセット
次のEEW待機
Current Build Status

現在の構成：

ESP32-WROOM系開発ボード
OLED表示
ブレッドボード試作
Wi-Fi常時接続
EEW自動受信

として動作しています。

Future Plans
Ver 1.1

予定：

津波情報表示
表示情報追加
警報機能改善
Ver 2.0

予定：

カラーLCD化
詳細地震情報表示
ケース製作
ユニバーサル基板化
Notes

本機は個人製作の緊急地震速報モニターです。

以下の理由により、
情報の遅延や取得失敗が発生する場合があります。

Wi-Fi通信状態
API提供状態
ESP32の動作状態

また、本機は防災用公式端末ではありません。

防災判断には気象庁など公式情報をご利用ください。

License

MIT License

Author

de101208s-crypto
