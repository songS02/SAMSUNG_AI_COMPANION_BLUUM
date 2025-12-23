// lightning_effect.cpp - 번개 효과 구현 (시리얼 제거)

#include "lightning_effect.h"
#include "control.h"

// ================= 번개 색상 설정 =================
#define LIGHTNING_R 100
#define LIGHTNING_G 150
#define LIGHTNING_B 170

// ================= 런타임 상태 =================
static unsigned long lightningStartMs = 0;
static bool lightningActive = false;

// ================= 공개 함수 =================

// 번개 효과 초기화
void initLightningEffect() {
  lightningStartMs = millis();
  lightningActive = true;
  
  // 화면 초기화 후 첫 번개 켜기
  clearMatrix();
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(LIGHTNING_R, LIGHTNING_G, LIGHTNING_B));
  }
  strip.show();
}

// 번개 효과 업데이트 (간단한 타임라인)
void updateLightningEffect() {
  if (!lightningActive) return;
  
  unsigned long elapsed = millis() - lightningStartMs;
  static int lastState = -1;
  int currentState = -1;
  
  // 타임라인에 따른 상태 결정
  if (elapsed < 500) {
    currentState = 1;  // ON
  } else if (elapsed < 1800) {
    currentState = 0;  // OFF
  } else if (elapsed < 1905) {
    currentState = 1;  // ON
  } else if (elapsed < 2001) {
    currentState = 0;  // OFF
  } else if (elapsed < 2005) {
    currentState = 1;  // ON
  } else if (elapsed < 4000) {
    currentState = 0;  // OFF (긴 휴지)
  } else {
    // 5초 후 종료
    lightningActive = false;
    clearMatrix();
    strip.show();
    return;
  }
  
  // 상태가 변경되었을 때만 LED 업데이트
  if (currentState != lastState) {
    lastState = currentState;
    
    if (currentState == 1) {
      // 번개 켜기
      for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(LIGHTNING_R, LIGHTNING_G, LIGHTNING_B));
      }
      strip.show();
    } else {
      // 번개 끄기
      clearMatrix();
      strip.show();
    }
  }
}

// 번개 효과 완료 여부
bool isLightningComplete() {
  return !lightningActive;
}

// 번개 효과 강제 종료 시간 체크 (5초)
bool isLightningTimeout() {
  if (!lightningActive) return false;
  unsigned long elapsed = millis() - lightningStartMs;
  return elapsed >= 5000;
}

// 번개 효과 리셋
void resetLightningEffect() {
  lightningActive = false;
  clearMatrix();
  strip.show();
}