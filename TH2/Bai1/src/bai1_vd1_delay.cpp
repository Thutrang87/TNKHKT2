// vd1_dung_delay.cpp - 3 viec goi delay() noi tiep nhau
#include <Arduino.h>

#define LED1 2     
#define ADC  34    

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
}

void loop() {
  digitalWrite(LED1, HIGH); delay(500);
  digitalWrite(LED1, LOW);  delay(500);

  int tho = analogRead(ADC);
  Serial.println(tho);
  delay(2000);

  Serial.println("heartbeat");
  delay(5000);
}
