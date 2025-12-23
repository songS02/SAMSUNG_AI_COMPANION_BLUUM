// led_color_test.ino - 메인 실행 파일

#include "control.h"

void setup() {
  Serial.begin(115200);
  Serial.println("LED 27초 시퀀스 시작");
  
  initNeoPixel();  // NeoPixel 초기화
}

void loop() {
  static bool done = false;
  
  if (!done) {
    unsigned long start = millis();
    
    // 27초 시퀀스 실행
    sequence27sec();
    
    unsigned long elapsed = millis() - start;
    Serial.print("실행 시간: ");
    Serial.print(elapsed);
    Serial.println("ms");
    
    // LED 끄기
    turnOffAllLED();
    
    done = true;
    Serial.println("완료");
  }
}

/*
타이밍:
- 30% → 70%: 2.7초 (27단계 × 100ms)
- 70% → 30%: 2.7초 (27단계 × 100ms)
- 1사이클: 5.4초
- 5사이클: 27초
*/