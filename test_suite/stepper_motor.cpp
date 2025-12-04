#include "stepper_motor.h"
#include <Arduino.h>

StepperMotor::StepperMotor(uint8_t stepPin, uint8_t dirPin, uint8_t encPinA, uint8_t encPinB,
                           long targetPos, long speed, bool invert)
  : stepPin(stepPin), dirPin(dirPin), encPinA(encPinA), encPinB(encPinB),
    endPos(targetPos), stepSpeed(speed), invertEncoder(invert) {}

void StepperMotor::begin() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(dirPin, HIGH);

  encoder.attachHalfQuad(encPinA, encPinB);
  encoder.clearCount();
  lastStepTime = 0;
}

long StepperMotor::getPosition() {
  long pos = encoder.getCount();
  return invertEncoder ? -pos : pos;
}

void StepperMotor::update() {
  currentPos = getPosition();
  if (currentPos < endPos) {
    digitalWrite(dirPin, HIGH);
    step();
  } else if (currentPos > endPos) {
    digitalWrite(dirPin, LOW);
    step();
  }
}

void StepperMotor::setTarget(long targetPos, long speed) {
  endPos = targetPos;
  stepSpeed = speed;
}

void StepperMotor::step() {
  if (stepSpeed <= 0) return;

  unsigned long now = micros();
  unsigned long delay_us = 1000000UL / stepSpeed;

  if (now - lastStepTime >= delay_us && currentPos < endPos) {
    lastStepTime = now;
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(3);
    digitalWrite(stepPin, LOW);

    stepsTaken++;
  }
}

// Optional blocking movement until target reached
void StepperMotor::moveToBlocking() {
  while (getPosition() != endPos) {
    update();
    delay(1);
  }
}

long StepperMotor::getStepsTaken() {
  return stepsTaken;
}

bool StepperMotor::endPositionReached(){
  return currentPos >= endPos;
}
