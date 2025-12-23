// #pragma once
// #include <Arduino.h>
// #include <math.h>

// // =====================
// // SERIAL
// // =====================
// static const unsigned long BAUD = 115200;
// static const unsigned long CMD_TIMEOUT_MS = 350;

// // =====================
// // TELEMETRY
// // =====================
// static const bool  TELEMETRY_ON = true;
// static const unsigned long TELEMETRY_MS = 500;

// // =====================
// // BTS pins (SAMA DENGAN YANG SUDAH DIPASANG)
// // =====================
// static const int BTS1_R_EN = 7;
// static const int BTS1_L_EN = 8;
// static const int BTS1_RPWM = 9;    // HW PWM (Timer1)
// static const int BTS1_LPWM = 10;   // HW PWM (Timer1)

// static const int BTS2_R_EN = 11;
// static const int BTS2_L_EN = 12;
// static const int BTS2_RPWM = 3;    // HW PWM (Timer2)
// static const int BTS2_LPWM = 5;    // HW PWM (Timer0)

// // =====================
// // PWM & behavior
// // =====================
// static const int   PWM_MAX        = 220;
// static const int   PWM_DEADBAND   = 12;
// static const float CMD_DEADBAND   = 0.04f;

// static const int ACCEL_PER_SEC = 2600;
// static const int DECEL_PER_SEC = 4200;

// static const int REVERSE_GUARD_PWM = 25;

// // Steering scaling (opsi B: steering makin kuat saat throttle tinggi)
// static inline float mixFactor(float th) { return fabs(th); } // 0..1

// // Motor sync calibration (kalau perlu)
// static const float K_L = 1.00f, K_R = 1.00f;
// static const float B_L = 0.00f, B_R = 0.00f;

// // =====================
// // PCA9685
// // =====================
// static const uint8_t PCA9685_ADDR = 0x40;
// static const float   PCA9685_HZ   = 50.0f;

// // Servo channels on PCA (sesuai kamu: colok di pin 1,2,3 PCA)
// // (di kode: channel index 0/1/2)
// static const uint8_t SERVO_YAW_CH   = 0;  // Servo 1 (Yaw)
// static const uint8_t SERVO_PITCH_CH = 1;  // Servo 2 (Pitch)
// static const uint8_t SERVO_FIRE_CH  = 2;  // Servo 3 (Fire)

// // Common pulse range
// static const uint16_t SERVO_US_MIN = 900;
// static const uint16_t SERVO_US_MAX = 2100;

// // =====================
// // SERVO 1 (YAW) rules
// // =====================
// static const int SERVO_YAW_CENTER = 85;
// static const int SERVO_YAW_MIN    = 35;
// static const int SERVO_YAW_MAX    = 135;

// // =====================
// // SERVO 2 (PITCH) rules
// // =====================
// static const int SERVO_PITCH_CENTER = 90;
// static const int SERVO_PITCH_MIN    = 75;
// static const int SERVO_PITCH_MAX    = 105;

// // =====================
// // SERVO 3 (FIRE) rules
// // =====================
// static const int SERVO_FIRE_IDLE   = 90;
// static const int SERVO_FIRE_ACTIVE = 0;

// // Durasi tahan di posisi ACTIVE (dipakai oleh mekanisme fire servo)
// static const unsigned long FIRE_PULSE_MS = 140;

// // =====================
// // BLDC / ESC (Shooter)
// // =====================
// static const int ESC1_PIN = 2;
// static const int ESC2_PIN = 4;

// // ESC control (servo-style pulse)
// static const int ESC_IDLE_US = 1000;
// static const int ESC_FULL_US = 2000;

// // Timing sequence (ms)
// static const unsigned long FIRE_MOTOR_SPINUP_MS = 1000; // 1 detik
// static const unsigned long FIRE_TOTAL_MS        = 3000; // total 3 detik

#pragma once
#include <Arduino.h>
#include <math.h>

// =====================
// SERIAL
// =====================
static const unsigned long BAUD = 115200;
static const unsigned long CMD_TIMEOUT_MS = 350;

// =====================
// TELEMETRY
// =====================
static const bool  TELEMETRY_ON = true;
static const unsigned long TELEMETRY_MS = 500;

// =====================
// BTS pins (SAMA DENGAN YANG SUDAH DIPASANG)
// =====================
static const int BTS1_R_EN = 7;
static const int BTS1_L_EN = 8;
static const int BTS1_RPWM = 9;    // HW PWM (Timer1)
static const int BTS1_LPWM = 10;   // HW PWM (Timer1)

static const int BTS2_R_EN = 11;
static const int BTS2_L_EN = 12;
static const int BTS2_RPWM = 3;    // HW PWM (Timer2)
static const int BTS2_LPWM = 5;    // HW PWM (Timer0)

// =====================
// PWM & behavior
// =====================
static const int   PWM_MAX        = 220;
static const int   PWM_DEADBAND   = 12;
static const float CMD_DEADBAND   = 0.04f;

static const int ACCEL_PER_SEC = 2600;
static const int DECEL_PER_SEC = 4200;

static const int REVERSE_GUARD_PWM = 25;

// Steering scaling (opsi B: steering makin kuat saat throttle tinggi)
static inline float mixFactor(float th) { return fabs(th); } // 0..1

// Motor sync calibration (kalau perlu)
static const float K_L = 1.00f, K_R = 1.00f;
static const float B_L = 0.00f, B_R = 0.00f;

// =====================
// PCA9685
// =====================
static const uint8_t PCA9685_ADDR = 0x40;
static const float   PCA9685_HZ   = 50.0f;

// Servo channels on PCA (sesuai kamu: colok di pin 1,2,3 PCA)
// (di kode: channel index 0/1/2)
static const uint8_t SERVO_YAW_CH   = 0;  // Servo 1 (Yaw)
static const uint8_t SERVO_PITCH_CH = 1;  // Servo 2 (Pitch)
static const uint8_t SERVO_FIRE_CH  = 2;  // Servo 3 (Fire)

// ESC channels on PCA (BARU - supaya gak konflik Timer1)
static const uint8_t ESC1_CH = 3;         // ESC 1 signal di PCA ch3
static const uint8_t ESC2_CH = 4;         // ESC 2 signal di PCA ch4

// Common pulse range
static const uint16_t SERVO_US_MIN = 900;
static const uint16_t SERVO_US_MAX = 2100;

// =====================
// SERVO 1 (YAW) rules
// =====================
static const int SERVO_YAW_CENTER = 85;
static const int SERVO_YAW_MIN    = 35;
static const int SERVO_YAW_MAX    = 135;

// =====================
// SERVO 2 (PITCH) rules
// =====================
static const int SERVO_PITCH_CENTER = 90;
static const int SERVO_PITCH_MIN    = 75;
static const int SERVO_PITCH_MAX    = 105;

// =====================
// SERVO 3 (FIRE) rules
// =====================
static const int SERVO_FIRE_IDLE   = 0;
static const int SERVO_FIRE_ACTIVE = 90;

// Durasi tahan di posisi ACTIVE (dipakai oleh mekanisme fire servo)
static const unsigned long FIRE_PULSE_MS = 140;

// =====================
// BLDC / ESC (Shooter)  (via PCA9685)
// =====================
// ESC control (servo-style pulse)
static const int ESC_IDLE_US = 1000;
static const int ESC_FULL_US = 2000;

// Timing sequence (ms)
static const unsigned long FIRE_MOTOR_SPINUP_MS = 1000; // 1 detik
static const unsigned long FIRE_TOTAL_MS        = 3000; // total 3 detik

