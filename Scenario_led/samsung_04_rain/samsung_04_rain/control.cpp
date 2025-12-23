// control.cpp - 기본 LED 제어 함수만 포함

#include "control.h"

//================= NeoPixel 객체 =================
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, PIXEL_TYPE);

//================= 초기화 함수 =================
void initNeoPixel() {
  strip.begin();                         // NeoPixel 스트립 초기화
  strip.show();                          // 모든 픽셀 끄기
  strip.setBrightness(DEFAULT_BRIGHTNESS); // 기본 밝기 설정
}

//================= 매트릭스 제어 함수 =================

// 2D 좌표를 1D 인덱스로 변환
// LED가 세로로 연결되어 있음 (지그재그 가능성 체크)
int getPixelIndex(int x, int y) {
  if(x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) {
    return -1; // 범위 벗어남
  }
  
  // 세로 지그재그 배열 테스트
  // 짝수 열은 위→아래, 홀수 열은 아래→위
  if(x % 2 == 0) {
    // 짝수 열: 위에서 아래로
    return x * MATRIX_HEIGHT + y;
  } else {
    // 홀수 열: 아래에서 위로 (반대)
    return x * MATRIX_HEIGHT + (MATRIX_HEIGHT - 1 - y);
  }
}

// 특정 좌표의 픽셀 색상 설정
void setPixel(int x, int y, int red, int green, int blue) {
  int index = getPixelIndex(x, y);
  if(index >= 0 && index < LED_COUNT) {
    strip.setPixelColor(index, strip.Color(red, green, blue));
  }
}

// 매트릭스 전체 지우기
void clearMatrix() {
  strip.clear();
}

// ================= Easing 함수 =================
// Ease-in-out 함수: 시작과 끝에서 느리고 중간에서 빠름
float easeInOutCubic(float t) {
  if (t < 0.5) {
    return 4 * t * t * t;
  } else {
    float p = 2 * t - 2;
    return 1 + p * p * p / 2;
  }
}

// Sine 기반 easing (부드러운 모션 제어)
float easeInOutSine(float t) {
  return -(cos(PI * t) - 1) / 2;
}

// 오버레이 관련 함수
void overlayWhiteAlpha(float alpha) {
  overlayColorAlpha(170, 130, 250, alpha);
}

void overlayColorAlpha(uint8_t targetR, uint8_t targetG, uint8_t targetB, float alpha) {
  if (alpha <= 0.0f) return;
  if (alpha > 1.0f) alpha = 1.0f;

  for (int i = 0; i < LED_COUNT; ++i) {
    uint32_t c = strip.getPixelColor(i);
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >>  8) & 0xFF;
    uint8_t b = (c >>  0) & 0xFF;

    // out = old + (target - old) * alpha
    uint8_t nr = r + (uint8_t)((targetR - r) * alpha);
    uint8_t ng = g + (uint8_t)((targetG - g) * alpha);
    uint8_t nb = b + (uint8_t)((targetB - b) * alpha);

    strip.setPixelColor(i, strip.Color(nr, ng, nb));
  }
}