// Turret.cpp (CLEAN, JSON-only) - FIXED
#include "Turret.h"
#include "Config.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

namespace Turret {

// =====================
// PCA9685 driver
// =====================
static Adafruit_PWMServoDriver pca(PCA9685_ADDR);

// PCA freq servo standard
static const float PCA_SERVO_HZ = 50.0f;

// Mapping pulse microseconds -> PCA ticks (0..4095)
static inline uint16_t usToTicks(uint16_t us) {
  const float period_us = 1000000.0f / PCA_SERVO_HZ; // ~20000us @50Hz
  float ticks_f = (4096.0f * us) / period_us;
  if (ticks_f < 0) ticks_f = 0;
  if (ticks_f > 4095) ticks_f = 4095;
  return (uint16_t)lround(ticks_f);
}

// Angle (deg) -> pulse(us) using min/max range for that servo
static inline uint16_t mapDegToUs(int deg, int degMin, int degMax,
                                 uint16_t usMin, uint16_t usMax) {
  if (deg < degMin) deg = degMin;
  if (deg > degMax) deg = degMax;

  float t = (degMax == degMin) ? 0.0f
            : (float)(deg - degMin) / (float)(degMax - degMin);

  float us = usMin + t * (float)(usMax - usMin);
  if (us < usMin) us = usMin;
  if (us > usMax) us = usMax;
  return (uint16_t)lround(us);
}

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
static inline float deadz(float x, float dz) {
  return (fabs(x) < dz) ? 0.0f : x;
}

// =====================
// Servo channels on PCA (from Config.h)
// =====================
static const uint8_t CH_YAW   = SERVO_YAW_CH;
static const uint8_t CH_PITCH = SERVO_PITCH_CH;
static const uint8_t CH_FIRE  = SERVO_FIRE_CH;

// =====================
// SERVO rules (from Config.h)
// =====================
// Yaw
static const int YAW_CENTER  = SERVO_YAW_CENTER;
static const int YAW_MIN_DEG = SERVO_YAW_MIN;
static const int YAW_MAX_DEG = SERVO_YAW_MAX;

// Pitch
static const int PITCH_CENTER  = SERVO_PITCH_CENTER;
static const int PITCH_MIN_DEG = SERVO_PITCH_MIN;
static const int PITCH_MAX_DEG = SERVO_PITCH_MAX;

// Fire
static const int FIRE_IDLE   = SERVO_FIRE_IDLE;
static const int FIRE_ACTIVE = SERVO_FIRE_ACTIVE;

// Pulse range (from Config.h)
static const uint16_t US_MIN = SERVO_US_MIN;
static const uint16_t US_MAX = SERVO_US_MAX;

// =====================
// HOLD MODE (rate-based)
// =====================
static int yawDeg   = YAW_CENTER;
static int pitchDeg = PITCH_CENTER;

// deadzone stick
static const float STICK_DZ = 0.08f;

// (3) yaw speed diturunkan
static const float YAW_RATE_DPS   = 350.0f; // deg/s saat stick full
static const float PITCH_RATE_DPS = 120.0f;

// =====================
// FIRE ramp (pelan maju & pelan balik)
// =====================
static int firePosDeg    = FIRE_IDLE; // posisi aktual
static int fireTargetDeg = FIRE_IDLE; // target

// tahan di ACTIVE (pakai Config FIRE_PULSE_MS)
static const unsigned long FIRE_HOLD_MS = FIRE_PULSE_MS;

// ramp step config (ubah ini kalau mau lebih pelan/cepat)
static const unsigned long FIRE_STEP_MS = 10; // semakin besar -> makin pelan
static const int FIRE_STEP_DEG = 2;           // semakin kecil -> makin halus/pelan

static bool fireHoldingActive = false;
static unsigned long fireHoldStartMs = 0;
static unsigned long lastFireStepMs = 0;

static unsigned long lastMs = 0;

static void writeServoDeg(uint8_t ch, int deg, int degMin, int degMax) {
  deg = clampi(deg, degMin, degMax);
  uint16_t pulseUs = mapDegToUs(deg, degMin, degMax, US_MIN, US_MAX);
  uint16_t ticks   = usToTicks(pulseUs);
  pca.setPWM(ch, 0, ticks);
}

static void applyOutputs() {
  // (4) clamp lagi di writeServoDeg, jadi double-safety
  writeServoDeg(CH_YAW,   yawDeg,   YAW_MIN_DEG,   YAW_MAX_DEG);
  writeServoDeg(CH_PITCH, pitchDeg, PITCH_MIN_DEG, PITCH_MAX_DEG);
}

static void updateFire(unsigned long nowMs, bool fireEvent, bool safe) {
  if (safe) {
    fireHoldingActive = false;
    fireTargetDeg = FIRE_IDLE;
    firePosDeg = FIRE_IDLE;
    writeServoDeg(CH_FIRE, firePosDeg, 0, 180);
    return;
  }

  // trigger -> set target ACTIVE dan mulai hold timer
  if (fireEvent) {
    fireHoldingActive = true;
    fireHoldStartMs = nowMs;
    fireTargetDeg = FIRE_ACTIVE;
  }

  // jika sedang hold ACTIVE dan durasi habis -> target balik ke IDLE
  if (fireHoldingActive && (nowMs - fireHoldStartMs >= FIRE_HOLD_MS)) {
    fireHoldingActive = false;
    fireTargetDeg = FIRE_IDLE;
  }

  // ramp step setiap FIRE_STEP_MS
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

void setup() {
  Wire.begin();
  pca.begin();
  pca.setPWMFreq((int)PCA_SERVO_HZ);
  delay(10);

  yawDeg   = clampi(YAW_CENTER,   YAW_MIN_DEG,   YAW_MAX_DEG);
  pitchDeg = clampi(PITCH_CENTER, PITCH_MIN_DEG, PITCH_MAX_DEG);

  lastMs = millis();

  // init fire ramp
  firePosDeg = FIRE_IDLE;
  fireTargetDeg = FIRE_IDLE;
  fireHoldingActive = false;
  fireHoldStartMs = 0;
  lastFireStepMs = millis();

  // Set initial outputs
  applyOutputs();
  writeServoDeg(CH_FIRE, firePosDeg, 0, 180);
}

void update(float rx, float ry, bool fireEvent, bool safe) {
  const unsigned long nowMs = millis();

  if (safe) {
    yawDeg   = clampi(YAW_CENTER,   YAW_MIN_DEG,   YAW_MAX_DEG);
    pitchDeg = clampi(PITCH_CENTER, PITCH_MIN_DEG, PITCH_MAX_DEG);
    applyOutputs();
    updateFire(nowMs, false, true);
    lastMs = nowMs;
    return;
  }

  // =====================
  // HOLD MODE: stick = rate (tidak auto balik tengah)
  // =====================
  rx = deadz(clampf(rx, -1.0f, 1.0f), STICK_DZ);
  ry = deadz(clampf(ry, -1.0f, 1.0f), STICK_DZ);

  // (2) invert yaw & pitch (hapus salah satu jika ternyata hanya 1 yang kebalik)
  rx = -rx;
  ry = -ry;

  float dt = (nowMs - lastMs) / 1000.0f;
  lastMs = nowMs;
  if (dt < 0) dt = 0;

  // rx -> yaw, ry -> pitch
  yawDeg   += (int)lround(rx * YAW_RATE_DPS   * dt);
  pitchDeg += (int)lround(ry * PITCH_RATE_DPS * dt);

  // (4) enforce limits
  yawDeg   = clampi(yawDeg,   YAW_MIN_DEG,   YAW_MAX_DEG);
  pitchDeg = clampi(pitchDeg, PITCH_MIN_DEG, PITCH_MAX_DEG);

  applyOutputs();

  // fire ramp update (1)
  updateFire(nowMs, fireEvent, false);
}

} // namespace Turret
