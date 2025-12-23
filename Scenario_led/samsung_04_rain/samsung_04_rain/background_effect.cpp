// background_effect.cpp - 수정된 배경 효과

#include "background_effect.h"
#include "control.h"
#include "rain_effect.h"

// ================= 전역 변수 =================
static unsigned long fadeStartTime = 0;

// 크로스페이드 초기화 (더 이상 크로스페이드 하지 않지만 호환성 유지)
void initCrossFadeEffect() {
  fadeStartTime = millis();
}

// 새로운 정적 배경 그리기 (외부 함수 호출)
void drawCrossFadeBackground() {
  // rain_effect.cpp에 정의된 새로운 배경 그리기 함수 호출
  drawRainBackground();
}

// 비와 배경 결합 효과
void updateRainWithBackground() {
  // 1. 화면 지우기
  clearMatrix();
  
  // 2. 새로운 비 배경 그리기
  drawRainBackground();
  
  // 3. 그라데이션 빗방울 업데이트
  moveRaindrops();  // 실제로는 updateGradientRaindrops() 호출
  
  // 4. 그라데이션 빗방울 그리기
  drawRaindrops();  // 실제로는 drawGradientRaindrops() 호출
  
  // 5. 화면 업데이트
  strip.show();
}