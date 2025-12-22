#include "Drive.h"
#include "Config.h"
#include <Arduino.h>
#include <math.h>

namespace Drive {

static int targetL = 0, targetR = 0;
static int outL = 0, outR = 0;
static unsigned long lastStepMs = 0;

static inline float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}
static inline int clampi(int x, int lo, int hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}
static inline int absi(int x) { return x < 0 ? -x : x; }
static inline int sgn(int x) { return (x > 0) - (x < 0); }

static inline float applyDeadbandF(float x, float dz) {
  return (fabs(x) < dz) ? 0.0f : x;
}

static int rampToward2(int prev, int target, int maxUp, int maxDown) {
  if (target > prev) return min(target, prev + maxUp);
  return max(target, prev - maxDown);
}

static inline void pwmWrite(int pin, int val) {
  val = constrain(val, 0, 255);
  analogWrite(pin, val);
}

static void btsWrite(int r_pwm, int l_pwm, int signedPwm) {
  int p = absi(signedPwm);
  p = clampi(p, 0, PWM_MAX);

  if (p < PWM_DEADBAND) {
    pwmWrite(r_pwm, 0);
    pwmWrite(l_pwm, 0);
    return;
  }

  if (signedPwm > 0) {
    pwmWrite(r_pwm, p);
    pwmWrite(l_pwm, 0);
  } else {
    pwmWrite(r_pwm, 0);
    pwmWrite(l_pwm, p);
  }
}

static void stopAll() {
  pwmWrite(BTS1_RPWM, 0);
  pwmWrite(BTS1_LPWM, 0);
  pwmWrite(BTS2_RPWM, 0);
  pwmWrite(BTS2_LPWM, 0);

  outL = outR = 0;
  targetL = targetR = 0;
}

static inline int applyReverseGuard(int currentOut, int desiredTarget) {
  if (desiredTarget == 0) return 0;
  if (currentOut == 0) return desiredTarget;

  if (sgn(currentOut) != sgn(desiredTarget)) {
    if (absi(currentOut) > REVERSE_GUARD_PWM) return 0;
    return desiredTarget;
  }
  return desiredTarget;
}

static void computeTargetsFromDrive(float thIn, float stIn) {
  thIn = applyDeadbandF(clampf(thIn, -1.0f, 1.0f), CMD_DEADBAND);
  stIn = applyDeadbandF(clampf(stIn, -1.0f, 1.0f), CMD_DEADBAND);

  float mix = mixFactor(thIn);
  float left  = clampf(thIn + stIn * mix, -1.0f, 1.0f);
  float right = clampf(thIn - stIn * mix, -1.0f, 1.0f);

  left  = left  * K_L + B_L;
  right = right * K_R + B_R;

  left  = clampf(left,  -1.0f, 1.0f);
  right = clampf(right, -1.0f, 1.0f);

  targetL = (int)lround(left  * PWM_MAX);
  targetR = (int)lround(right * PWM_MAX);
}

void setup() {
  pinMode(BTS1_R_EN, OUTPUT); pinMode(BTS1_L_EN, OUTPUT);
  pinMode(BTS1_RPWM, OUTPUT); pinMode(BTS1_LPWM, OUTPUT);

  pinMode(BTS2_R_EN, OUTPUT); pinMode(BTS2_L_EN, OUTPUT);
  pinMode(BTS2_RPWM, OUTPUT); pinMode(BTS2_LPWM, OUTPUT);

  digitalWrite(BTS1_R_EN, HIGH); digitalWrite(BTS1_L_EN, HIGH);
  digitalWrite(BTS2_R_EN, HIGH); digitalWrite(BTS2_L_EN, HIGH);

  stopAll();
  lastStepMs = 0;
}

void update(float thIn, float stIn, bool safe) {
  const unsigned long now = millis();

  if (safe) {
    stopAll();
    lastStepMs = now;
    return;
  }

  computeTargetsFromDrive(thIn, stIn);

  int safeTargetL = applyReverseGuard(outL, targetL);
  int safeTargetR = applyReverseGuard(outR, targetR);

  if (lastStepMs == 0) lastStepMs = now;
  unsigned long dtMs = now - lastStepMs;
  lastStepMs = now;

  int maxUp   = (int)lround(ACCEL_PER_SEC * (dtMs / 1000.0f));
  int maxDown = (int)lround(DECEL_PER_SEC * (dtMs / 1000.0f));
  if (maxUp < 1) maxUp = 1;
  if (maxDown < 1) maxDown = 1;

  outL = rampToward2(outL, safeTargetL, maxUp, maxDown);
  outR = rampToward2(outR, safeTargetR, maxUp, maxDown);

  btsWrite(BTS1_RPWM, BTS1_LPWM, outL);
  btsWrite(BTS2_RPWM, BTS2_LPWM, outR);
}

int getTargetL() { return targetL; }
int getTargetR() { return targetR; }
int getOutL() { return outL; }
int getOutR() { return outR; }

} // namespace Drive
