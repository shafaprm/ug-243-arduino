#include "Config.h"
#include "Drive.h"
#include "Turret.h"
#include "Comm.h"
#include "Telemetry.h"

void setup() {
  Comm::setup();
  Drive::setup();
  Turret::setup();
  Telemetry::sendBoot();
}

void loop() {
  Comm::poll();

  bool timeout  = Comm::isTimeout();
  bool safeMode = Comm::isSafeMode();
  bool estop    = Comm::getEstop();
  bool safe     = timeout || safeMode || estop;

  Drive::update(Comm::getTh(), Comm::getSt(), safe);
  Turret::update(
    Comm::getRx(),
    Comm::getRy(),
    Comm::consumeFire(),
    safe
  );

  Telemetry::tick(timeout, safe);
}
