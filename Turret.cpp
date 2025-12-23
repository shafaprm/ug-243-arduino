// // Turret.cpp (FINAL, FIXED, STATE MACHINE)
// #include "Turret.h"
// #include "Config.h"
// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>
// #include <Servo.h>
// #include <math.h>

// namespace Turret {

// // ==================================================
// // ESC (BLDC Shooter)
// // ==================================================
// static Servo esc1;
// static Servo esc2;

// // ==================================================
// // PCA9685
// // ==================================================
// static Adafruit_PWMServoDriver pca(PCA9685_ADDR);
// static const float PCA_SERVO_HZ = 50.0f;

// // us -> PCA ticks
// static inline uint16_t usToTicks(uint16_t us) {
//   const float period_us = 1000000.0f / PCA_SERVO_HZ;
//   float t = (4096.0f * us) / period_us;
//   if (t < 0) t = 0;
//   if (t > 4095) t = 4095;
//   return (uint16_t)lround(t);
// }

// // deg -> us
// static inline uint16_t mapDegToUs(
//   int deg, int dMin, int dMax, uint16_t usMin, uint16_t usMax
// ) {
//   deg = constrain(deg, dMin, dMax);
//   float k = (dMax == dMin) ? 0.0f : (float)(deg - dMin) / (float)(dMax - dMin);
//   float us = usMin + k * (float)(usMax - usMin);
//   if (us < usMin) us = usMin;
//   if (us > usMax) us = usMax;
//   return (uint16_t)lround(us);
// }

// // ==================================================
// // SERVO CHANNELS
// // ==================================================
// static const uint8_t CH_YAW   = SERVO_YAW_CH;
// static const uint8_t CH_PITCH = SERVO_PITCH_CH;
// static const uint8_t CH_FIRE  = SERVO_FIRE_CH;

// // ==================================================
// // YAW/PITCH HOLD MODE
// // ==================================================
// static int yawDeg   = SERVO_YAW_CENTER;
// static int pitchDeg = SERVO_PITCH_CENTER;

// static const float STICK_DZ = 0.08f;
// static const float YAW_RATE_DPS   = 350.0f;
// static const float PITCH_RATE_DPS = 120.0f;

// // ==================================================
// // FIRE SERVO (PCA) RAMP
// // ==================================================
// static int firePosDeg    = SERVO_FIRE_IDLE;
// static int fireTargetDeg = SERVO_FIRE_IDLE;

// static unsigned long lastFireStepMs = 0;
// static const unsigned long FIRE_STEP_MS = 10;
// static const int FIRE_STEP_DEG = 2;

// // ==================================================
// // FIRE STATE MACHINE
// // ==================================================
// enum FireState {
//   FIRE_IDLE,
//   FIRE_SPINUP,
//   FIRE_SERVO,
//   FIRE_STOP
// };

// static FireState fireState = FIRE_IDLE;
// static unsigned long fireStartMs = 0;
// static unsigned long lastMs = 0;

// // ==================================================
// // LOW-LEVEL SERVO WRITE
// // ==================================================
// static void writeServoDeg(uint8_t ch, int deg, int dMin, int dMax) {
//   uint16_t us = mapDegToUs(deg, dMin, dMax, SERVO_US_MIN, SERVO_US_MAX);
//   pca.setPWM(ch, 0, usToTicks(us));
// }

// static void applyYawPitch() {
//   writeServoDeg(CH_YAW, yawDeg, SERVO_YAW_MIN, SERVO_YAW_MAX);
//   writeServoDeg(CH_PITCH, pitchDeg, SERVO_PITCH_MIN, SERVO_PITCH_MAX);
// }

// // ==================================================
// // FIRE UPDATE (ESC + SERVO)
// // ==================================================
// static void updateFire(unsigned long nowMs, bool fireEvent, bool safe) {

//   if (safe) {
//     esc1.writeMicroseconds(ESC_IDLE_US);
//     esc2.writeMicroseconds(ESC_IDLE_US);
//     fireTargetDeg = SERVO_FIRE_IDLE;
//     fireState = FIRE_IDLE;
//     return;
//   }

