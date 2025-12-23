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

//================= 짧은 선형 페이드 함수 (0.2초용) =================
void linearFadeShort(int fromR, int fromG, int fromB,
                     int toR, int toG, int toB, int durationMs) {
  int steps = durationMs / 10;  // 10ms 단위
  
  for(int i = 0; i <= steps; i++) {
    // 선형 보간
    int currentR = fromR + ((toR - fromR) * i / steps);
    int currentG = fromG + ((toG - fromG) * i / steps);
    int currentB = fromB + ((toB - fromB) * i / steps);
    
    turnOnAllLED(currentR, currentG, currentB);
    
    if(i < steps) {
      delay(10);
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

//================= 개별 픽셀 페이드 함수 =================
void individualPixelFade(int red, int green, int blue, 
                         bool fadeIn, int durationMs) {
  int pixelDelay = durationMs / LED_COUNT;
  if(pixelDelay < 1) pixelDelay = 1;
  
  if(fadeIn) {
    strip.clear();
    strip.show();
    
    for(int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(red, green, blue));
      strip.show();
      delay(pixelDelay);
    }
  } else {
    for(int i = LED_COUNT - 1; i >= 0; i--) {
      strip.setPixelColor(i, 0);
      strip.show();
      delay(pixelDelay);
    }
  }
}

//================= 웨이브 페이드 함수 =================
void waveFade(int red, int green, int blue, 
              bool fadeIn, int durationMs) {
  int waveSize = 20;
  int steps = LED_COUNT / waveSize;
  int stepDelay = durationMs / steps;
  
  if(fadeIn) {
    strip.clear();
    for(int step = 0; step < steps; step++) {
      int startIdx = step * waveSize;
      int endIdx = min(startIdx + waveSize, LED_COUNT);
      
      for(int i = startIdx; i < endIdx; i++) {
        strip.setPixelColor(i, strip.Color(red, green, blue));
      }
      strip.show();
      delay(stepDelay);
    }
    for(int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(red, green, blue));
    }
    strip.show();
  } else {
    for(int step = 0; step < steps; step++) {
      int startIdx = LED_COUNT - ((step + 1) * waveSize);
      int endIdx = LED_COUNT - (step * waveSize);
      
      for(int i = max(startIdx, 0); i < endIdx; i++) {
        strip.setPixelColor(i, 0);
      }
      strip.show();
      delay(stepDelay);
    }
    strip.clear();
    strip.show();
  }
}

//================= 그라데이션 페이드 함수 =================
void gradientFade(int red, int green, int blue, 
                  bool fadeIn, int durationMs) {
  int steps = 100;
  int stepDelay = durationMs / steps;
  
  if(fadeIn) {
    for(int step = 0; step <= steps; step++) {
      float progress = (float)step / steps;
      int activePixels = (int)(LED_COUNT * progress);
      
      for(int i = 0; i < activePixels; i++) {
        strip.setPixelColor(i, strip.Color(red, green, blue));
      }
      
      int fadeZone = 10;
      for(int i = 0; i < fadeZone && (activePixels + i) < LED_COUNT; i++) {
        float fadeFactor = 1.0 - ((float)i / fadeZone);
        int fadedR = (int)(red * fadeFactor);
        int fadedG = (int)(green * fadeFactor);
        int fadedB = (int)(blue * fadeFactor);
        strip.setPixelColor(activePixels + i, strip.Color(fadedR, fadedG, fadedB));
      }
      
      strip.show();
      delay(stepDelay);
    }
  } else {
    for(int step = 0; step <= steps; step++) {
      float progress = (float)step / steps;
      int remainingPixels = (int)(LED_COUNT * (1.0 - progress));
      
      strip.clear();
      for(int i = 0; i < remainingPixels; i++) {
        strip.setPixelColor(i, strip.Color(red, green, blue));
      }
      
      int fadeZone = 10;
      for(int i = 0; i < fadeZone && (remainingPixels - fadeZone + i) >= 0; i++) {
        float fadeFactor = (float)i / fadeZone;
        int fadedR = (int)(red * fadeFactor);
        int fadedG = (int)(green * fadeFactor);
        int fadedB = (int)(blue * fadeFactor);
        int pixelIdx = remainingPixels - fadeZone + i;
        if(pixelIdx >= 0) {
          strip.setPixelColor(pixelIdx, strip.Color(fadedR, fadedG, fadedB));
        }
      }
      
      strip.show();
      delay(stepDelay);
    }
  }
}

//================= 새로운 20초 시퀀스 (OFF에서 시작) =================
void sequence20sec() {
  // 색상 정의
  int color70_R = 140, color70_G = 140, color70_B = 60;  // 70% 컬러
  int color100_R = 180, color100_G = 180, color100_B = 70; // 100% 컬러
  
  // === 0초: LED OFF ===
  strip.clear();
  strip.show();
  delay(1500);  // 1.5초 대기
  
  // === 1.5초-2초: 70% 색상 페이드인 (개별 LED) ===
  individualPixelFade(color70_R, color70_G, color70_B, true, 500);
  
  // === 2초-2.5초: 70% 유지 ===
  delay(500);
  
  // === 2.5초-3초: 70% 페이드아웃 (개별 LED) ===
  individualPixelFade(color70_R, color70_G, color70_B, false, 500);
  
  // === 3초-9.5초: OFF 유지 ===
  delay(2000);
  
  // === 9.5초-10초: 100% 색상 페이드인 (개별 LED) ===
  individualPixelFade(color100_R, color100_G, color100_B, true, 500);
  
  // === 10초-12초: 100% 유지 ===
  delay(2000);
  
  // === 12초-12.5초: 100% 페이드아웃 (개별 LED) ===
  individualPixelFade(color100_R, color100_G, color100_B, false, 500);
  
  // === 12.5초-20초: OFF 유지 ===
  delay(7500);
}

//================= 새로운 시퀀스2 (30% ↔ 100% 전환) =================
void sequence20sec_v2() {
  // 색상 정의
  int color30_R = 110, color30_G = 110, color30_B = 60;  // 30% 컬러
  int color100_R = 180, color100_G = 180, color100_B = 70; // 100% 컬러
  
  // === 0초-1.8초: 30% 유지 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);
  
  // === 1.8초-2초: 30% → 100% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color100_R, color100_G, color100_B, 200);
  
  // === 2초-2.5초: 100% 유지 (0.5초) ===
  turnOnAllLED(color100_R, color100_G, color100_B);
  delay(500);
  
  // === 2.5초-2.7초: 100% → 30% 전환 (0.2초) ===
  linearFadeShort(color100_R, color100_G, color100_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 2.7초-9.8초: 30% 유지 (7.1초) ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(7100);
  
  // === 9.8초-10초: 30% → 100% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color100_R, color100_G, color100_B, 200);
  
  // === 10초-12초: 100% 유지 (2초) ===
  turnOnAllLED(color100_R, color100_G, color100_B);
  delay(2000);
  
  // === 12초-12.2초: 100% → 30% 전환 (0.2초) ===
  linearFadeShort(color100_R, color100_G, color100_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 12.2초-20초: 30% 유지 (7.8초) ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(7800);
}