// cloud_effect.cpp - 먹구름 모션 효과 (대기시간 제거)

#include "cloud_effect.h"
#include "control.h"

// ================= 먹구름 모션 효과 변수 =================
static float cloudOffset = -6.0f;        // 구름 패턴 Y 오프셋 (화면 위에서 시작)
static unsigned long lastCloudUpdateMs = 0;
static int cloudCycleCount = 0;          // 현재 사이클 횟수 (0~1, 총 2번)

// 먹구름 패턴 배열
const uint8_t cloudPattern[CLOUD_PATTERN_HEIGHT][MATRIX_WIDTH] PROGMEM = {
    // y=0: 진회색
    {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20},
    // y=1: 화이트 회색  
    {80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80, 80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80},
    // y=2: 가장 밝은 화이트
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    // y=3: 화이트 회색
    {80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80, 80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80},
    // y=4: 회색
    {40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40, 40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40},
    // y=5: 진회색
    {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20, 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20}
};

// 먹구름 효과 초기화
void initCloudMotion() {
  cloudOffset = -(float)CLOUD_PATTERN_HEIGHT;  // 화면 위에서 시작
  lastCloudUpdateMs = millis();
  cloudCycleCount = 0;
}

// 먹구름 효과 패턴 그리기
void drawCloudPattern() {
  clearMatrix();
  
  for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      // 현재 화면 Y 위치에서 패턴의 어느 줄에 해당하는지 계산
      float patternY = y - cloudOffset;
      
      // 패턴 범위 내에 있는지 확인
      if(patternY >= 0 && patternY < CLOUD_PATTERN_HEIGHT) {
        int patternRow = (int)patternY;
        uint8_t brightness = pgm_read_byte(&cloudPattern[patternRow][x]);
        
        // 회색조로 설정
        setPixel(x, y, brightness, brightness, brightness);
      }
    }
  }
  
  strip.show();
}

// 먹구름 모션 업데이트 (Y축: 위에서 아래로)
void updateCloudMotion() {
  unsigned long currentTime = millis();
  
  // 더 빠른 업데이트 주기
  unsigned long frameInterval;
  float bottomLinePosition = cloudOffset + CLOUD_PATTERN_HEIGHT - 1;  // 패턴의 아래쪽 위치
  
  // y=2 근처에서만 약간 느리게, 나머지는 빠르게
  if(bottomLinePosition >= 1.5f && bottomLinePosition <= 2.5f) {
    frameInterval = CLOUD_SLOW_FPS;  // 느린 프레임률
  } else {
    frameInterval = CLOUD_FAST_FPS;  // 빠른 프레임률
  }
  
  if(currentTime - lastCloudUpdateMs >= frameInterval) {
    lastCloudUpdateMs = currentTime;
    
    // 이동 속도 (아래로 이동)
    if(bottomLinePosition >= 1.5f && bottomLinePosition <= 2.5f) {
      cloudOffset += CLOUD_SLOW_SPEED;  // 느리게
    } else {
      cloudOffset += CLOUD_FAST_SPEED;   // 빠르게
    }
    
    // 패턴이 완전히 벗어나면 즉시 다음 사이클 시작 (대기 없음)
    if(cloudOffset > MATRIX_HEIGHT) {  // 화면 아래로 벗어남
      cloudCycleCount++;
      
      if(cloudCycleCount < CLOUD_CYCLES) {
        // 대기 없이 즉시 다시 시작
        cloudOffset = -(float)CLOUD_PATTERN_HEIGHT;
      }
    }
    
    drawCloudPattern();
  }
}

// 먹구름 효과 사이클 완료 여부 확인
bool isCloudMotionComplete() {
  return (cloudCycleCount >= CLOUD_CYCLES && cloudOffset > MATRIX_HEIGHT);
}