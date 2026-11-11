#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "TP-Link_EFF8";
const char* WIFI_PASS = "98203574";

const char* BROKER = "test.mosquitto.org";   // phải GIỐNG broker trong Node-RED
const int   PORT   = 1883;

const char* NAMESPACE = "aet2025";
const char* SV_ID     = "nhom-07";

const int LED = 14;                          // đề yêu cầu GPIO14

String TOPIC_TELE, TOPIC_CMD, CLIENT_ID;
WiFiClient wifi;
PubSubClient mqtt(wifi);
unsigned long lan = 0;
bool ledOn = false;

float docNhiet() { return 27.0 + (random(-20, 21) / 10.0); }
float docAm()    { return 55.0 + (random(-50, 51) / 10.0); }

// Chiều VỀ: nhận lệnh từ Node-RED
void onMessage(char* topic, byte* payload, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  msg.trim();
  msg.toUpperCase();
  Serial.print("Nhan ["); Serial.print(topic); Serial.print("]: "); Serial.println(msg);

  if (msg == "1" || msg == "ON")       { ledOn = true;  digitalWrite(LED, HIGH); Serial.println("-> LED BAT"); }
  else if (msg == "0" || msg == "OFF") { ledOn = false; digitalWrite(LED, LOW);  Serial.println("-> LED TAT"); }
}

void noiWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Dang ket noi Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println();
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.print("Dang ket noi MQTT...");
    if (mqtt.connect(CLIENT_ID.c_str())) {
      Serial.println("OK");
      mqtt.subscribe(TOPIC_CMD.c_str());
      Serial.print("Da subscribe: "); Serial.println(TOPIC_CMD);
    } else {
      Serial.print("loi, rc="); Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  noiWiFi();

  TOPIC_TELE = String(NAMESPACE) + "/" + SV_ID + "/telemetry";
  TOPIC_CMD  = String(NAMESPACE) + "/" + SV_ID + "/cmd/led";
  String mac = WiFi.macAddress(); mac.replace(":", "");
  CLIENT_ID = String("esp32-") + SV_ID + "-" + mac;

  Serial.print("Telemetry: "); Serial.println(TOPIC_TELE);
  Serial.print("Lenh LED : "); Serial.println(TOPIC_CMD);

  mqtt.setServer(BROKER, PORT);
  mqtt.setCallback(onMessage);
  noiBroker();
}

void loop() {
  if (!mqtt.connected()) noiBroker();
  mqtt.loop();

  // Chiều ĐI: publish mỗi 2 giây
  if (millis() - lan >= 2000) {
    lan = millis();
    JsonDocument doc;
    doc["temp"]   = docNhiet();
    doc["hum"]    = docAm();
    doc["uptime"] = millis() / 1000;
    doc["rssi"]   = WiFi.RSSI();
    doc["led"]    = ledOn ? 1 : 0;

    char b[160];
    serializeJson(doc, b);
    mqtt.publish(TOPIC_TELE.c_str(), b);
    Serial.print("Publish: "); Serial.println(b);
  }
}