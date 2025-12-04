#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>
#include <ESP32Encoder.h>

class StepperMotor {
  public:
    StepperMotor(uint8_t stepPin, uint8_t dirPin, uint8_t encPinA, uint8_t encPinB,
                 long targetPos = 0, long speed = 0, bool invert = false);

    void begin();
    long getPosition();
    void update();
    void setTarget(long targetPos, long speed);
    void moveToBlocking();
    long getStepsTaken();
    bool endPositionReached();

  private:
    uint8_t stepPin, dirPin, encPinA, encPinB;
    bool invertEncoder = false;
    ESP32Encoder encoder;
    unsigned long lastStepTime = 0;
    long currentPos = 0;
    long endPos;
    long stepSpeed; // steps/sec
    long startTime;
    long endTime;
    long stepsTaken = 0;
    void step();
};

#endif
