// This #include statement was automatically added by the Particle IDE.
#include "particle-functions.h"

// This #include statement was automatically added by the Particle IDE.
#include "DualMC33926MotorShield/DualMC33926MotorShield.h"

// This #include statement was automatically added by the Particle IDE.
#include "TimeAlarms/TimeAlarms.h"

// This #include statement was automatically added by the Particle IDE.
#include "weather/weather.h"

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL)); //Use on photon

extern int targetVentPosition;
extern int currentVentPosition;
extern int motorStatus;

void setup() {
particleInit();
motor.init();

}

void loop() {
Alarm.delay(1000);
    while (targetVentPosition != currentVentPosition && motorStatus == 0) // turn on motor when positions are different
    {
        ventMotor(currentVentPosition, targetVentPosition);
        Serial.println("run motor called to move vent");
    }
    while (targetVentPosition == currentVentPosition && motorStatus == 1) // turn off motor when position is reached
    {
        ventMotor(currentVentPosition, targetVentPosition);
         Serial.println("run motor called to stop motor");
    }
}
