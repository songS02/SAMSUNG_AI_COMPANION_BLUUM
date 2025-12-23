// control.h - 함수 선언 헤더 파일

#ifndef CONTROL_H
#define CONTROL_H

#include <Adafruit_NeoPixel.h>
#include "config.h"

// 초기화 함수
void initNeoPixel();

// LED 제어 함수들
void turnOnAllLED(int red, int green, int blue);
void turnOffAllLED();

// 선형 페이드 함수
void linearFade(int fromR, int fromG, int fromB, 
                int toR, int toG, int toB);

// 짧은 선형 페이드 함수 (0.2초용)
void linearFadeShort(int fromR, int fromG, int fromB,
                     int toR, int toG, int toB, int durationMs);

// 개별 픽셀 페이드 함수
void individualPixelFade(int red, int green, int blue, 
                         bool fadeIn, int durationMs);

// 웨이브 페이드 함수
void waveFade(int red, int green, int blue, 
              bool fadeIn, int durationMs);

// 그라데이션 페이드 함수
void gradientFade(int red, int green, int blue, 
                  bool fadeIn, int durationMs);

// 27초 시퀀스 (기존)
void sequence27sec();

// 20초 시퀀스 (새로운) - OFF에서 시작
void sequence20sec();

// 20초 시퀀스 v2 - 30%에서 시작
void sequence20sec_v2();

#endif