//   // Trigger (Square)
//   if (fireEvent && fireState == FIRE_IDLE) {
//     fireState = FIRE_SPINUP;
//     fireStartMs = nowMs;

//     // (1) BLDC full speed
//     esc1.writeMicroseconds(ESC_FULL_US);
//     esc2.writeMicroseconds(ESC_FULL_US);
//   }

//   switch (fireState) {

//     case FIRE_SPINUP:
//       // (2) after 1 sec -> start servo firing
//       if (nowMs - fireStartMs >= FIRE_MOTOR_SPINUP_MS) {
//         fireState = FIRE_SERVO;
//         fireTargetDeg = SERVO_FIRE_ACTIVE;
//       }
//       break;

//     case FIRE_SERVO:
//       // hold until total 3 sec since start
//       if (nowMs - fireStartMs >= FIRE_TOTAL_MS) {
//         fireState = FIRE_STOP;
//       }
//       break;

//     case FIRE_STOP:
//       // stop BLDC + return servo
//       esc1.writeMicroseconds(ESC_IDLE_US);
//       esc2.writeMicroseconds(ESC_IDLE_US);
//       fireTargetDeg = SERVO_FIRE_IDLE;
//       fireState = FIRE_IDLE;
//       fireStartMs = 0;
//       break;

//     default:
//       break;
//   }

//   // Servo fire ramp (maju/mundur halus)
//   if (nowMs - lastFireStepMs >= FIRE_STEP_MS) {
//     lastFireStepMs = nowMs;

//     if (firePosDeg < fireTargetDeg) {
//       firePosDeg = min(firePosDeg + FIRE_STEP_DEG, fireTargetDeg);
//     } else if (firePosDeg > fireTargetDeg) {
//       firePosDeg = max(firePosDeg - FIRE_STEP_DEG, fireTargetDeg);
//     }

//     writeServoDeg(CH_FIRE, firePosDeg, 0, 180);
//   }
// }

// // ==================================================
// // PUBLIC API
// // ==================================================
// void setup() {
//   Wire.begin();
//   pca.begin();
//   pca.setPWMFreq((int)PCA_SERVO_HZ);
//   delay(10);

//   // ESC attach + arm idle
//   esc1.attach(ESC1_PIN);
//   esc2.attach(ESC2_PIN);
//   esc1.writeMicroseconds(ESC_IDLE_US);
//   esc2.writeMicroseconds(ESC_IDLE_US);

//   yawDeg   = constrain(SERVO_YAW_CENTER, SERVO_YAW_MIN, SERVO_YAW_MAX);
//   pitchDeg = constrain(SERVO_PITCH_CENTER, SERVO_PITCH_MIN, SERVO_PITCH_MAX);

//   firePosDeg = SERVO_FIRE_IDLE;
//   fireTargetDeg = SERVO_FIRE_IDLE;

//   lastMs = millis();
//   lastFireStepMs = millis();

//   applyYawPitch();
//   writeServoDeg(CH_FIRE, firePosDeg, 0, 180);
// }

// void update(float rx, float ry, bool fireEvent, bool safe) {
//   const unsigned long nowMs = millis();

//   if (safe) {
//     yawDeg   = SERVO_YAW_CENTER;
//     pitchDeg = SERVO_PITCH_CENTER;
//     applyYawPitch();
//     updateFire(nowMs, false, true);
//     lastMs = nowMs;
//     return;
//   }

//   // deadzone + invert (kalau kebalik, hapus salah satunya)
//   rx = (fabs(rx) < STICK_DZ) ? 0.0f : -rx;
//   ry = (fabs(ry) < STICK_DZ) ? 0.0f : -ry;

//   float dt = (nowMs - lastMs) / 1000.0f;
//   lastMs = nowMs;
//   if (dt < 0) dt = 0;

//   yawDeg   += (int)lround(rx * YAW_RATE_DPS * dt);
//   pitchDeg += (int)lround(ry * PITCH_RATE_DPS * dt);

//   yawDeg   = constrain(yawDeg, SERVO_YAW_MIN, SERVO_YAW_MAX);
//   pitchDeg = constrain(pitchDeg, SERVO_PITCH_MIN, SERVO_PITCH_MAX);

