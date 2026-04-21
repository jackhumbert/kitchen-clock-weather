#pragma once

#include <stdint.h>

namespace BoardConfig {
inline constexpr uint16_t kDisplayWidth = 466;
inline constexpr uint16_t kDisplayHeight = 466;
inline constexpr uint16_t kLvglBufferLines = 40;

inline constexpr int kLcdSdio0 = 4;
inline constexpr int kLcdSdio1 = 5;
inline constexpr int kLcdSdio2 = 6;
inline constexpr int kLcdSdio3 = 7;
inline constexpr int kLcdSclk = 38;
inline constexpr int kLcdCs = 12;
inline constexpr int kLcdReset = 39;

inline constexpr int kI2cSda = 15;
inline constexpr int kI2cScl = 14;
inline constexpr int kTouchInterrupt = 11;
inline constexpr int kTouchReset = 40;
}