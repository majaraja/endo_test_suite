#ifndef N20_MOTOR_H
#define N20_MOTOR_H

#include <Arduino.h>
#include <ESP32Encoder.h>

constexpr int PwmResolution = 8; 
constexpr int MaxPWM = (1 << PwmResolution) - 1;  
constexpr float Kp_pos = 0.025f;        

class N20Motor {
  public:
    N20Motor(uint8_t pin1, uint8_t pin2, uint8_t encPinA, uint8_t encPinB,
             bool invertEncoder = false);

    void begin();
    float getPosition();
    void setIncrement(float inc);
    void updatePWM();
    void runMotor();
    void printDiagnostics();

  private:
    ESP32Encoder encoder;

    uint8_t pin1, pin2;
    uint8_t encPinA, encPinB;
    bool invertEncoder;
    int pwm = 0;
    float desiredPos = 0;
    float pos = 0;
    float posIncrement = 0;
};

#endif
