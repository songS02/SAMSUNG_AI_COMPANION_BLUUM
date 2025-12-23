// lightning_effect.h - 번개 효과 헤더

#ifndef LIGHTNING_EFFECT_H
#define LIGHTNING_EFFECT_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// ===== 번개 효과 함수 =====
void initLightningEffect();
void updateLightningEffect();
bool isLightningComplete();
bool isLightningTimeout();
void resetLightningEffect();

#endif