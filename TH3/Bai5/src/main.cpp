#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Sửa lại cho giống Bài 1
const char* WIFI_SSID = "TP-Link_EFF8";
const char* WIFI_PASS = "98203574";

// Broker: cloud công cộng, hoặc đổi thành IP máy chạy Mosquitto trong LAN
const char* BROKER = "test.mosquitto.org";
const int   PORT   = 1883;

// (1) Topic theo tên nhóm + client id riêng cho kit này
const char* TOPIC_COUNT = "bcse/th3/nhom-07";
const char* TOPIC_LED   = "bcse/th3/nhom-07/led";
const char* CLIENT_ID   = "esp32-th3-nhom07-kitA";   // kit thứ 2 đổi thành ...kitB

const int LED = 2;

WiFiClient wifi;
PubSubClient mqtt(wifi);
int dem = 0;
unsigned long lastSend = 0;

// (2) Callback: nhận '1' bật LED, '0' tắt LED
void onMessage(char* topic, byte* payload, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  Serial.print("Nhan ["); Serial.print(topic); Serial.print("]: ");
  Serial.println(msg);

  if (msg == "1") {
    digitalWrite(LED, HIGH);
    Serial.println("-> LED BAT");
  } else if (msg == "0") {
    digitalWrite(LED, LOW);
    Serial.println("-> LED TAT");
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Dang ket noi Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Dang ket noi MQTT...");
    if (mqtt.connect(CLIENT_ID)) {
      Serial.println("OK");
      mqtt.subscribe(TOPIC_LED);          // đăng ký topic điều khiển
      Serial.print("Da subscribe: ");
      Serial.println(TOPIC_LED);
    } else {
      Serial.print("loi, rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  connectWiFi();
  mqtt.setServer(BROKER, PORT);
  mqtt.setCallback(onMessage);
  connectMQTT();
}

void loop() {
  if (!mqtt.connected()) connectMQTT();   // tự nối lại nếu rớt
  mqtt.loop();                            // gọi thường xuyên để nhận lệnh

  // Gửi số đếm mỗi 2 giây (không dùng delay để vẫn nhận lệnh kịp)
  if (millis() - lastSend >= 2000) {
    lastSend = millis();
    String s = String(dem++);
    mqtt.publish(TOPIC_COUNT, s.c_str());
    Serial.print("Publish: ");
    Serial.println(s);
  }
}