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
    Serial.println("\n=== 20초 시퀀스 v2 시작 (30% ↔ 100%) ===");
    unsigned long start = millis();
    
    // 20초 시퀀스 v2 실행 (30%에서 시작)
    sequence20sec_v2();
    
    unsigned long elapsed = millis() - start;
    Serial.print("실행 시간: ");
    Serial.print(elapsed);
    Serial.println("ms");
    
    // LED 끄기
    turnOffAllLED();
    
    done = true;
    Serial.println("완료 - LED OFF");
    
    // 다른 시퀀스를 테스트하려면 아래 주석 해제
    // sequence20sec();    // OFF에서 시작하는 버전
    // sequence27sec();    // 27초 버전
  }
}

/*
타이밍:
- 30% → 70%: 2.7초 (27단계 × 100ms)
- 70% → 30%: 2.7초 (27단계 × 100ms)
- 1사이클: 5.4초
- 5사이클: 27초

=== 20초 새로운 시퀀스 타임라인 ===
0초: OFF
1.8초-2초: OFF → 70% 페이드인 (0.2초)
2초-2.5초: 70% 유지
2.5초-2.7초: 70% → OFF 페이드아웃 (0.2초)
2.7초-8초: OFF 유지
8초-9.8초: OFF 유지
9.8초-10초: OFF → 100% 페이드인 (0.2초)
10초-12초: 100% 유지
12초-12.2초: 100% → OFF 페이드아웃 (0.2초)
12.2초-20초: OFF 유지

=== 색상 정의 ===
30% 컬러: RGB(110, 110, 60)
70% 컬러: RGB(140, 140, 60)
100% 컬러: RGB(180, 180, 70)
*/