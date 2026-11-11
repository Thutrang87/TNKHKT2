// Bai 2 - Demo deep sleep - Vi du 1: bien GIU va bien MAT qua deep sleep
//  soLanThuc  : RTC_DATA_ATTR -> nam trong bo nho RTC (van duoc cap nguon luc ngu) -> GIU gia tri
//  demThuong  : RAM thuong    -> bi tat nguon luc ngu                              -> MAT, ve 0
// Chu trinh: thuc AWAKE_S giay (LED sang) -> ngu SLEEP_S giay (LED tat) -> thuc day, setup() chay lai.
#include <Arduino.h>
#include "esp_sleep.h"

#define LED_PIN 2

#ifndef SLEEP_S
#define SLEEP_S 10           // ngu bao lau (giay)
#endif
#ifndef AWAKE_S
#define AWAKE_S 5            // thuc bao lau truoc khi ngu (giay)
#endif

// GIU qua deep sleep (vung nho RTC):
RTC_DATA_ATTR int soLanThuc = 0;

// MAT qua deep sleep (RAM thuong):
int demThuong = 0;

void lamDeepSleep() {
  soLanThuc++;                       // RTC: cong don
  demThuong++;                       // thuong: se mat
  digitalWrite(LED_PIN, LOW);        // tat LED
  Serial.printf("Truoc khi ngu: soLanThuc=%d | demThuong=%d\n", soLanThuc, demThuong);
  Serial.printf("Ngu %d giay...\n\n", SLEEP_S);
  Serial.flush();                    // doi in xong roi moi ngu
  esp_sleep_enable_timer_wakeup(SLEEP_S * 1000000ULL);   // 10 giay (micro giay)
  esp_deep_sleep_start();            // lenh nay khong quay lai
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);       // LED sang khi dang thuc

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
    Serial.println("== Thuc day tu deep sleep ==");
  else
    Serial.println("== Khoi dong lan dau / reset / cap nguon ==");

  Serial.printf("Vua thuc: soLanThuc=%d (RTC, giu) | demThuong=%d (RAM, mat)\n", soLanThuc, demThuong);

  delay(AWAKE_S * 1000);             // thuc de quan sat
  lamDeepSleep();
}

void loop() {}                       // khong bao gio chay toi