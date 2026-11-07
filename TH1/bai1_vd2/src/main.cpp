// Bai 1 - Vi du 2: LED fading, CO ngat 
#include <Arduino.h>

#define BUTTON_PIN 4
#define LED_PIN 2

volatile bool buttonPressed = false;
static void ledSetup() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED_PIN, 5000, 8);
#else
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);
#endif
}

static void ledWrite(int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(LED_PIN, duty);
#else
  ledcWrite(0, duty);
#endif
}

void IRAM_ATTR handleInterrupt() {
  buttonPressed = true;   // ISR ngan gon, chi dat co
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  ledSetup();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),handleInterrupt, FALLING);
}

void loop() {
  static int brightness = 0, step = 1;
  if (buttonPressed) {
    buttonPressed = false;
    ledWrite(0);   delay(80);   // nhay tat nhanh
    ledWrite(255); delay(80);   // nhay sang nhanh
  }
  brightness += step;
  if (brightness <= 0 || brightness >= 255) step = -step;
  ledWrite(brightness);
  delay(10);
}