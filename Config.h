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

// Common pulse range (lebih konservatif supaya servo patuh limit & tidak dorong mentok)
// Kalau mau lebih luas, bisa balik ke 600..2400.
static const uint16_t SERVO_US_MIN = 900;
static const uint16_t SERVO_US_MAX = 2100;

// =====================
// SERVO 1 (YAW) rules
// =====================
static const int SERVO_YAW_CENTER = 85;

// (5) Kurangi limit yaw biar tidak terlalu mentok beloknya
// Silakan adjust: makin kecil range -> makin aman
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
static const int SERVO_FIRE_IDLE   = 90;
static const int SERVO_FIRE_ACTIVE = 0;      // posisi "tembak"

// Durasi tahan di posisi ACTIVE (bukan durasi total gerak; gerak dibuat ramp di Turret.cpp)
static const unsigned long FIRE_PULSE_MS = 140;
