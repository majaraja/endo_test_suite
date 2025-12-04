#include "n20_motor.h"

N20Motor::N20Motor(uint8_t pin1, uint8_t pin2, uint8_t encPinA, uint8_t encPinB,
                   bool invertEncoder)
: pin1(pin1), pin2(pin2),
  encPinA(encPinA), encPinB(encPinB),
  invertEncoder(invertEncoder)
{}

void N20Motor::begin() {
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);

    encoder.attachFullQuad(encPinA, encPinB);
    encoder.clearCount();
}

float N20Motor::getPosition() {
    pos = (float)encoder.getCount();
    return invertEncoder ? -pos : pos;
}

void N20Motor::setIncrement(float inc) {
    posIncrement = inc;
}

void N20Motor::updatePWM() {
    // Update target
    desiredPos += posIncrement;

    // Compute error
    long error = desiredPos - getPosition();

    // Proportional control
    float motorCmd = Kp_pos * error;

    // Bound to actual PWM range
    motorCmd = constrain(motorCmd, -1.0f, 1.0f);

    pwm = (int)(motorCmd * MaxPWM);
}

void N20Motor::runMotor() {
    pwm = constrain(pwm, -MaxPWM, MaxPWM);

    if (pwm < 0) {
        analogWrite(pin1, 0);
        analogWrite(pin2, -pwm);
    } else {
        analogWrite(pin1, pwm);
        analogWrite(pin2, 0);
    }
}

void N20Motor::printDiagnostics(){
    Serial.print("pos: ");
    Serial.print(pos);
    Serial.print("  desiredPos: ");
    Serial.print(desiredPos);
    Serial.print("  pwm: ");
    Serial.println(pwm);
}
