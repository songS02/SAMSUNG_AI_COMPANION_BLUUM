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

// 27초 시퀀스
void sequence27sec();

#endif