#include <WiFi.h>

const char* ssid = "TP-Link_EFF8";   
const char* pass = "98203574";    

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  Serial.print("Dang ket noi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Da vao mang!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {}