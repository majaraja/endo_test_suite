#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ------------------------------------------------------------
// LOAD CELL CALIBRATION
// ------------------------------------------------------------
// Your comment said: 191000 counts for 50 g * 9.81
// That results in ~37462 counts per Newton
constexpr float COUNTS_PER_NEWTON = 37462.0f;
constexpr float ENC_COUNT_TO_NEWTONS = 1.0f / COUNTS_PER_NEWTON;

// ------------------------------------------------------------
// ENCODER CONSTANTS
// ------------------------------------------------------------
// Rotary encoder: 4092 counts = 360 degrees
constexpr float ENC_PER_REV = 4092.0f;
constexpr float DEG_PER_REV = 360.0f;

constexpr float ENC_PER_DEG = ENC_PER_REV / DEG_PER_REV;   // ≈ 11.366 enc/deg
constexpr float DEG_PER_ENC = DEG_PER_REV / ENC_PER_REV;   // ≈ 0.0879 deg/enc

// Linear actuator: measured ~3400 encoder counts = 1 cm
constexpr float ENC_PER_CM = 3400.0f;
constexpr float CM_PER_ENC = 1.0f / ENC_PER_CM;

// ------------------------------------------------------------
// STEPPER CONSTANTS
// ------------------------------------------------------------
// Stepper: 1.8 deg/step → 200 steps/rev
constexpr float STEPPER_DEG_PER_STEP = 1.8f;
constexpr float STEPS_PER_REV = 360.0f / STEPPER_DEG_PER_STEP; // 200

// Deg → steps
constexpr float DEG_TO_STEPS = 1.0f / STEPPER_DEG_PER_STEP; // ≈ 0.555 steps/deg

// CM → steps (via encoder → degrees → steps)
constexpr float CM_TO_STEPS = ENC_PER_CM * (STEPS_PER_REV / ENC_PER_REV);
// = 3400 * (200 / 4092) = ~166.1 steps/cm

// ------------------------------------------------------------
// TARGET POSITIONS
// ------------------------------------------------------------
constexpr float INS_TARGET_CM  = 10.0f;   // cm
constexpr float ROT_TARGET_DEG = 720.0f;  // deg

constexpr long INS_TARGET_POS = (long)(INS_TARGET_CM  * ENC_PER_CM);  // encoder counts
constexpr long ROT_TARGET_POS = (long)(ROT_TARGET_DEG * ENC_PER_DEG); // encoder counts

// ------------------------------------------------------------
// SPEEDS
// ------------------------------------------------------------
// User-friendly speeds:
constexpr float INS_SPEED_CM_S  = 1.0f;  // cm/s
constexpr float ROT_SPEED_DEG_S = 50.0f; // deg/s

// Convert speeds to stepper steps/sec:
constexpr float INS_SPEED_STEPS_S = INS_SPEED_CM_S  * CM_TO_STEPS;
constexpr float ROT_SPEED_STEPS_S = ROT_SPEED_DEG_S * DEG_TO_STEPS;

// ------------------------------------------------------------
// N20 Constants
// ------------------------------------------------------------

constexpr float COUNTS_PER_REVOLUTION = 12.0f;                        // 12 CPR
constexpr float GEAR_RATIO = 100.0f;                                  // 100:1 
constexpr float COUNTS_PER_REV = COUNTS_PER_REVOLUTION * GEAR_RATIO;  // 1200 counts/rev
constexpr float S_PER_MS = 1.0/1000.0;                                // s/ms
constexpr float REV_PER_DEG = 1.0/360.0;                              // rev/deg



#endif
