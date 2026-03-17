#pragma once

class ITriggerInput {
 public:
  virtual ~ITriggerInput() = default;
  virtual void begin() = 0;
  virtual bool isHeld() = 0;
};
