// config.h - LED 매트릭스 설정 파일

#ifndef CONFIG_H
#define CONFIG_H

// ================= 하드웨어 설정 =================
// NeoPixel 핀 설정
#define LED_PIN    6              // Arduino 디지털 핀 6번

// NeoPixel 개수
#define LED_COUNT  512            // 총 LED 개수 (32 x 16)

// 매트릭스 크기 설정
#define MATRIX_WIDTH  32          // 가로 32개
#define MATRIX_HEIGHT 16          // 세로 16개

// NeoPixel 타입 설정
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)  // WS2812B 타입

// ================= 밝기 설정 =================
// 기본 밝기 설정 (0-255)
#define DEFAULT_BRIGHTNESS 20     // 약 8% 밝기 (실내용)

// ================= 비 효과 설정 =================
#define MAX_RAINDROPS 10         // 동시에 떨어지는 빗방울 최대 개수
#define RAINDROP_LENGTH 6        // 빗방울 길이 (세로 6개 픽셀)
#define RAIN_CREATE_CHANCE 15    // 새 빗방울 생성 확률 (%)
#define RAIN_MIN_SPEED 1.0       // 빗방울 최소 속도
#define RAIN_MAX_SPEED 3.0       // 빗방울 최대 속도

// ================= 먹구름 효과 설정 (수정됨) =================
#define CLOUD_PATTERN_HEIGHT 6   // 구름 패턴 높이
#define CLOUD_SLOW_SPEED 0.08f   // 구름 느린 속도 (절반으로 감소)
#define CLOUD_FAST_SPEED 0.3f    // 구름 빠른 속도 (감소)
#define CLOUD_SLOW_FPS 30        // 구름 느린 프레임 간격 (ms)
#define CLOUD_FAST_FPS 15        // 구름 빠른 프레임 간격 (ms)
#define CLOUD_WAIT_TIME 0        // 구름 사이클 간 대기시간 제거 (0ms)
#define CLOUD_CYCLES 2           // 구름 반복 횟수

// ================= 번개 효과 설정 =================
#define LIGHTNING_TOGGLE_TIME 100    // 번개 토글 간격 (ms)
#define LIGHTNING_FLASH_COUNT 3      // 번개 플래시 횟수
#define LIGHTNING_FADE_TIME 1000     // 번개 페이드 시간 (ms)
#define LIGHTNING_COLOR_R 70         // 번개 색상 R
#define LIGHTNING_COLOR_G 70         // 번개 색상 G  
#define LIGHTNING_COLOR_B 70         // 번개 색상 B
#define LIGHTNING_OVERLAY_R 170      // 번개 오버레이 R
#define LIGHTNING_OVERLAY_G 130      // 번개 오버레이 G
#define LIGHTNING_OVERLAY_B 250      // 번개 오버레이 B

// ================= 배경 효과 설정 =================
#define CROSSFADE_CYCLE_TIME 3000    // 크로스페이드 전체 사이클 (ms)
#define PATTERN1_HOLD_TIME 1000      // 패턴1 유지 시간 (ms)
#define PATTERN_FADE_TIME 500        // 패턴 전환 시간 (ms)
#define PATTERN2_HOLD_TIME 1000      // 패턴2 유지 시간 (ms)

// ================= 타이밍 설정 =================
#define RAIN_DURATION 4000           // 비 효과 지속 시간 (ms)

// ================= 디버깅 설정 =================
// #define DEBUG_MODE               // 주석 해제시 시리얼 디버그 메시지 출력
#ifdef DEBUG_MODE
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_BAUDRATE    115200
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#endif // CONFIG_H