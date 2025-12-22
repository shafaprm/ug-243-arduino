#pragma once
#include <Arduino.h>

namespace Comm {
  void setup();
  void poll();

  float getTh();
  float getSt();
  float getRx();
  float getRy();

  bool consumeFire();

  bool getEstop();
  bool isTimeout();
  bool isSafeMode();

  const char* getMode();
  unsigned long ageMs();

  // untuk telemetry/debug
  bool consumeParseErrorFlag();
}
