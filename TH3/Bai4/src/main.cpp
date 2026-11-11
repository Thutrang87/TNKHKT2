#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Sửa lại cho giống Bài 1
const char* WIFI_SSID = "VJU Office";
const char* WIFI_PASS = "VJuOffice@2023";

WebServer server(80);
const int LED = 2;
bool on = false;

// Tạo trang HTML hiển thị trạng thái + 2 nút BẬT / TẮT
String buildPage() {
  String h = "<!DOCTYPE html><html><head>";
  h += "<meta charset='UTF-8'>";
  h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  h += "<title>ESP32 LED</title></head>";
  h += "<body style='font-family:sans-serif;text-align:center'>";
  h += "<h1>ESP32 LED - Nhom 7</h1>";
  h += "<p style='font-size:28px'>Trang thai: <b style='color:";
  h += on ? "green'>DANG BAT" : "red'>DANG TAT";
  h += "</b></p>";
  h += "<a href='/on'><button style='font-size:24px;padding:12px 30px'>BAT</button></a> ";
  h += "<a href='/off'><button style='font-size:24px;padding:12px 30px'>TAT</button></a>";
  h += "</body></html>";
  return h;
}

void handleRoot() {
  server.send(200, "text/html", buildPage());
}

void handleOn() {
  on = true;
  digitalWrite(LED, HIGH);
  server.sendHeader("Location", "/");   // quay lại trang chính
  server.send(303);
}

void handleOff() {
  on = false;
  digitalWrite(LED, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Nối Wi-Fi (STA như Bài 1)
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

  // Đăng ký route
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("Web server da chay (cong 80)");
}

void loop() {
  server.handleClient();
}