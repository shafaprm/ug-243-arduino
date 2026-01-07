#pragma once
#include <Arduino.h>

namespace Comm {

  // ==================================================
  // Start TAMBAHAN SALAFI
  // ==================================================
  enum TurretMode : uint8_t{
    TURRET_RATE = 0,
    TURRET_POS = 1
  };
  // ==================================================
  // END TAMBAHAN SALAFI
  // ==================================================

  void setup();
  void poll();

  float getTh();
  float getSt();
  float getRx();
  float getRy();
  // ==================================================
  // Start TAMBAHAN SALAFI
  // ==================================================
  TurretMode getTurretMode();
  // ==================================================
  // END TAMBAHAN SALAFI
  // ==================================================
  bool consumeFire();

  bool getEstop();
  bool isTimeout();
  bool isSafeMode();

  const char* getMode();
  unsigned long ageMs();

  // untuk telemetry/debug
  bool consumeParseErrorFlag();
}
