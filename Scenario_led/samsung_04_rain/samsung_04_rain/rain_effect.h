// rain_effect.h - 비 효과 관련 헤더

#ifndef RAIN_EFFECT_H
#define RAIN_EFFECT_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// ===== 빗방울 구조체 (기존 호환성) =====
struct Raindrop {
  int x;           // x 좌표 (열 번호: 0~31)
  float y;         // y 좌표 (빗방울의 맨 아래 위치, float로 부드러운 움직임)
  bool active;     // 활성화 상태
  float speed;     // 떨어지는 속도
};

// ===== 초기화 함수 =====
void initRainEffect();

// ===== 비 효과 함수 =====
void createRaindrop();
void moveRaindrops();
void drawRaindrops();
void updateRainEffect();

// ===== 새로운 비 배경 함수 =====
void drawRainBackground();

// ===== 전역 변수 선언 (extern) =====
extern Raindrop raindrops[MAX_RAINDROPS];

#endif