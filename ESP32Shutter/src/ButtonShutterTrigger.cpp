#include "ButtonShutterTrigger.h"

ButtonShutterTrigger::ButtonShutterTrigger(ITriggerInput& input,
                       uint32_t minimumIntervalMs)
  : input_(input), minimumIntervalMs_(minimumIntervalMs), lastHeld_(false) {}

void ButtonShutterTrigger::begin() {
  // The underlying ITriggerInput is initialized by its owner; we only need to
  // seed our edge-detection state. Treat startup as "not held" so the very
  // first read after begin() can produce an edge if needed.
  lastHeld_ = false;
}

bool ButtonShutterTrigger::consumeFireRequest() {
  const bool held = input_.isHeld();
  const bool risingEdge = held && !lastHeld_;
  lastHeld_ = held;
  return risingEdge;
}

uint32_t ButtonShutterTrigger::minimumIntervalMs() const {
  return minimumIntervalMs_;
}
