#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Sửa lại cho giống các bài trước
const char* WIFI_SSID = "TP-Link_EFF8";
const char* WIFI_PASS = "98203574";

WebServer server(80);

// Số đo giả (dao động nhẹ quanh giá trị gốc)
float docNhiet() { return 27.0 + (random(-20, 21) / 10.0); }   // 25.0 - 29.0
float docAm()    { return 55.0 + (random(-50, 51) / 10.0); }   // 50.0 - 60.0

void handleSensor() {
  JsonDocument doc;
  doc["id"]     = "25112113";           
  doc["temp"]   = docNhiet();
  doc["hum"]    = docAm();
  doc["uptime"] = millis() / 1000;
  doc["rssi"]   = WiFi.RSSI();           // trường riêng: cường độ Wi-Fi (dBm)

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Dang ket noi Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP cua ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/api/sensor", HTTP_GET, handleSensor);
  server.begin();
  Serial.println("Web server chay, mo: http://<IP>/api/sensor");
}

void loop() {
  server.handleClient();
}