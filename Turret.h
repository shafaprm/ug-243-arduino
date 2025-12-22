#pragma once

namespace Turret {
  void setup();
  void update(float rx, float ry, bool fireEvent, bool safe);

  // debug helpers
  void debugSerial();  // kontrol manual via Serial Monitor
}
