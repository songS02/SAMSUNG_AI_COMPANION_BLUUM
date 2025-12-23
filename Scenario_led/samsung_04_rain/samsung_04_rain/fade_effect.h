// fade_effect.h - 페이드 효과 헤더

#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// ===== 페이드 효과 함수 =====
void initFadeToRain();
void updateFadeToRain();
bool isFadeToRainComplete();

// ===== 페이드 설정 =====
#define FADE_TO_RAIN_DURATION 1000  // 1초간 페이드인

#endif