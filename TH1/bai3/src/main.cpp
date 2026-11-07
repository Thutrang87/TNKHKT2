// Bai 3 - Demo: doc tho -> loc trung binh truot -> LEDC
#include <Arduino.h>

#ifndef ADC_PIN
#define ADC_PIN 34     // 34 = ADC1_CH6 (input-only). ADC2: vi du GPIO 25
#endif
#ifndef USE_FILTER
#define USE_FILTER 1   // 0 = chi dung gia tri tho, 1 = dung loc
#endif
#ifndef TELEPLOT
#define TELEPLOT 0     // 1 = in theo dinh dang Teleplot (">ten:gia_tri")
#endif
#ifndef USE_WIFI
#define USE_WIFI 0     // 1 = bat WiFi de quan sat xung dot ADC2
#endif

#define LED 18
#define N   8          // so mau trung binh truot

#if USE_WIFI
#include <WiFi.h>
#endif

int buf[N];
int idx = 0;

// Tuong thich Arduino-ESP32 core 2.x va core 3.x
static void ledSetup() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED, 5000, 8);
#else
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED, 0);
#endif
}

static void ledWrite(int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(LED, duty);
#else
  ledcWrite(0, duty);
#endif
}

int loc(int m) {
  buf[idx] = m;
  idx = (idx + 1) % N;
  long t = 0;
  for (int i = 0; i < N; i++) t += buf[i];
  return t / N;
}

void setup() {
  Serial.begin(115200);
  ledSetup();
#if USE_WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin("Wokwi-GUEST", "");   // ket noi WiFi -> ADC2 bi xung dot
#endif
  // Do day bo dem bang mau dau tien de loc khong bi lech ve 0 luc khoi dong
  int first = analogRead(ADC_PIN);
  for (int i = 0; i < N; i++) buf[i] = first;
  Serial.printf("ADC_PIN=%d USE_FILTER=%d USE_WIFI=%d\n", ADC_PIN, USE_FILTER, USE_WIFI);
}

void loop() {
  int tho = analogRead(ADC_PIN);
  int l = loc(tho);
#if USE_FILTER
  int dung = l;
#else
  int dung = tho;
#endif
  ledWrite(map(dung, 0, 4095, 0, 255));
#if TELEPLOT
  Serial.print(">tho:"); Serial.println(tho);   // dinh dang Teleplot
  Serial.print(">loc:"); Serial.println(l);
#else
  // "ten:gia_tri,ten:gia_tri" -> Serial Plotter ve 2 duong cung luc
  Serial.print("Tho:"); Serial.print(tho);
  Serial.print(",");
  Serial.print("Loc:"); Serial.println(l);
#endif
  delay(20);
}