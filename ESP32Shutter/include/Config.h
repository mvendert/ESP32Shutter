#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint8_t FOCUS_PIN = 25;
constexpr uint8_t SHUTTER_PIN = 26;
constexpr uint8_t FOCUS_LED_PIN = 15;
constexpr uint8_t SHUTTER_LED_PIN = 16;
constexpr uint8_t FOCUS_BUTTON_PIN = 18;
constexpr uint8_t SHUTTER_BUTTON_PIN = 19;
constexpr uint8_t LASER_EMITTER_PIN = 32;
constexpr uint8_t LASER_SENSOR_PIN = 34;

// Logic level on LASER_SENSOR_PIN when the beam is detected by the sensor.
// The KY-008 receiver wiring used here pulls the line LOW while the beam is
// present.
constexpr bool LASER_SENSOR_DETECTED_STATE = LOW;

constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;
constexpr uint32_t TELEMETRY_INTERVAL_MS = 500;

// Duration of a single shutter pulse driven by the controller.
constexpr uint32_t SHUTTER_PULSE_MS = 200;

// Minimum time between accepted shutter firings from each trigger source.
constexpr uint32_t BUTTON_SHUTTER_MIN_INTERVAL_MS = 300;
constexpr uint32_t LASER_SHUTTER_MIN_INTERVAL_MS = 2000;
}  // namespace Config
