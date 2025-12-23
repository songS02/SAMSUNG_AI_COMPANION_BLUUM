// rain_effect.cpp - 새로운 비 효과 (그라데이션 빗방울)

#include "rain_effect.h"
#include "control.h"

// ================= 기존 빗방울 배열 (호환성 유지) =================
Raindrop raindrops[MAX_RAINDROPS];

// ================= 새로운 빗방울 구조체 =================
struct GradientRaindrop {
  float x, y, speed;
  bool active;
};

#define MAX_GRADIENT_RAINDROPS 3
static GradientRaindrop gradientRaindrops[MAX_GRADIENT_RAINDROPS];

// 빗방울 그라데이션 패턴 (5픽셀)
#define GRADIENT_RAINDROP_HEIGHT 5
const uint8_t raindropGradient[GRADIENT_RAINDROP_HEIGHT][3] PROGMEM = {
  {50, 50, 50},      // 가장 어두운 부분 (머리)
  {102, 102, 102},   
  {170, 170, 170},   
  {220, 220, 220},   
  {255, 255, 255}    // 가장 밝은 부분 (꼬리)
};

// ================= 초기화 함수 (Y축 반전) =================
void initRainEffect() {
  // 그라데이션 빗방울 초기화
  for(int i = 0; i < MAX_GRADIENT_RAINDROPS; i++) {
    gradientRaindrops[i].active = (i < 1);  // 처음에 1개만 활성화
    gradientRaindrops[i].x = random(MATRIX_WIDTH);
    gradientRaindrops[i].y = MATRIX_HEIGHT + random(MATRIX_HEIGHT);  // 아래에서 시작
    gradientRaindrops[i].speed = 1.4;
  }
  
  // 기존 빗방울 배열 비활성화 (호환성)
  for(int i = 0; i < MAX_RAINDROPS; i++) {
    raindrops[i].active = false;
  }
  
  randomSeed(analogRead(0));
}

// ================= 새로운 비 배경 그리기 (Y축 반전) =================
void drawRainBackground() {
  // 새로운 배경 패턴 (Y축 반전: 아래가 회색, 위가 파란색)
  for(int x = 0; x < MATRIX_WIDTH; x++) {
    for(int y = 0; y < MATRIX_HEIGHT; y++) {
      if(y == 15) {
        // 맨 아래 - 회색 하늘 (반전)
        setPixel(x, y, 144, 144, 144);
      } else if(y == 1) {
        // y=1 - 밝은 파란색 (반전)
        setPixel(x, y, 0, 111, 255);
      } else if(y == 0) {
        // 맨 위 - 진한 파란색 (반전)
        setPixel(x, y, 103, 161, 255);
      }
      // 나머지는 검은색 (빗방울이 보이도록)
    }
  }
}

// ================= 그라데이션 빗방울 업데이트 (Y축 반전) =================
void updateGradientRaindrops() {
  for(int i = 0; i < MAX_GRADIENT_RAINDROPS; i++) {
    if(gradientRaindrops[i].active) {
      // 위로 이동 (Y축 반전)
      gradientRaindrops[i].y -= gradientRaindrops[i].speed;
      
      // 화면 위로 벗어나면 아래에서 다시 시작
      if(gradientRaindrops[i].y < -GRADIENT_RAINDROP_HEIGHT) {
        gradientRaindrops[i].y = MATRIX_HEIGHT + GRADIENT_RAINDROP_HEIGHT;
        gradientRaindrops[i].x = random(MATRIX_WIDTH);
      }
    }
  }
  
  // 확률적으로 새 빗방울 활성화
  static unsigned long lastActivation = 0;
  unsigned long currentTime = millis();
  
  if(currentTime - lastActivation > 1500) {  // 1.5초마다 체크
    for(int i = 0; i < MAX_GRADIENT_RAINDROPS; i++) {
      if(!gradientRaindrops[i].active && random(100) < 40) {  // 40% 확률
        gradientRaindrops[i].active = true;
        gradientRaindrops[i].x = random(MATRIX_WIDTH);
        gradientRaindrops[i].y = MATRIX_HEIGHT + GRADIENT_RAINDROP_HEIGHT;  // 아래에서 시작
        gradientRaindrops[i].speed = 1.2 + (random(100) / 200.0);  // 1.2 ~ 1.7 속도
        lastActivation = currentTime;
        break;
      }
    }
  }
}

// ================= 그라데이션 빗방울 그리기 (Y축 반전) =================
void drawGradientRaindrops() {
  for(int i = 0; i < MAX_GRADIENT_RAINDROPS; i++) {
    if(gradientRaindrops[i].active) {
      int baseX = (int)gradientRaindrops[i].x;
      int baseY = (int)gradientRaindrops[i].y;
      
      // 5픽셀 그라데이션 빗방울 그리기 (순서 반전: 밝음→어두움)
      for(int py = 0; py < GRADIENT_RAINDROP_HEIGHT; py++) {
        int y = baseY - py;  // 위로 그리기 (반전)
        
        if(y >= 0 && y < MATRIX_HEIGHT) {
          // 그라데이션 순서 반전 (꼬리가 위, 머리가 아래)
          uint8_t brightness = pgm_read_byte(&raindropGradient[GRADIENT_RAINDROP_HEIGHT - 1 - py][0]);
          
          // 파란색 영역(y <= 1)에 들어가면 색상 조정 (반전)
          if(y <= 1) {
            // 파란색과 섞임
            int r = brightness * 0.4;
            int g = brightness * 0.6;
            int b = brightness;
            setPixel(baseX, y, r, g, b);
          } else {
            // 일반 회색 빗방울
            setPixel(baseX, y, brightness, brightness, brightness);
          }
        }
      }
    }
  }
}

// ================= 기존 인터페이스 함수들 (호환성 유지) =================

// 새로운 빗방울 생성 (더 이상 사용하지 않지만 호환성 유지)
void createRaindrop() {
  // 새로운 시스템에서는 updateGradientRaindrops()에서 처리
}

// 빗방울 이동
void moveRaindrops() {
  updateGradientRaindrops();
}

// 빗방울 그리기
void drawRaindrops() {
  drawGradientRaindrops();
}

// 비 효과 전체 업데이트
void updateRainEffect() {
  updateGradientRaindrops();
}