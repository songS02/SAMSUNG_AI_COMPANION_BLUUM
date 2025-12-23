// control.h - 메인 제어 헤더 파일

#ifndef CONTROL_H
#define CONTROL_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// ===== 초기화 함수 =====
void initNeoPixel();

// ===== 매트릭스 제어 함수 =====
int getPixelIndex(int x, int y);
void setPixel(int x, int y, int red, int green, int blue);
void clearMatrix();

// ===== Easing 함수 =====
float easeInOutCubic(float t);
float easeInOutSine(float t);

// ===== 오버레이 함수 =====
void overlayWhiteAlpha(float alpha);
void overlayColorAlpha(uint8_t targetR, uint8_t targetG, uint8_t targetB, float alpha);

// ===== 전역 변수 선언 (extern) =====
extern Adafruit_NeoPixel strip;

#endif