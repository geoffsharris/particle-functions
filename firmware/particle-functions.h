#ifndef PARTICLE-FUNCTIONS_H
#define PARTICLE-FUNCTIONS_H
#include <application.h>

// used to set time offset from UTC
extern int DST_offset;

// functions that can be called
void particleInit(); // called at setup to set particle.functions and timezone
void setTimezone(int); // used to set time zone

// particle.function declarations
int ghData(String data); // greehouse weather data from indigo
int passProgramParam(String data); // used to set daily program and AM and PM setbacks
int motor(String data); // used to run motor 
int setDST(String data); // called to reset timeZone for DST

#endif
