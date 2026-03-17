#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint8_t FOCUS_PIN = 26;
constexpr uint8_t SHUTTER_PIN = 27;
constexpr uint8_t FOCUS_BUTTON_PIN = 4;
constexpr uint8_t SHUTTER_BUTTON_PIN = 15;

constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;
constexpr uint32_t TELEMETRY_INTERVAL_MS = 500;
}  // namespace Config
