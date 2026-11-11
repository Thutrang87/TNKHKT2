// vd2_non_blocking.cpp - 3 viec chay dung chu ky bang millis(), KHONG co delay()

#include <Arduino.h>

#define LED1 2     
#define ADC  34    

const unsigned long CK_LED   = 500;    // nhay LED1 moi 500 ms
const unsigned long CK_ADC   = 2000;   // doc bien tro moi 2000 ms
const unsigned long CK_HEART = 5000;   // in heartbeat moi 5000 ms

unsigned long lanTruocLed = 0, lanTruocAdc = 0, lanTruocHeart = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  if (now - lanTruocLed >= CK_LED) {
    lanTruocLed += CK_LED;               
    ledState = !ledState;               
    digitalWrite(LED1, ledState);
  }
  if (now - lanTruocAdc >= CK_ADC) {
    lanTruocAdc += CK_ADC;
    Serial.println(analogRead(ADC));     // doc ADC
  }
  if (now - lanTruocHeart >= CK_HEART) {
    lanTruocHeart += CK_HEART;
    Serial.println("heartbeat");         // heartbeat
  }
}