//   applyYawPitch();
//   updateFire(nowMs, fireEvent, false);
// }

// } // namespace Turret

// Turret.cpp (FINAL, FIXED, ESC VIA PCA9685 - NO Servo.h)
#include "Turret.h"
#include "Config.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

namespace Turret {

// ==================================================
// PCA9685
// ==================================================
static Adafruit_PWMServoDriver pca(PCA9685_ADDR);
static const float PCA_SERVO_HZ = 50.0f;

// us -> PCA ticks
static inline uint16_t usToTicks(uint16_t us) {
  const float period_us = 1000000.0f / PCA_SERVO_HZ;
  float t = (4096.0f * us) / period_us;
  if (t < 0) t = 0;
  if (t > 4095) t = 4095;
  return (uint16_t)lround(t);
}

// deg -> us
static inline uint16_t mapDegToUs(
  int deg, int dMin, int dMax, uint16_t usMin, uint16_t usMax
) {
  deg = constrain(deg, dMin, dMax);
  float k = (dMax == dMin) ? 0.0f : (float)(deg - dMin) / (float)(dMax - dMin);
  float us = usMin + k * (float)(usMax - usMin);
  if (us < usMin) us = usMin;
  if (us > usMax) us = usMax;
  return (uint16_t)lround(us);
}

// ==================================================
// SERVO & ESC CHANNELS (PCA9685)
// ==================================================
static const uint8_t CH_YAW   = SERVO_YAW_CH;
static const uint8_t CH_PITCH = SERVO_PITCH_CH;
static const uint8_t CH_FIRE  = SERVO_FIRE_CH;

static const uint8_t CH_ESC1  = ESC1_CH;
static const uint8_t CH_ESC2  = ESC2_CH;

// ==================================================
// YAW/PITCH HOLD MODE
// ==================================================
static int yawDeg   = SERVO_YAW_CENTER;
static int pitchDeg = SERVO_PITCH_CENTER;

static const float STICK_DZ = 0.08f;
static const float YAW_RATE_DPS   = 350.0f;
static const float PITCH_RATE_DPS = 120.0f;

// ==================================================
// FIRE SERVO (PCA) RAMP
// ==================================================
static int firePosDeg    = SERVO_FIRE_IDLE;
static int fireTargetDeg = SERVO_FIRE_IDLE;

static unsigned long lastFireStepMs = 0;
static const unsigned long FIRE_STEP_MS = 10;
static const int FIRE_STEP_DEG = 2;

// ==================================================
// FIRE STATE MACHINE
// ==================================================
enum FireState {
  FIRE_IDLE,
  FIRE_SPINUP,
  FIRE_SERVO,
  FIRE_STOP
};

static FireState fireState = FIRE_IDLE;
static unsigned long fireStartMs = 0;
static unsigned long lastMs = 0;

// ==================================================
// LOW-LEVEL WRITE
// ==================================================
static void writeServoDeg(uint8_t ch, int deg, int dMin, int dMax) {
  uint16_t us = mapDegToUs(deg, dMin, dMax, SERVO_US_MIN, SERVO_US_MAX);
  pca.setPWM(ch, 0, usToTicks(us));
}

static void writeEscUs(uint8_t ch, int us) {
  // clamp sesuai config ESC
  us = constrain(us, ESC_IDLE_US, ESC_FULL_US);
  pca.setPWM(ch, 0, usToTicks((uint16_t)us));
}

static void applyYawPitch() {
  writeServoDeg(CH_YAW, yawDeg, SERVO_YAW_MIN, SERVO_YAW_MAX);
  writeServoDeg(CH_PITCH, pitchDeg, SERVO_PITCH_MIN, SERVO_PITCH_MAX);
}

// ==================================================
// FIRE UPDATE (ESC + SERVO)
// ==================================================
static void updateFire(unsigned long nowMs, bool fireEvent, bool safe) {

  if (safe) {
    writeEscUs(CH_ESC1, ESC_IDLE_US);
    writeEscUs(CH_ESC2, ESC_IDLE_US);
    fireTargetDeg = SERVO_FIRE_IDLE;
    fireState = FIRE_IDLE;
    return;
  }

  // Trigger (Square)
  if (fireEvent && fireState == FIRE_IDLE) {
    fireState = FIRE_SPINUP;
    fireStartMs = nowMs;

    // (1) BLDC full speed
    writeEscUs(CH_ESC1, ESC_FULL_US);
    writeEscUs(CH_ESC2, ESC_FULL_US);
  }

  switch (fireState) {

    case FIRE_SPINUP:
      // (2) after spinup -> start servo firing
      if (nowMs - fireStartMs >= FIRE_MOTOR_SPINUP_MS) {
        fireState = FIRE_SERVO;
        fireTargetDeg = SERVO_FIRE_ACTIVE;
      }
      break;

    case FIRE_SERVO:
      // hold until total time since start
      if (nowMs - fireStartMs >= FIRE_TOTAL_MS) {
        fireState = FIRE_STOP;
      }
      break;

    case FIRE_STOP:
      // stop BLDC + return servo
      writeEscUs(CH_ESC1, ESC_IDLE_US);
      writeEscUs(CH_ESC2, ESC_IDLE_US);
      fireTargetDeg = SERVO_FIRE_IDLE;
      fireState = FIRE_IDLE;
      fireStartMs = 0;
      break;

    default:
      break;
  }

  // Servo fire ramp (halus)
  if (nowMs - lastFireStepMs >= FIRE_STEP_MS) {
    lastFireStepMs = nowMs;

    if (firePosDeg < fireTargetDeg) {
      firePosDeg = min(firePosDeg + FIRE_STEP_DEG, fireTargetDeg);
    } else if (firePosDeg > fireTargetDeg) {
      firePosDeg = max(firePosDeg - FIRE_STEP_DEG, fireTargetDeg);
    }

    writeServoDeg(CH_FIRE, firePosDeg, 0, 180);
  }
}

// ==================================================
// PUBLIC API
// ==================================================
void setup() {
  Wire.begin();

  pca.begin();
  pca.setPWMFreq((int)PCA_SERVO_HZ);
  delay(10);

  // Arm ESC to idle via PCA
  writeEscUs(CH_ESC1, ESC_IDLE_US);
  writeEscUs(CH_ESC2, ESC_IDLE_US);

  yawDeg   = constrain(SERVO_YAW_CENTER, SERVO_YAW_MIN, SERVO_YAW_MAX);
  pitchDeg = constrain(SERVO_PITCH_CENTER, SERVO_PITCH_MIN, SERVO_PITCH_MAX);

  firePosDeg = SERVO_FIRE_IDLE;
  fireTargetDeg = SERVO_FIRE_IDLE;

  lastMs = millis();
  lastFireStepMs = millis();

  applyYawPitch();
  writeServoDeg(CH_FIRE, firePosDeg, 0, 180);
}

void update(float rx, float ry, bool fireEvent, bool safe) {
  const unsigned long nowMs = millis();

  if (safe) {
    yawDeg   = SERVO_YAW_CENTER;
    pitchDeg = SERVO_PITCH_CENTER;
    applyYawPitch();
    updateFire(nowMs, false, true);
    lastMs = nowMs;
    return;
  }

  // deadzone + invert (kalau kebalik, hapus salah satunya)
  rx = (fabs(rx) < STICK_DZ) ? 0.0f : -rx;
  ry = (fabs(ry) < STICK_DZ) ? 0.0f : -ry;

  float dt = (nowMs - lastMs) / 1000.0f;
  lastMs = nowMs;
  if (dt < 0) dt = 0;

  yawDeg   += (int)lround(rx * YAW_RATE_DPS * dt);
  pitchDeg += (int)lround(ry * PITCH_RATE_DPS * dt);

  yawDeg   = constrain(yawDeg, SERVO_YAW_MIN, SERVO_YAW_MAX);
  pitchDeg = constrain(pitchDeg, SERVO_PITCH_MIN, SERVO_PITCH_MAX);

  applyYawPitch();
  updateFire(nowMs, fireEvent, false);
}

} // namespace Turret

