# ESP32-CAM に基づくパブリックネットワーク監視

> **选择你的语言/Choose your language/言語を選択してください/Choisissez votre langue**:
> - [中文](README.md)
> - [English](README-EN.md)
> - 日本語(今)
> - [Français](README-FR.md)

> **注意**: このドキュメントは機械翻訳されています。元の中国語版は [README.md](README.md) を参照してください。

ESP32-CAM に基づくパブリックネットワーク監視、長距離リモートコントロール車両に適しています。

---

## 概要

このプロジェクトは、Espressif Systems の CameraWebServer に基づいており、クラウドサーバー転送を通じてパブリックネットワーク監視を実現します。遠隔操作車両、農業機械などの機器に使用できます。コンパクトな ESP32-CAM と UDP プロトコルストリーミングを使用して、軽量で低遅延を実現します。未使用の ESP32 ピンを使用して、追加機能のさらなる開発が可能です。私のスキルが限られているため、このプロジェクトは AI の支援を受けて開発されました。

> **注意**: このプロジェクトは暗号化されていない伝送プロトコルを使用しており、機密性が必要なシナリオやプライバシーに関わるシナリオには推奨されません。家庭用監視には、Espressif Systems の CameraWebServer を直接使用してローカルネットワーク内で展開できます。

**現地の法律を遵守し、違法な使用を厳禁してください。**

---

## ライセンス

- プロジェクト全体は [MIT License](LICENSE) を採用しています。
- ファイル [app_httpd.cpp](./esp32/app_httpd.cpp) は Espressif Systems に由来し、[Apache License 2.0](./esp32/LICENSE-APACHE-2.0) を採用しています。

---

## デプロイ手順

### ESP32-CAM コードの変更とアップロード

1. **プロジェクトをクローンする**
   - フォルダー名を "Public-network-monitoring-based-on-ESP32-CAM" に変更します。
   - Arduino を使用して `Public-network-monitoring-based-on-ESP32-CAM.ino` ファイルを開きます。

2. **状況とコメントに応じてコードを変更する**
   - [board_config.h](board_config.h) ファイルで、開発ボードのモデルのコメントを解除します。設定が正しいことを確認してください。そうしないと、ボードが損傷する可能性があります。
   - 97 行目で解像度を設定します。デフォルトは QVGA (320×240) です。
   - 155 行目でフレームレートを設定します。計算方法：`1000 / 値 = FPS`。たとえば、33（デフォルト）に設定すると、約 30 FPS になります。
     - 320x240 30fps を使用すると、サーバーの帯域幅消費量は下り約 1.1mbps、上り約 1.8mbps です。

3. **コンパイルしてアップロードする**

---

### サーバー構成

#### 方法1：伝統的なデプロイ方法
1. **`server.py` ファイルをサーバーにアップロードする**

2. **aiohttp をインストールしてサーバーを起動する**
   - pip を使用してインストール：
     ```bash
     pip install aiohttp
     python server.py
     ```
   - **ログアウト後もサーバーが継続して実行されるように screen を使用することをお勧めします**
     - `Ctrl+C` でプロセスを終了します。
     - screen セッションを作成：
       ```bash
       screen
       ```
     - サーバーを起動：
       ```bash
       python server.py
       ```
     - screen セッションをデタッチ：
       ショートカット `Ctrl+A+D` を使用します（Ctrl を押しながら A を押し、その後 D を押します）。

   - **次のメッセージが表示されてインストールに失敗した場合**：
     ```
     × This environment is externally managed
     ╰─> To install Python packages system-wide, try apt install python3-xyz...
     ```
     - venv モジュールがインストールされていることを確認：
       ```bash
       sudo apt update
       sudo apt install python3-full python3-venv -y
       ```
     - 仮想環境を作成：
       ```bash
       python3 -m venv ~/my_aiohttp_env
       ```
     - 仮想環境を有効化：
       ```bash
       source ~/my_aiohttp_env/bin/activate
       ```
     - aiohttp をインストール：
       ```bash
       pip install aiohttp
       ```
     - 仮想環境のパスを取得：
       ```bash
       which python3
       ```
     - `run.sh` ファイルを作成：
       ```bash
       nano run.sh
       ```
       次の内容を入力：
       ```bash
       #!/bin/bash
       PYTHON_PATH="/仮想環境のパス/"
       YOUR_SCRIPT="スクリプト名.py"
       cd "$(dirname "$0")"
       exec "$PYTHON_PATH" "$YOUR_SCRIPT"
       ```
     - 仮想環境を無効化：
       ```bash
       deactivate
       ```
     - サーバーを起動：
       ```bash
       sh run.sh
       ```

3. **ポート 8888 と 8081 を開放する**

#### 方法2：Docker を使用したデプロイ
1. **Docker Compose を使用する**
   - `docker-compose.yml` ファイルを作成：
     ```yaml
     version: '3.8'
     
     services:
       esp32-cam-server:
         image: ghcr.io/eric6227/pnm-esp32cam:latest
         container_name: esp32-cam-server
         ports:
           - "8888:8888/udp"
           - "8081:8081/tcp"
         volumes:
           - ./logs:/app/logs
         restart: unless-stopped
         environment:
           - PYTHONUNBUFFERED=1
     ```
   - サービスを起動：
     ```bash
     docker-compose up -d
     ```
   - ログを表示：
     ```bash
     docker-compose logs -f
     ```
   - サービスを停止：
     ```bash
     docker-compose down
     ```

2. **手動で Docker を使用したデプロイ**
   - イメージをプル：
     ```bash
     docker pull ghcr.io/eric6227/pnm-esp32cam:latest
     ```
   - コンテナを起動：
     ```bash
     docker run -d \
       --name esp32-cam-server \
       -p 8888:8888/udp \
       -p 8081:8081/tcp \
       -v ./logs:/app/logs \
       --restart unless-stopped \
       ghcr.io/eric6227/pnm-esp32cam:latest
     ```

---

## 接続

1. **接続をテストする**
   - ブラウザに次を入力：
     ```
     http://[サーバーのIP]:8081/stream/[カメラID]
     ```
   - ESP32-CAM が撮影した映像が表示されます。

2. **motionEye などのソフトウェアを使用して接続する**
   - 詳細な方法は省略します。