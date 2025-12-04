#ifndef PINS_H
#define PINS_H

#include <stdint.h>

// Load cell pins
constexpr uint8_t LOADCELL_CLK = 5;
constexpr uint8_t LOADCELL_DAT = 7;

// Stepper motor pins
constexpr uint8_t INS_STEP_PIN = 4;
constexpr uint8_t INS_DIR_PIN  = 3;

constexpr uint8_t ROT_STEP_PIN = 2;
constexpr uint8_t ROT_DIR_PIN  = 1;

// Stepper encoder pins
constexpr uint8_t INS_ENC_PIN_A = 39;
constexpr uint8_t INS_ENC_PIN_B = 40;

constexpr uint8_t ROT_ENC_PIN_A = 37;
constexpr uint8_t ROT_ENC_PIN_B = 38;


// N20 motor pins
constexpr uint8_t MOTOR_A_PIN1 = 47;
constexpr uint8_t MOTOR_A_PIN2 = 35;

constexpr uint8_t MOTOR_B_PIN1 = 34;
constexpr uint8_t MOTOR_B_PIN2 = 33;

// N20 encoder pins
constexpr uint8_t ENC_A_PIN_AW = 45;
constexpr uint8_t ENC_A_PIN_BY = 46;

constexpr uint8_t ENC_B_PIN_AW = 41;
constexpr uint8_t ENC_B_PIN_BY = 42;

#endif
