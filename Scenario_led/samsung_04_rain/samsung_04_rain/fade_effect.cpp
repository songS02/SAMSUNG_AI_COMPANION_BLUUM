// fade_effect.cpp - 페이드 효과 구현

#include "fade_effect.h"
#include "control.h"
#include "rain_effect.h"
#include "background_effect.h"

// ================= 페이드 상태 =================
static unsigned long fadeStartMs = 0;
static bool fadeActive = false;

// ================= 페이드인 함수 =================

// 페이드 초기화
void initFadeToRain() {
  fadeStartMs = millis();
  fadeActive = true;
  
  // 화면 검은색으로 시작
  clearMatrix();
  strip.show();
}

// 페이드 업데이트
void updateFadeToRain() {
  if (!fadeActive) return;
  
  unsigned long now = millis();
  unsigned long elapsed = now - fadeStartMs;
  
  if (elapsed >= FADE_TO_RAIN_DURATION) {
    // 페이드 완료 - 최종 비 효과 표시
    updateRainWithBackground();
    fadeActive = false;
    return;
  }
  
  // 페이드 진행도 계산 (0.0 ~ 1.0)
  float progress = (float)elapsed / FADE_TO_RAIN_DURATION;
  
  // Ease-in-out 적용
  progress = easeInOutSine(progress);
  
  // 화면 지우기
  clearMatrix();
  
  // 배경 그리기 (페이드 적용)
  for(int x = 0; x < MATRIX_WIDTH; x++) {
    for(int y = 0; y < MATRIX_HEIGHT; y++) {
      uint8_t r = 0, g = 0, b = 0;
      
      // Y축 반전된 비 배경 패턴
      if(y == 15) {
        // 맨 아래 - 회색 하늘
        r = g = b = 144 * progress;
      } else if(y == 1) {
        // y=1 - 밝은 파란색
        r = 0;
        g = 111 * progress;
        b = 255 * progress;
      } else if(y == 0) {
        // 맨 위 - 진한 파란색
        r = 103 * progress;
        g = 161 * progress;
        b = 255 * progress;
      }
      
      if(r > 0 || g > 0 || b > 0) {
        setPixel(x, y, r, g, b);
      }
    }
  }
  
  // 빗방울도 페이드 적용하여 그리기
  moveRaindrops();
  drawRaindrops();  // 기존 빗방울 그리기
  
  // 전체 화면에 페이드 효과 적용 (알파 블렌딩)
  if (progress < 1.0) {
    for(int i = 0; i < LED_COUNT; i++) {
      uint32_t c = strip.getPixelColor(i);
      uint8_t r = ((c >> 16) & 0xFF) * progress;
      uint8_t g = ((c >>  8) & 0xFF) * progress;
      uint8_t b = ((c >>  0) & 0xFF) * progress;
      strip.setPixelColor(i, strip.Color(r, g, b));
    }
  }
  
  strip.show();
}

// 페이드 완료 여부
bool isFadeToRainComplete() {
  return !fadeActive;
}