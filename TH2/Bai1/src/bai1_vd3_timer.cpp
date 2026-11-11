// vd3_timer_hw.cpp 

#include <Arduino.h>

#define LED1 2     
#define ADC  34    

#define TICK_MS     500    
#define ADC_TICKS   4       
#define HEART_TICKS 10      

hw_timer_t* hwTimer = nullptr;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

volatile bool     ledState = false;
volatile uint32_t adcPending = 0;     
volatile uint32_t heartPending = 0;   
uint32_t adcCnt = 0, heartCnt = 0;    

void IRAM_ATTR onTimer() {           
  portENTER_CRITICAL_ISR(&mux);
  ledState = !ledState;               
  digitalWrite(LED1, ledState);

  adcCnt = adcCnt + 1;
  if (adcCnt >= ADC_TICKS) {        
    adcCnt = 0;
    adcPending = adcPending + 1;
  }
  heartCnt = heartCnt + 1;
  if (heartCnt >= HEART_TICKS) {     
    heartCnt = 0;
    heartPending = heartPending + 1;
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void khoiTaoTimer() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  hwTimer = timerBegin(1000000);                           // 1 MHz -> 1 tick = 1 us
  timerAttachInterrupt(hwTimer, &onTimer);
  timerAlarm(hwTimer, TICK_MS * 1000ULL, true, 0);         // chu ky 500 ms, tu nap lai
#else
  hwTimer = timerBegin(0, 80, true);                       // timer0, chia 80 -> 1 tick = 1 us
  timerAttachInterrupt(hwTimer, &onTimer, true);
  timerAlarmWrite(hwTimer, TICK_MS * 1000ULL, true);
  timerAlarmEnable(hwTimer);
#endif
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  khoiTaoTimer();
}

void loop() {
  uint32_t a, h;
  portENTER_CRITICAL(&mux);           // lay va xoa co, tranh ISR chen giua
  a = adcPending;   adcPending = 0;
  h = heartPending; heartPending = 0;
  portEXIT_CRITICAL(&mux);

  for (; a > 0; a--) Serial.println(analogRead(ADC));   // doc ADC (ngoai ISR)
  for (; h > 0; h--) Serial.println("heartbeat");       // heartbeat (ngoai ISR)
}