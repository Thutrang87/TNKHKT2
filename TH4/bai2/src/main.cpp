#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Sửa lại cho giống các bài trước
const char* WIFI_SSID = "TP-Link_EFF8";
const char* WIFI_PASS = "98203574";

// Yêu cầu 2: broker lớp dùng (đổi nếu lớp dùng broker khác / IP máy Mosquitto trong LAN)
const char* BROKER = "test.mosquitto.org";
const int   PORT   = 1883;

// Yêu cầu 1: namespace riêng theo nhóm/SV
const char* SV_ID = "nhom-07";            // <-- đổi theo mã nhóm/SV của bạn
String TOPIC_TELE;                        // aet2022/<SV_ID>/telemetry
String CLIENT_ID;                         // ghép MAC để mỗi kit khác nhau

WiFiClient wifi;
PubSubClient mqtt(wifi);
unsigned long lan = 0;

float docNhiet() { return 27.0 + (random(-20, 21) / 10.0); }
float docAm()    { return 55.0 + (random(-50, 51) / 10.0); }

void noiWiFi() {
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

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.print("Dang ket noi MQTT...");
    if (mqtt.connect(CLIENT_ID.c_str())) {
      Serial.println("OK");
    } else {
      Serial.print("loi, rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  noiWiFi();

  // Dựng topic và client id (client id có MAC nên không trùng giữa các kit)
  TOPIC_TELE = String("aet2025/") + SV_ID + "/telemetry";
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  CLIENT_ID = String("esp32-") + SV_ID + "-" + mac;
  Serial.print("Topic: ");     Serial.println(TOPIC_TELE);
  Serial.print("Client ID: "); Serial.println(CLIENT_ID);

  mqtt.setServer(BROKER, PORT);
  noiBroker();
}

void loop() {
  if (!mqtt.connected()) noiBroker();
  mqtt.loop();                                   // gọi thường xuyên

  if (millis() - lan >= 2000) {
    lan = millis();

    JsonDocument doc;
    doc["temp"]   = docNhiet();
    doc["hum"]    = docAm();
    doc["uptime"] = millis() / 1000;
    doc["rssi"]   = WiFi.RSSI();                 // yêu cầu 3: trường riêng

    char b[128];
    serializeJson(doc, b);
    mqtt.publish(TOPIC_TELE.c_str(), b);

    Serial.print("Publish: ");
    Serial.println(b);
  }
}