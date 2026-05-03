# Public Network Monitoring Based on ESP32-CAM

> **选择你的语言/Choose your language/言語を選択してください/Choisissez votre langue**:
> - 中文（现在）
> - [English](README-EN.md)
> - [日本語](README-JP.md)
> - [Français](README-FR.md)

基于 ESP32-CAM 的公网监控，可用于远距离遥控车

## 配件
- [可以3D打印的配件](https://github.com/eric6227/3D-printing-accessories-for-ESP32-CAM)


## 简介

本项目是基于 Espressif Systems 的 CamerawebServer 开发的通过云服务器转发实现公网监控的项目，可以用于远程遥控车、农业机械等设备。使用小巧的 ESP32-CAM ，UDP 协议推流，可以做到轻量化、低延迟。未占用 ESP32-CAM 多余 GPIO，用户可在此基础上自由添加其他功能。受限于本人水平，本项目使用了 AI 辅助开发。

> **注意**：本项目使用未加密的传输协议，不建议用于需要保密或涉及隐私的场景。如果是家庭监控，可以直接使用 Espressif Systems 的 CamerawebServer ，在内网部署。

**请遵守当地法律，严禁用作非法用途。**

---

## 许可证

- 本项目整体采用 [MIT License](LICENSE)
- [app_httpd.cpp](app_httpd.cpp) 文件来源于 Espressif Systems，采用 [Apache License 2.0](LICENSE-APACHE-2.0)

---

## 部署方法

### ESP32-CAM 代码修改及上传

1. **克隆本项目**
   - 将文件夹名称改为 “Public-network-monitoring-based-on-ESP32-CAM”
   - 使用 Arduino 打开 `Public-network-monitoring-based-on-ESP32-CAM.ino` 文件。

2. **根据实际情况和注释修改代码**
   - 在 [board_config.h](board_config.h) 文件中取消注释你的开发板的型号，必须确保设置正确，否则有可能损坏开发板。
   - 第 97 行设置分辨率，默认是 QVGA ，320×240。
   - 第 155 行设置帧率，计算方法：`1000/设置值=帧数`，假如设置为 33（默认），则是大约 30 帧。
     - 使用 320x240 30fps 时，消耗的服务器带宽约为下行 1.1mbps，上行 1.8mbps。

3. **编译并上传**

---

### 服务端配置

1. **将 `server.py` 文件上传到服务器**

2. **安装 aiohttp，启动服务端**
   - 使用 pip 命令安装：
     ```bash
     pip install aiohttp
     python server.py
     ```
   - **建议使用 screen 确保退出登录后服务端持续运行**
     - 使用快捷键 `Ctrl+C` 结束进程
     - 创建一个 screen：
       ```bash
       screen
       ```
     - 启动服务端：
       ```bash
       python server.py
       ```
     - 退出 screen：
       使用快捷键 `Ctrl+A+D`（按住 Ctrl 的同时，先按 A，再按 D）

   - **如果安装不成功，提示以下信息**：
     ```
     × This environment is externally managed
     ╰─> To install Python packages system-wide, try apt install python3-xyz...
     ```
     - 确保安装了 venv 模块：
       ```bash
       sudo apt update
       sudo apt install python3-full python3-venv -y
       ```
     - 创建虚拟环境：
       ```bash
       python3 -m venv ~/my_aiohttp_env
       ```
     - 激活虚拟环境：
       ```bash
       source ~/my_aiohttp_env/bin/activate
       ```
     - 安装 aiohttp：
       ```bash
       pip install aiohttp
       ```
     - 获取虚拟环境路径：
       ```bash
       which python3
       ```
     - 创建 `run.sh` 文件：
       ```bash
       nano run.sh
       ```
       输入以下内容：
       ```bash
       #!/bin/bash
       PYTHON_PATH="/你的虚拟环境路径/"
       YOUR_SCRIPT="你的脚本文件名.py"
       cd "$(dirname "$0")"
       exec "$PYTHON_PATH" "$YOUR_SCRIPT"
       ```
     - 退出虚拟环境：
       ```bash
       deactivate
       ```
     - 启动服务端：
       ```bash
       sh run.sh
       ```

3. **开放端口 8888 和 8081**

---

## 连接

1. **测试连接**
   - 在浏览器中输入：
     ```
     http://[你的服务器的IP]:8081/stream/[你设置的camId]
     ```
   - 可以看见 ESP32-CAM 拍摄的画面。

2. **使用 motionEye 等软件连接**
   - 具体方法略。
