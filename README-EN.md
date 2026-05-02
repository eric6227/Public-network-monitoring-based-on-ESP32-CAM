# Public Network Monitoring Based on ESP32-CAM

> **选择你的语言/Choose your language/言語を選択してください/Choisissez votre langue**:
> - [中文](README.md)
> - English(now)
> - [日本語](README-JP.md)
> - [Français](README-FR.md)

> **Note**: This document is machine-translated. For the original Chinese version, see [README.md](README.md).

Public network monitoring based on ESP32-CAM, suitable for long-distance remote control vehicles.

---

## Introduction

This project is based on Espressif Systems' CameraWebServer and implements public network monitoring through cloud server forwarding. It can be used for remote control vehicles, agricultural machinery, and other equipment. Using the compact ESP32-CAM and UDP protocol streaming, it achieves lightweight and low latency. Unused ESP32 pins allow for further development of additional features. Due to my limited skills, this project was developed with AI assistance.

**Please comply with local laws and strictly prohibit illegal use.**

---

## License

- The project as a whole adopts the [MIT License](LICENSE).
- The file [app_httpd.cpp](app_httpd.cpp) originates from Espressif Systems and adopts the [Apache License 2.0](LICENSE-APACHE-2.0).

---

## Deployment Instructions

### ESP32-CAM Code Modification and Upload

1. **Clone the project**
   - Rename the folder to "Public-network-monitoring-based-on-ESP32-CAM".
   - Open the `Public-network-monitoring-based-on-ESP32-CAM.ino` file using Arduino.

2. **Modify the code according to your situation and comments**
   - Uncomment your development board model in the [board_config.h](board_config.h) file. Ensure the settings are correct to avoid damaging the board.
   - Line 97 sets the resolution, default is QVGA (320×240).
   - Line 151 sets the frame rate. Calculation: `1000 / value = FPS`. For example, if set to 33 (default), it is approximately 30 FPS.
     - Using 320x240 at 30fps consumes about 1.1mbps downstream and 1.8mbps upstream bandwidth on the server.

3. **Compile and upload**

---

### Server Configuration

1. **Upload the `server.py` file to the server**

2. **Install aiohttp and start the server**
   - Install using pip:
     ```bash
     pip install aiohttp
     python server.py
     ```
   - **It is recommended to use screen to ensure the server continues running after logging out**
     - End the process with `Ctrl+C`.
     - Create a screen session:
       ```bash
       screen
       ```
     - Start the server:
       ```bash
       python server.py
       ```
     - Detach the screen session:
       Use the shortcut `Ctrl+A+D` (hold Ctrl, press A, then D).

   - **If installation fails with the following message**:
     ```
     × This environment is externally managed
     ╰─> To install Python packages system-wide, try apt install python3-xyz...
     ```
     - Ensure the venv module is installed:
       ```bash
       sudo apt update
       sudo apt install python3-full python3-venv -y
       ```
     - Create a virtual environment:
       ```bash
       python3 -m venv ~/my_aiohttp_env
       ```
     - Activate the virtual environment:
       ```bash
       source ~/my_aiohttp_env/bin/activate
       ```
     - Install aiohttp:
       ```bash
       pip install aiohttp
       ```
     - Get the virtual environment path:
       ```bash
       which python3
       ```
     - Create a `run.sh` file:
       ```bash
       nano run.sh
       ```
       Enter the following content:
       ```bash
       #!/bin/bash
       PYTHON_PATH="/path/to/your/virtual/environment/bin/python3"
       YOUR_SCRIPT="your_script_name.py"
       cd "$(dirname "$0")"
       exec "$PYTHON_PATH" "$YOUR_SCRIPT"
       ```
     - Deactivate the virtual environment:
       ```bash
       deactivate
       ```
     - Start the server:
       ```bash
       sh run.sh
       ```

3. **Open ports 8888 and 8081**

---

## Connection

1. **Test the connection**
   - Enter the following in your browser:
     ```
     http://[your-server-IP]:8081/stream/[your-camId]
     ```
   - You should see the video captured by the ESP32-CAM.

2. **Connect using software like motionEye**
   - Specific methods are omitted here.