# include "particle-functions.h"
# include <application.h>

extern WeatherData weather;

void WeatherData :: message(int data)     // defined outside class definition
{
 Serial.print("Timestamp: "); Serial.println(_weatherTime);
 Serial.print("Temp (F): "); Serial.println(_greenhouseTemp);
 Serial.print("Humidity (%): "); Serial.println(_greenhouseHumidity);
}

void WeatherData :: weatherData()
{
    _weatherTime = 0;
 _greenhouseTemp = 0;
 _greenhouseHumidity = 0;
 _backupGreenhouseTemp = 0;
 _backupGreenhouseHumidity = 0;
 _outsideTemp = 0;
 _outsideHumidity = 0;
  _solar = 0;
 _high = 0;
 _low = 0;
}

void WeatherData :: weatherData(unsigned int weatherTime, int greenhouseTemp,int greenhouseHumidity,
                                int backupGreenhouseTemp, int backupGreenhouseHumidity,int outsideTemp,
                                int outsideHumidity, int solar, int high, int low)
 {
 _weatherTime = weatherTime;
 _greenhouseTemp = greenhouseTemp;
 _greenhouseHumidity = greenhouseHumidity;
 _backupGreenhouseTemp = backupGreenhouseTemp;
 _backupGreenhouseHumidity = backupGreenhouseHumidity;
 _outsideTemp = outsideTemp;
 _outsideHumidity = outsideHumidity;
  _solar = solar;
 _high = high;
 _low = low;
 }


int greenhouseData(String data)
    {


    //Expected parameters in CSV format
    // 1. Unix time stamp
    // 2. current greenhouse temperature
    // 3. current greenhouse humidity
    // 4. backup sensor current greenhouse temperature
    // 5. backup sensor current greenhouse humidity
    // 6. current outside temperature
    // 7. current outside humidity
    // 8. solar radiation
    // 9. forecast high
    // 10. forecast low


    char copyStr[64];
    data.toCharArray(copyStr,64);
    char *p = strtok(copyStr, ",");

    unsigned int weatherTime = atoi(p);
    p = strtok(NULL,",");
    int greenhouseTemp = atoi(p);
    p = strtok(NULL,",");
    int greenhouseHumidity = atoi(p);
    p = strtok(NULL,",");
    int backupGreenhouseTemp = atoi(p);
    p = strtok(NULL,",");
    int backupGreenhouseHumidity = atoi(p);
    p = strtok(NULL,",");
    int outsideTemp = atoi(p);
    p = strtok(NULL,",");
    int outsideHumidity = atoi(p);
    p = strtok(NULL,",");
    int solar = atoi(p);
    p = strtok(NULL,",");
    int high = atoi(p);
    p = strtok(NULL,",");
    int low = atoi(p);

    weather.weatherData(weatherTime, greenhouseTemp, greenhouseHumidity, backupGreenhouseTemp,
                        backupGreenhouseHumidity, outsideTemp, outsideHumidity, solar, high, low);

return 1;
}

int setSeason(String command) // pass CSV AMsetback, PMsetback, dayProgram, nightProgram
{
   char copyStr[64];
    command.toCharArray(copyStr,64);
    char *p = strtok(copyStr, ",");

    AMsetback = atoi(p);
    String AMsetback_string = p;
    p = strtok(NULL,",");
    String PMsetback_string = p;
    PMsetback = atoi(p);
    p = strtok(NULL,",");
    dayProgram = atoi(p);
    p = strtok(NULL,",");
    nightProgram = atoi(p);

    Alarm.free(0);
    Alarm.free(1);
    Alarm.alarmRepeat(AMsetback,0,0, MorningAlarm);
    Alarm.alarmRepeat(PMsetback,0,0, EveningAlarm);
    Spark.publish("AMsetback", AMsetback_string);
    Spark.publish("PMsetback", PMsetback_string);

return 1;
}

void particleInit()
{
 // time function setup
  Time.zone(tzOffset);
  Alarm.alarmRepeat(AMsetback,0,0, MorningAlarm);
  Alarm.alarmRepeat(PMsetback,0,0, EveningAlarm);
  Alarm.alarmRepeat(0,0,0, synchTime);

 // setup particle functions
 Spark.function("ghData", greenhouseData);
 Spark.function("test_call",call_function);
 Spark.function("setSeason", setSeason);
}

// variables used in program
int tzOffset = -7;
int AMsetback = 7;
int PMsetback = 22;
int dayProgram = 1;
int nightProgram = 3;

// functions called by time functions
void MorningAlarm();
{

}
void EveningAlarm();
{

}

// called once per day at midnight to synch photon
void synchTime();
{
 Spark.syncTime();
}
