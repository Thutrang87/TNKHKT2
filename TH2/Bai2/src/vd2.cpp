// Bai 2 - Demo deep sleep - Vi du 2: thuc day chay lai tu setup()

#include <Arduino.h>
#include "esp_sleep.h"

#define LED_PIN 2

#ifndef USE_WEB
#define USE_WEB 1            // 1 = Wi-Fi AP + web, 0 = chi in Serial (dung cho Wokwi)
#endif
#ifndef SLEEP_S
#define SLEEP_S 10           // ngu bao lau (giay)
#endif
#ifndef AWAKE_S
#define AWAKE_S 5           // thuc bao lau truoc khi tu ngu (giay), du de kip xem web
#endif

#if USE_WEB
#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);
#endif

// GIU qua deep sleep (vung nho RTC):
RTC_DATA_ATTR int soLanThuc = 0;            // so lan da thuc day (cong don)
RTC_DATA_ATTR int demThuongTruocNgu = 0;    // chi de doi chieu: demThuong luc truoc khi ngu

// MAT qua deep sleep (RAM thuong):
int demThuong = 0;
bool ledState = false;

const char* tenNguyenNhan() {
  switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_TIMER:     return "timer (het thoi gian ngu)";
    case ESP_SLEEP_WAKEUP_UNDEFINED: return "cap nguon / reset (khong phai tu deep sleep)";
    default:                         return "nguyen nhan khac";
  }
}

void lamDeepSleep() {
  soLanThuc++;                       // RTC: cong don
  demThuong++;                       // thuong: se mat
  demThuongTruocNgu = demThuong;     // RTC: ghi lai de sau khi thuc doi chieu voi demThuong (se la 0)
  digitalWrite(LED_PIN, LOW);        // tat LED
  Serial.printf("[ngu] soLanThuc=%d demThuong=%d -> ngu %d giay...\n", soLanThuc, demThuong, SLEEP_S);
  Serial.flush();
  esp_sleep_enable_timer_wakeup(SLEEP_S * 1000000ULL);   // 10 giay (tinh bang micro giay)
  esp_deep_sleep_start();            // lenh nay khong quay lai
}

#if USE_WEB
void trangChu() {
  static char buf[1800];
  snprintf(buf, sizeof buf, R"HTML(<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta http-equiv="refresh" content="1"><title>Deep sleep</title>
<style>body{font-family:sans-serif;max-width:430px;margin:20px auto;padding:0 12px}
table{border-collapse:collapse;width:100%%}td,th{border:1px solid #bbb;padding:8px;text-align:left}
.keep{color:#0a7a3d;font-weight:bold}.lost{color:#c0392b;font-weight:bold}
a.b{display:inline-block;margin-top:14px;padding:10px 16px;background:#1f4e78;color:#fff;text-decoration:none;border-radius:6px}</style></head><body>
<h2>Deep sleep: biến giữ / biến mất</h2>
<table><tr><th>Biến</th><th>Giá trị</th><th>Lưu ở</th></tr>
<tr><td>soLanThuc</td><td class="keep">%d</td><td>RTC: GIỮ</td></tr>
<tr><td>demThuong</td><td class="lost">%d</td><td>RAM thường: MẤT</td></tr>
<tr><td>demThuong lúc trước khi ngủ</td><td>%d</td><td>RTC (để đối chiếu)</td></tr></table>
<p>Nguyên nhân thức: <b>%s</b></p>
<p>Đã thức %lu s, tự ngủ sau %lu s (ngủ %d s).</p>
<a class="b" href="/sleep">Ngủ ngay</a></body></html>)HTML",
           soLanThuc, demThuong, demThuongTruocNgu, tenNguyenNhan(),
           (unsigned long)(millis() / 1000),
           (unsigned long)(millis() >= AWAKE_S * 1000UL ? 0 : AWAKE_S - millis() / 1000),
           SLEEP_S);
  server.send(200, "text/html; charset=utf-8", buf);
}

void trangNgu() {
  server.send(200, "text/html; charset=utf-8",
              "<meta charset=\"utf-8\"><p>Đang ngủ... Kết nối lại Wi-Fi sau vài giây.</p>");
  delay(300);                        // cho trinh duyet nhan xong phan hoi
  lamDeepSleep();
}
#endif

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  delay(300);

  // nguyen nhan thuc (timer / cap nguon):
  Serial.printf("== %s ==\n", tenNguyenNhan());
  Serial.printf("Vua thuc: soLanThuc=%d (RTC, giu) | demThuong=%d (RAM, mat) | demThuong truoc khi ngu=%d\n",
                soLanThuc, demThuong, demThuongTruocNgu);

#if USE_WEB
  // khoi tao Wi-Fi AP + web
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-DeepSleep");
  server.on("/", trangChu);
  server.on("/sleep", trangNgu);
  server.begin();
  Serial.printf("Web: ket noi Wi-Fi 'ESP32-DeepSleep' roi mo http://%s\n",
                WiFi.softAPIP().toString().c_str());
#endif
  Serial.printf("Thuc %d giay roi tu ngu %d giay.\n", AWAKE_S, SLEEP_S);
}

void loop() {
#if USE_WEB
  server.handleClient();
#endif

  // LED nhap nhay bao CPU con chay (khong dung delay)
  static unsigned long tLed = 0;
  if (millis() - tLed >= 250) {
    tLed = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  // het thoi gian thuc -> ngu
  if (millis() >= AWAKE_S * 1000UL) lamDeepSleep();
}
