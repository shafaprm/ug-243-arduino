#pragma once
#include <Arduino.h>

namespace Turret {
  void setup();
  void update(float rx, float ry, uint8_t mode, bool fireEvent, bool safe);

  int getYawDeg();
  int getPitchDeg();
  float getRxAct();
  float getRyAct();
}


namespace Turret {
  void setup();
  // void update(float rx, float ry, bool fireEvent, bool safe);
  void update(float rx, float ry, uint8_t mode, bool fireEvent, bool safe);

  // ==================================================
  // START TAMBAHAN SALAFI
  // ==================================================
  int getYawDeg();
  int getPitchDeg();

  float getRxAct();
  float getRyAct();
  // ==================================================
  // END TAMBAHAN SALAFI
  // ==================================================

  // debug helpers
  void debugSerial();  // kontrol manual via Serial Monitor
}
