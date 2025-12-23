// control.cpp - LED 제어 함수 구현 (트래킹 모션 추가)

#include "control.h"
#include <math.h>

//================= NeoPixel 객체 =================
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, PIXEL_TYPE);

//================= 초기화 함수 =================
void initNeoPixel() {
  strip.begin();                         // NeoPixel 스트립 초기화
  strip.show();                          // 모든 픽셀 끄기
  strip.setBrightness(DEFAULT_BRIGHTNESS); // 기본 밝기 설정
}

//================= 매트릭스 인덱스 변환 함수 =================
// 32x16 매트릭스에서 세로 한 줄을 위한 인덱스 계산
// x: 0~31 (가로 32개)
// y: 0~15 (세로 16개)
int getPixelIndex(int x, int y) {
  // 테스트 코드와 동일한 방식
  // 각 행(y)은 32개씩, x는 해당 행에서의 위치
  return y * 32 + x;
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
  delay(6500);
  
  // === 9.5초-10초: 100% 색상 페이드인 (개별 LED) ===
  individualPixelFade(color100_R, color100_G, color100_B, true, 500);
  
  // === 10초-12초: 100% 유지 ===
  delay(2000);
  
  // === 12초-12.5초: 100% 페이드아웃 (개별 LED) ===
  individualPixelFade(color100_R, color100_G, color100_B, false, 500);
  
  // === 12.5초-20초: OFF 유지 ===
  delay(7500);
}

