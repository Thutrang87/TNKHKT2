// Bai 2 - Demo Debounce: chong doi bang millis() ngay trong ISR
#include <Arduino.h>

#define BUTTON_PIN 4    
#define LED_PIN    2    

#ifndef DEBOUNCE_MS     // duoc dat boi build_flags trong platformio.ini
#define DEBOUNCE_MS 50  // Cua so chong doi: bo qua suon xuong trong 50ms sau suon dau
#endif

volatile uint32_t debouncedCount = 0;        // So lan nhan nut THUC SU (da chong doi)
volatile uint32_t rawEdgeCount = 0;          // So suon xuong THO (de doi chieu, chua chong doi)
volatile unsigned long lastInterruptTime = 0; // Moc thoi gian (ms) cua suon xuong hop le gan nhat

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// ----------------- ISR: CO CHONG DOI BANG millis() -----------------
void IRAM_ATTR handleInterrupt() {
  rawEdgeCount++;   // Ghi nhan moi suon xuong tho

  unsigned long now = millis();
  if (now - lastInterruptTime >= DEBOUNCE_MS) {
    // Da qua DEBOUNCE_MS ke tu suon hop le truoc do -> day la lan nhan MOI
    debouncedCount++;
    lastInterruptTime = now;
  }
  // Nguoc lai: suon nay roi vao "cua so chong doi" -> bo qua, khong dem
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, FALLING);
  Serial.printf("Debounce demo, DEBOUNCE_MS = %d\n", DEBOUNCE_MS);
}

void loop() {
  static uint32_t lastShown = 0;
  static bool ledState = false;

  // Doc 2 bien volatile 32-bit trong vung gang de khong bi ngat chen giua
  portENTER_CRITICAL(&mux);
  uint32_t raw = rawEdgeCount;
  uint32_t deb = debouncedCount;
  portEXIT_CRITICAL(&mux);

  if (deb != lastShown) {          // co lan nhan moi da loc
    lastShown = deb;
    ledState = !ledState;          // dao LED moi lan nhan thuc
    digitalWrite(LED_PIN, ledState);
    Serial.printf("rawEdgeCount=%u debouncedCount=%u\n", raw, deb);
  }
  delay(5);
}