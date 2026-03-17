#pragma once

#include <Arduino.h>

class IShutterController {
 public:
  virtual ~IShutterController() = default;
  virtual void begin() = 0;
  virtual void setFocus(bool active) = 0;
  virtual void setShutter(bool active) = 0;
};
