#include <Arduino.h>
#include "esp_camera.h"
#include <driver/gpio.h>
#include <WiFi.h>
#include <WiFiUdp.h>  

// ========== 你的摄像头型号头文件 ==========
#include "board_config.h"
//在board_config.h文件中取消注释你的esp32开发板型号，默认AI Thinker。选择错误可能损坏开发板
//分辨率在 line 97 修改。默认 QVGA 320×240 帧率在 line 155 修改

// ========== WiFi 信息 ==========
const char *ssid = "你的WiFi的ssid";
const char *password = "你的WiFi密码";

//设置camId，必须唯一
const char *camId = "设置唯一的camId，尽量不用中文";

// ========== 云服务器信息 ==========
const char *server_ip = "你的云服务器IP";
const uint16_t server_port = 8888;  // UDP 端口，必须与服务器上设置的端口一致，防火墙及安全组打开此端口，协议为 UDP

// UDP 对象
WiFiUDP udp;

// 分片大小（小于 MTU 1500，给包头留余量）
#define CHUNK_SIZE 1200

// UDP 分片发送函数
void sendFrameUDP(camera_fb_t *fb) {
  uint8_t *buf = fb->buf;
  size_t len = fb->len;

  // 总分片数
  uint8_t totalChunks = (len + CHUNK_SIZE - 1) / CHUNK_SIZE;
  if (totalChunks == 0) return;

  // 简单帧序号（用于服务器端重组，短时间内够用）
  static uint16_t frameSeq = 0;
  frameSeq++;

  for (uint8_t i = 0; i < totalChunks; i++) {
    size_t offset = i * CHUNK_SIZE;
    size_t chunkLen = (i == totalChunks - 1) ? (len - offset) : CHUNK_SIZE;

    udp.beginPacket(server_ip, server_port);

    // 1. 写入 camId 长度（1字节）+ camId 字符串
    uint8_t idLen = strlen(camId);
    udp.write(&idLen, 1);
    udp.write((const uint8_t *)camId, idLen);

    // 2. 写入帧序号（2字节大端）
    udp.write((frameSeq >> 8) & 0xFF);
    udp.write(frameSeq & 0xFF);

    // 3. 写入总分片数（1字节）
    udp.write(totalChunks);

    // 4. 写入当前分片索引（1字节，从0开始）
    udp.write(i);

    // 5. 写入分片数据
    udp.write(buf + offset, chunkLen);

    udp.endPacket();
    delay(2);  // 微小延时，防止连续发送太快丢包
  }
}

// ========== 初始化部分 ==========
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // ----- 摄像头配置 -----
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;          //在此处设置分辨率
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("摄像头初始化失败: 0x%x", err);
    return;
  }

  // 可选的翻转/镜像
  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  // s->set_hmirror(s, 1);

  // ----- WiFi 连接 -----
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("WiFi连接中");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi已连接，IP: " + WiFi.localIP().toString());

  gpio_reset_pin(GPIO_NUM_4);
  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_4, 0);
  
  Serial.println("系统初始化完成，开始 UDP 推流...");
}

// ========== 主循环 ==========
void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("抓帧失败");
    delay(100);
    return;
  }

  sendFrameUDP(fb);

  esp_camera_fb_return(fb);

  // 控制帧率，VGA 建议 10-15fps，这里 33ms ≈ 30fps，可适当调大到 50~80
  delay(33);
}