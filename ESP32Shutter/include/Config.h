#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint8_t FOCUS_PIN = 25;
constexpr uint8_t SHUTTER_PIN = 26;
constexpr uint8_t FOCUS_LED_PIN = 15;
constexpr uint8_t SHUTTER_LED_PIN = 16;
constexpr uint8_t FOCUS_BUTTON_PIN = 18;
constexpr uint8_t SHUTTER_BUTTON_PIN = 19;

constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;
constexpr uint32_t TELEMETRY_INTERVAL_MS = 500;
}  // namespace Config
