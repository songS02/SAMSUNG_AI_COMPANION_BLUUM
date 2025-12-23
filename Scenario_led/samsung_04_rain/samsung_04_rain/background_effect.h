// background_effect.h - 배경 효과 헤더

#ifndef BACKGROUND_EFFECT_H
#define BACKGROUND_EFFECT_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// ===== 크로스페이드 배경 효과 함수 =====
void initCrossFadeEffect();
void drawCrossFadeBackground();
void updateRainWithBackground();

#endif