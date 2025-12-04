#include "Arduino.h"
#include <Ticker.h>
#include "HX711.h"
#include "stepper_motor.h"
#include "n20_motor.h"
#include "pins.h"
#include "constants.h"
#include <ArduinoJson.h>

// ------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------
HX711 scale;

long tare_zero = 0;
long load_cell_force_enc = 0;
double load_cell_force_newtons = 0;

// Default parameters (overridden by JSON)
float target_pos_rot = 720.0;  // deg
float target_pos_ins = 10.0;   // cm
float ins_speed      = 1.0;    // cm/s
float rot_speed      = 60.0;   // deg/s

// Converted units (encoder counts / stepper steps)
long ins_target_enc = 0;
long rot_target_enc = 0;
long ins_speed_steps = 0;
long rot_speed_steps = 0;

// ------------------------------------------------------------
// STEPPER MOTORS (real global motors)
// ------------------------------------------------------------
StepperMotor insertionMotor(
    INS_STEP_PIN, INS_DIR_PIN, INS_ENC_PIN_A, INS_ENC_PIN_B,
    0, 0, true
);

StepperMotor rotationMotor(
    ROT_STEP_PIN, ROT_DIR_PIN, ROT_ENC_PIN_A, ROT_ENC_PIN_B,
    0, 0, true
);

// ------------------------------------------------------------
// N20 MOTORS
// ------------------------------------------------------------
float motorA_deg_s = 60;
float motorB_deg_s = 60;



float motorAPosInc = motorA_deg_s * REV_PER_DEG * COUNTS_PER_REV * S_PER_MS; // counts/ms
float motorBPosInc = motorB_deg_s * REV_PER_DEG * COUNTS_PER_REV * S_PER_MS; // counts/ms

N20Motor motorA(
    MOTOR_A_PIN1, MOTOR_A_PIN2,
    ENC_A_PIN_AW, ENC_A_PIN_BY,
    false
);

N20Motor motorB(
    MOTOR_B_PIN1, MOTOR_B_PIN2,
    ENC_B_PIN_AW, ENC_B_PIN_BY,
    false
);

// ------------------------------------------------------------
// LOAD CELL ROUTINE
// ------------------------------------------------------------
void read_loadcell() {
    if (!scale.is_ready()) return;

    long reading = scale.read();
    load_cell_force_enc = tare_zero - reading;
    load_cell_force_newtons = load_cell_force_enc * ENC_COUNT_TO_NEWTONS;
}

// ------------------------------------------------------------
// JSON INPUT
// ------------------------------------------------------------
bool readJsonFromSerial(String &jsonOut) {
    while (Serial.available() == 0) {
        delay(10);
    }

    jsonOut = Serial.readStringUntil('\n');
    jsonOut.trim();

    // check if tare:
    

    return jsonOut.length() > 0;
}

void parseMessage(const char* payload) {
    StaticJsonDocument<200> doc;
    if (deserializeJson(doc, payload)) {
        Serial.println("ERROR: JSON parse failed");
        return;
    }

    if (doc.containsKey("rotation_angle"))
        target_pos_rot = doc["rotation_angle"];

    if (doc.containsKey("insertion_distance"))
        target_pos_ins = doc["insertion_distance"];

    if (doc.containsKey("insertion_speed"))
        ins_speed = doc["insertion_speed"];

    if (doc.containsKey("rotation_speed"))
        rot_speed = doc["rotation_speed"];


    if (doc.containsKey("roller_speed"))
        motorA_deg_s = doc["roller_speed"];
        motorB_deg_s = doc["roller_speed"];


    Serial.print("time,ins_pos(enc),rot_pos(enc),load(N), roller_pos(enc)\n");
}

// ------------------------------------------------------------
// TIMER ISR
// ------------------------------------------------------------
Ticker timer;

void IRAM_ATTR timerIsr() {
    insertionMotor.update();
    rotationMotor.update();

    motorA.updatePWM();
    motorB.updatePWM();
}

// ------------------------------------------------------------
// SETUP
// ------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(200);

    // ---- Read startup JSON ----
    Serial.println("Waiting for JSON configuration line...");
    String json;
    if (readJsonFromSerial(json)) {
        parseMessage(json.c_str());
        Serial.print("Startup JSON: ");
        Serial.println(json);
    } else {
        Serial.println("No JSON received, using defaults.");
    }

    // ---- Convert user-friendly units → encoder and stepper units ----
    ins_target_enc  = (long)(target_pos_ins * ENC_PER_CM);
    rot_target_enc  = (long)(target_pos_rot * ENC_PER_DEG);

    ins_speed_steps = (long)(ins_speed * CM_TO_STEPS);
    rot_speed_steps = (long)(rot_speed * DEG_TO_STEPS);

    Serial.printf("Final encoder values: %d, %d, %d, %d\n", ins_target_enc, rot_target_enc, ins_speed_steps, rot_speed_steps);
    // RX: Final encoder values: 1700000, 93115, 16617, 27

    // ---- Configure motors ----
    insertionMotor.setTarget(ins_target_enc, ins_speed_steps);
    rotationMotor.setTarget(rot_target_enc, rot_speed_steps);

    insertionMotor.begin();
    rotationMotor.begin();

    motorA.begin();
    motorB.begin();
    motorA.setIncrement(motorAPosInc);
    motorB.setIncrement(motorBPosInc);

    // ---- Load cell ----
    scale.begin(LOADCELL_DAT, LOADCELL_CLK);
    delay(500);

    if (scale.is_ready()) {
        tare_zero = scale.read_average(10);
        Serial.print("Load cell tare = ");
        Serial.println(tare_zero);
    } else {
        Serial.println("Load cell not ready.");
    }

    // ---- Timer ISR every 1ms ----
    timer.attach_ms(1, timerIsr);
    Serial.println("Timer attached");
}

// ------------------------------------------------------------
// LOOP
// ------------------------------------------------------------
void loop() {
    read_loadcell();
    motorA.runMotor();
    motorB.runMotor();

    // Print telemetry every 100 ms (commented out for timing stability)
    static unsigned long lastPrint = 0;
    unsigned long now = millis();
    if (now - lastPrint >= 100) {
        lastPrint = now;
        // Enable this if you need telemetry:
        Serial.printf("%d,%d,%d,%d,%f\n", now, insertionMotor.getPosition(), rotationMotor.getPosition(), load_cell_force_enc, motorA.getPosition());

        
    }
}
