// Isi:

// btsWrite(), stopAll(), rampToward2()

// computeTargetsFromDrive()

// applyReverseGuard()

// state: targetL,targetR,outL,outR

// Expose fungsi:

// Drive::setup()

// Drive::update(th, st, safe) → melakukan output motor

#pragma once

namespace Drive {
  void setup();
  void update(float th, float st, bool safe);

  // telemetry getters
  int getTargetL();
  int getTargetR();
  int getOutL();
  int getOutR();
}

