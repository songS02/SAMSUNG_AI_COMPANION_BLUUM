// cloud_effect.h - 먹구름 효과 헤더

#ifndef CLOUD_EFFECT_H
#define CLOUD_EFFECT_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// ===== 먹구름 모션 함수 =====
void initCloudMotion();
void drawCloudPattern();
void updateCloudMotion();
bool isCloudMotionComplete();

#endif