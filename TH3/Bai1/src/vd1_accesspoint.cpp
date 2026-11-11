#include <WiFi.h>
const char* ssid = "ESP32";
const char* pass = "12345678";
void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, pass);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}
void loop() {}