//================= 새로운 시퀀스3 (가속 패턴) =================
void sequence15sec() {
  // 색상 정의
  int color30_R = 110, color30_G = 110, color30_B = 60;   // 30% 컬러
  int color70_R = 140, color70_G = 140, color70_B = 60;   // 70% 컬러
  int color100_R = 180, color100_G = 180, color100_B = 70; // 100% 컬러
  
  // === 0초: 30% 시작 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);  // 1.8초 유지
  
  // === 1.8초-2초: 30% → 70% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color70_R, color70_G, color70_B, 200);
  
  // === 2초-3.8초: 70% 유지 ===
  turnOnAllLED(color70_R, color70_G, color70_B);
  delay(1800);
  
  // === 3.8초-4초: 70% → 30% 전환 (0.2초) ===
  linearFadeShort(color70_R, color70_G, color70_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 4초-5.8초: 30% 유지 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);
  
  // === 5.8초-6초: 30% → 70% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color70_R, color70_G, color70_B, 200);
  
  // === 6초-7.8초: 70% 유지 ===
  turnOnAllLED(color70_R, color70_G, color70_B);
  delay(1800);
  
  // === 7.8초-8초: 70% → 30% 전환 (0.2초) ===
  linearFadeShort(color70_R, color70_G, color70_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 8초-9.8초: 30% 유지 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);
  
  // === 9.8초-10초: 30% → 70% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color70_R, color70_G, color70_B, 200);
  
  // === 10초: 70% → 100% 즉시 전환 ===
  turnOnAllLED(color100_R, color100_G, color100_B);
  
  // === 10초-12초: 100% 유지 (2초) ===
  delay(2000);
  
  // === 12초-12.2초: 100% → 30% 전환 (0.2초) ===
  linearFadeShort(color100_R, color100_G, color100_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 12.2초-15.5초: 30% 유지 (3.3초) ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(3300);
  
  // === 15.5초: LED OFF ===
  turnOffAllLED();
}

//================= 이징 함수 (가속도 제어) =================
float easeInOutCubic(float t) {
  // t: 0.0 ~ 1.0
  if (t < 0.5) {
    return 4 * t * t * t;
  } else {
    return 1 - pow(-2 * t + 2, 3) / 2;
  }
}

float easeInCubic(float t) {
  return t * t * t;
}

float easeOutCubic(float t) {
  return 1 - pow(1 - t, 3);
}

//================= 트래킹 컬럼 그리기 함수 =================
void drawTrackingColumn(float position, int red, int green, int blue, float gradientWidth) {
  // 16x32 매트릭스로 해석 (테스트 코드와 동일)
  // position: 0.0 ~ 15.0 (16개 세로 열)
  
  // 먼저 전체 매트릭스 클리어
  strip.clear();
  
  // 그라데이션 효과를 위한 컬럼 그리기
  for(int col = 0; col < 8; col++) {  // 16개 세로 열
    float distance = fabs(col - position);
    
    if(distance <= gradientWidth) {
      // 거리에 따른 밝기 계산 (가우시안 분포)
      float brightness = exp(-(distance * distance) / (2 * gradientWidth * gradientWidth));
      
      int actualR = (int)(red * brightness);
      int actualG = (int)(green * brightness);  
      int actualB = (int)(blue * brightness);
      
      // 테스트 코드와 완전히 동일한 방식
      for(int row = 0; row < 32; row++) {  // 각 열당 32개 픽셀
        int pixelIndex = col * 32 + row;  // 테스트 코드와 동일
        strip.setPixelColor(pixelIndex, strip.Color(actualR, actualG, actualB));
      }
    }
  }
  
  strip.show();
}

//================= 트래킹 모션 시퀀스 =================
void trackingMotion() {
  // 색상 정의 - 30% 밝기 사용
  int color30_R = 110, color30_G = 90, color30_B = 60;  // 30% 컬러
  
  // === Phase 1: 오른쪽(15)에서 왼쪽(0)으로 이동 (2초) ===
  int steps1 = 100;  // 100 스텝으로 나누기
  float startPos1 = 15.0;  // 오른쪽 끝 (16번째 열)
  float endPos1 = 0.0;     // 왼쪽 끝 (1번째 열)
  
  for(int i = 0; i <= steps1; i++) {
    float t = (float)i / steps1;
    float easedT = easeInOutCubic(t);  // 중간에 가속
    
    float currentPos = startPos1 + (endPos1 - startPos1) * easedT;
    drawTrackingColumn(currentPos, color30_R, color30_G, color30_B, 0.8);  // 그라데이션 폭 줄임
    
    delay(20);  // 20ms * 100 = 2000ms (2초)
  }
  
  // === Phase 2: 왼쪽(0)에서 1초 대기 ===
  drawTrackingColumn(0.0, color30_R, color30_G, color30_B, 0.8);
  delay(1000);
  
  // === Phase 3: 왼쪽(0)에서 중간(8)으로 이동 (1.5초) ===
  int steps2 = 75;  // 75 스텝
  float startPos2 = 0.0;
  float endPos2 = 8.0;  // 중간 위치로 이동
  
  for(int i = 0; i <= steps2; i++) {
    float t = (float)i / steps2;
    float easedT = easeOutCubic(t);  // 처음 빠르고 나중에 감속
    
    float currentPos = startPos2 + (endPos2 - startPos2) * easedT;
    drawTrackingColumn(currentPos, color30_R, color30_G, color30_B, 0.8);
    
    delay(20);  // 20ms * 75 = 1500ms (1.5초)
  }
  
  // === Phase 4: 최종 위치에서 1초 유지 ===
  drawTrackingColumn(8.0, color30_R, color30_G, color30_B, 0.8);
  delay(1000);
  
  // === Phase 5: 페이드 아웃 (0.5초) ===
  int fadeSteps = 25;
  for(int i = fadeSteps; i >= 0; i--) {
    float brightness = (float)i / fadeSteps;
    int fadeR = (int)(color30_R * brightness);
    int fadeG = (int)(color30_G * brightness);
    int fadeB = (int)(color30_B * brightness);
    
    drawTrackingColumn(8.0, fadeR, fadeG, fadeB, 0.8);
    delay(20);  // 20ms * 25 = 500ms
  }
  
  // 완전히 OFF
  strip.clear();
  strip.show();
}

//================= 통합 시퀀스 (17초까지 30% + 트래킹) =================
void sequenceWithTracking() {
  // 색상 정의
  int color30_R = 110, color30_G = 110, color30_B = 60;   // 30% 컬러
  int color70_R = 140, color70_G = 140, color70_B = 60;   // 70% 컬러
  int color100_R = 180, color100_G = 180, color100_B = 70; // 100% 컬러
  
  // === 0초: 30% 시작 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);  // 1.8초 유지
  
  // === 1.8초-2초: 30% → 70% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color70_R, color70_G, color70_B, 200);
  
  // === 2초-3.8초: 70% 유지 ===
  turnOnAllLED(color70_R, color70_G, color70_B);
  delay(1800);
  
  // === 3.8초-4초: 70% → 30% 전환 (0.2초) ===
  linearFadeShort(color70_R, color70_G, color70_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 4초-5.8초: 30% 유지 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);
  
  // === 5.8초-6초: 30% → 70% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color70_R, color70_G, color70_B, 200);
  
  // === 6초-7.8초: 70% 유지 ===
  turnOnAllLED(color70_R, color70_G, color70_B);
  delay(1800);
  
  // === 7.8초-8초: 70% → 30% 전환 (0.2초) ===
  linearFadeShort(color70_R, color70_G, color70_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 8초-9.8초: 30% 유지 ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(1800);
  
  // === 9.8초-10초: 30% → 70% 전환 (0.2초) ===
  linearFadeShort(color30_R, color30_G, color30_B,
                  color70_R, color70_G, color70_B, 200);
  
  // === 10초: 70% → 100% 즉시 전환 ===
  turnOnAllLED(color100_R, color100_G, color100_B);
  
  // === 10초-12초: 100% 유지 (2초) ===
  delay(2000);
  
  // === 12초-12.2초: 100% → 30% 전환 (0.2초) ===
  linearFadeShort(color100_R, color100_G, color100_B,
                  color30_R, color30_G, color30_B, 200);
  
  // === 12.2초-17초: 30% 유지 (4.8초) ===
  turnOnAllLED(color30_R, color30_G, color30_B);
  delay(4800);
  
  // === 17초부터: 트래킹 모션 시작 ===
  Serial.println("17초: 트래킹 모션 시작");
  trackingMotion();  // 약 6초 소요 (2초 이동 + 1초 대기 + 1.5초 이동 + 1초 유지 + 0.5초 페이드아웃)
  
  // === 총 23초에 완료 ===
}