// Bai 1 - Vi du 1: LED fading, KHONG ngat 
#include <Arduino.h>

#define BUTTON_PIN 4
#define LED_PIN 2

static void ledSetup() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED_PIN, 5000, 8);        // pin, tan so 5kHz, 8 bit
#else
  ledcSetup(0, 5000, 8);               // kenh 0, 5kHz, 8 bit
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

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  ledSetup();
}

void loop() {
  for (int b = 0; b <= 255; b++) {
    ledWrite(b);
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("button pressed");
    }
    delay(10);   // moi buoc fade mat 10 ms
  }
  for (int b = 255; b >= 0; b--) {
    ledWrite(b);
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("button pressed");
    }
    delay(10);
  }
}