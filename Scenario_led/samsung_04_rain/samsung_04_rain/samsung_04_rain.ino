// samsung_04_rain_F.ino - 메인 프로그램 (30초 시퀀스)
// 먹구름(6초) → 비(6초) → 번개(5초) → 페이드인 → 비(13초) → 종료

#include "control.h"
#include "rain_effect.h"
#include "background_effect.h"
#include "cloud_effect.h"
#include "lightning_effect.h"
#include "fade_effect.h"

enum Mode {
  MODE_CLOUD_MOTION = 0,    // 먹구름 모션 (0-6초)
  MODE_RAIN_FIRST,          // 첫 번째 비 효과 (6-12초)
  MODE_LIGHTNING,           // 번개 효과 (12-17초)
  MODE_FADE_TO_RAIN,        // 비로 페이드인
  MODE_RAIN_SECOND,         // 두 번째 비 효과 (17-30초)
  MODE_COMPLETE             // 완료 상태
};

Mode currentMode = MODE_CLOUD_MOTION;
unsigned long programStartMs = 0;  // 전체 프로그램 시작 시간

void setup() {
  // 기본 초기화
  initNeoPixel();
  
  // 프로그램 시작 시간 기록
  programStartMs = millis();
  
  // 첫 번째 모드 초기화
  currentMode = MODE_CLOUD_MOTION;
  initCloudMotion();
}

void loop() {
  unsigned long now = millis();
  unsigned long programElapsed = now - programStartMs;
  
  // 30초 후 프로그램 종료
  if (programElapsed >= 17000) {
    if (currentMode != MODE_COMPLETE) {
      currentMode = MODE_COMPLETE;
      clearMatrix();
      strip.show();
    }
    return;
  }
  
  // 각 모드별 처리 및 전환
  switch(currentMode) {
    case MODE_CLOUD_MOTION:
      updateCloudMotion();
      
      // 6초 경과 또는 먹구름 완료시 즉시 비로 전환
      if (programElapsed >= 6500 || isCloudMotionComplete()) {
        currentMode = MODE_RAIN_FIRST;
        // 화면 초기화 없이 바로 전환
        initRainEffect();
        initCrossFadeEffect();
      }
      break;
      
    case MODE_RAIN_FIRST:
      updateRainWithBackground();
      
      // 정확히 12초에 번개로 전환
      if (programElapsed >= 6000) {
        currentMode = MODE_LIGHTNING;
        clearMatrix();
        strip.show();
        delay(20);
        initLightningEffect();
      }
      break;
      
    case MODE_LIGHTNING:
      updateLightningEffect();
      
      // 17초에 페이드인 시작
      if (programElapsed >= 9500) {
        currentMode = MODE_FADE_TO_RAIN;
        clearMatrix();
        strip.show();
        initRainEffect();
        initFadeToRain();
      }
      break;
      
    case MODE_FADE_TO_RAIN:
      updateFadeToRain();
      
      // 18초에 두 번째 비로 전환
      if (programElapsed >= 18000 || isFadeToRainComplete()) {
        currentMode = MODE_RAIN_SECOND;
      }
      break;
      
    case MODE_RAIN_SECOND:
      updateRainWithBackground();
      // 30초까지 계속 (상단에서 처리)
      break;
      
    case MODE_COMPLETE:
      // 아무 동작 없음
      break;
  }
  
  // 프레임 안정성
  delay(5);
}