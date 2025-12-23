// control.cpp - LED 제어 함수 구현

#include "control.h"

//================= NeoPixel 객체 =================
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, PIXEL_TYPE);

//================= 초기화 함수 =================
void initNeoPixel() {
  strip.begin();                         // NeoPixel 스트립 초기화
  strip.show();                          // 모든 픽셀 끄기
  strip.setBrightness(DEFAULT_BRIGHTNESS); // 기본 밝기 설정
}

//================= 기본 제어 함수 =================

// 모든 LED 켜기 (512개 전체)
void turnOnAllLED(int red, int green, int blue) {
  for(int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}

// 모든 LED 끄기 (512개 전체)
void turnOffAllLED() {
  strip.clear();  
  strip.show();
}

//================= 페이드 효과 함수 (단순 선형) =================

// 2.7초 동안 선형으로 색상 변경
void linearFade(int fromR, int fromG, int fromB, 
                int toR, int toG, int toB) {
  
  int steps = 27;  // 27단계 (2.7초 / 0.1초 = 27)
  int stepDelay = 100;  // 100ms = 0.1초
  
  for(int i = 0; i <= steps; i++) {
    // 선형 보간 계산
    int currentR = fromR + ((toR - fromR) * i / steps);
    int currentG = fromG + ((toG - fromG) * i / steps);
    int currentB = fromB + ((toB - fromB) * i / steps);
    
    // 모든 LED에 적용
    turnOnAllLED(currentR, currentG, currentB);
    
    if(i < steps) {  // 마지막 단계에서는 delay 없음
      delay(stepDelay);
    }
  }
}

//================= 27초 시퀀스 =================
void sequence27sec() {
  // 색상 정의
  int color30_R = 110, color30_G = 110, color30_B = 60;  // 30% 컬러
  int color70_R = 140, color70_G = 140, color70_B = 60;  // 70% 컬러
  
  // 시작: 30% 색상으로 설정
  turnOnAllLED(color30_R, color30_G, color30_B);
  
  // 5사이클 실행
  for(int cycle = 0; cycle < 5; cycle++) {
    // 30% → 70% (2.7초)
    linearFade(color30_R, color30_G, color30_B,
               color70_R, color70_G, color70_B);
    
    // 70% → 30% (2.7초)  
    linearFade(color70_R, color70_G, color70_B,
               color30_R, color30_G, color30_B);
  }
}