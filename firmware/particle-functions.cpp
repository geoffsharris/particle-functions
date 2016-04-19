#include "TimeAlarms/TimeAlarms.h"
#include "particle-functions.h"
#include "weather/weather.h"
#include "DualMC33926MotorShield/DualMC33926MotorShield.h"


// default startup values
int DST_offset = -7;
int AMsetback = 7;
int PMsetback = 15;
int dayProgram = 2;
int nightProgram = 4;
int targetVentPosition = 0;
int currentVentPosition;
int liveProgram [5][2];
int motorStatus;
int ventdiff; // used for proxy of motor run time
int magnetSensor = D4;

void particleInit()
{
    Spark.function ("ghData" , ghData); //used to pass greenhouse/weather data from HA system 
    Spark.function ("passProgram" , passProgramParam);  //used to pass program parameters from HA system 
    Spark.function ("motor" , runMotor); //used to test motor
    Spark.function ("offsetUTC" , offsetUTC); //used to pass UTC offset parameter from HA system 
    
    setTimezone(DST_offset); // set default time zone offset
    Alarm.alarmRepeat(0,0,0, synchTime); // synch time every 24hrs at midnight
    Alarm.timerRepeat(0,1,0, runProgram);
    setAlarms(AMsetback, PMsetback); // set initial setback times
    setProgramSeason();
    initializeDefaultSetbacks();
    pinMode(magnetSensor, INPUT);
}

int ghData(String data)
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

    weatherstation.weatherData(weatherTime, greenhouseTemp, greenhouseHumidity, backupGreenhouseTemp,
                        backupGreenhouseHumidity, outsideTemp, outsideHumidity, solar, high, low);
                        
                        
                        
    Serial.println("data loaded to greenhouseData: "); Serial.println(data);
   

return 1;
}
int passProgramParam(String data)
{
   char copyStr[64];
    data.toCharArray(copyStr,64);
    char *p = strtok(copyStr, ",");

    int AMsetback = atoi(p);
    p = strtok(NULL,",");
    int PMsetback = atoi(p);
    p = strtok(NULL,",");
    dayProgram = atoi(p);
    p = strtok(NULL,",");
    nightProgram = atoi(p);
    
   
    Serial.print("data loaded to setProgram: "); Serial.println(data); // used for testing remove later
    setAlarms(AMsetback, PMsetback); // pass setback values to function

return 1;
}

int runMotor(String data)
{
    Serial.print("motor function called: "); Serial.println(data); // used for testing remove later
    char copyStr[64];
    data.toCharArray(copyStr,64);
    int pass_param = atoi(copyStr); // converts character array into int, subtracts 1 to get index # of program row from programParam array
    //char * param = new char[5]; // creates character array to store program variable from RESTful call
    //strcpy(param, data.c_str()); // copies program variable into character array
    //int pass_param = atoi(param); // converts character array into int, subtracts 1 to get index # of program row from programParam array
        
    motor.setM1Speed(pass_param);
    
    
    return 1;
}

int offsetUTC(String data)
{
    char copyStr[64];
    data.toCharArray(copyStr,64);
    int DST_offset = atoi(copyStr);
    if (DST_offset >= -12 && DST_offset <= 12)
    {
        setTimezone(DST_offset); // call to reset timezone
        Serial.println("offsetUTC function called"); // used for testing remove later
        Serial.println(DST_offset); // used for testing remove later
    return 1;
    }
    else
    {
         Serial.println("offsetUTC function error"); // used for testing remove later
          return 0;
    }
    
}

void setTimezone(int offset) // call to set timezone offset from UTC
{
Time.zone(offset);
 Serial.print("Time.zone(offset) called TZ: "); Serial.println(now()); // used for testing remove later
 
}

void setAlarms(int AMsetback, int PMsetback)
{
    if  ((AMsetback >= 0 && AMsetback < 24) && (PMsetback >= 0 && PMsetback < 24))
    {
        
    
    if (Alarm.isAllocated(2))
        {
            Alarm.free(2);
            Alarm.free(3);
        }
        Alarm.alarmRepeat(AMsetback,0,0, MorningAlarm);  // 8:30am every day
        Alarm.alarmRepeat(PMsetback,0,0,EveningAlarm);
        Serial.println("setAlarms code called");
    }
    else
        {
        Serial.println("setAlarms code error");
        }
}

void MorningAlarm()
{
    Serial.println("Morning alarm code called");
    ventProgram(dayProgram);
}
void EveningAlarm()
{
    Serial.println("Evening alarm code called");
     ventProgram(nightProgram);
}

void synchTime()
{
    Spark.syncTime();
    setProgramSeason();
}

//--------------------------//

int  ventProgram(int passedProgram)
// call once per hour on the hour
// the program number is passed from setTimePeriod and the coresponding 
// temperature,vent position combos are loaded into an array
{  
    
    if (passedProgram == 1)
    {
        // warm day program July 1 - Oct 5
        int program[5][2] =
        { // program 0
            {85,5},
            {82,4},
            {79,3},
            {77,2},
            {76,0}
        };
        for (int i=0; i<5; i++)
            {
            Serial.print("Temp: ");Serial.print(program[i][0]); Serial.print(" Vent: ");Serial.println(program[i][1]);
            liveProgram[i][0] = program[i][0];
            liveProgram[i][1] = program[i][1];
            
            }
       
      
    }
    else if (passedProgram == 2)
    {
        
        // normal day program Oct 6 - June 30
        int program[5][2]=
        { // program 1
            {85,5},
            {83,4},
            {81,2},
            {79,1},
            {77,0}
        }; 
        for (int i=0; i<5; i++)
            {
            Serial.print("Temp: ");Serial.print(program[i][0]); Serial.print(" Vent: ");Serial.println(program[i][1]);
            liveProgram[i][0] = program[i][0];
            liveProgram[i][1] = program[i][1];
            
            }
       
    }
    
    else if (passedProgram == 3)
    {
        // warm nights program July 1 - Oct 5
        
        int program[5][2]=
        { // program 3
            {67,5},
            {65,4},
            {64,2},
            {63,0},
            {63,0}
        };
        for (int i=0; i<5; i++)
            {
            Serial.print("Temp: ");Serial.print(program[i][0]); Serial.print(" Vent: ");Serial.println(program[i][1]);
            liveProgram[i][0] = program[i][0];
            liveProgram[i][1] = program[i][1];
            
            }
       
    }
    else if (passedProgram == 4)
    {
        // normal nights program Oct 6 - June 30
        int program[5][2]=
        { // program 2
            {75,5},
            {73,4},
            {71,3},
            {69,2},
            {67,0}
        };
        for (int i=0; i<5; i++)
            {
            Serial.print("Temp: ");Serial.print(program[i][0]); Serial.print(" Vent: ");Serial.println(program[i][1]);
            liveProgram[i][0] = program[i][0];
            liveProgram[i][1] = program[i][1];
            
            }
     
    }
    
       
        return 1;
}

void runProgram()
{
    for (int i=0; i<5; i++)
            {
            Serial.print("Temp: ");Serial.print(liveProgram[i][0]); Serial.print(" Vent: ");Serial.println(liveProgram[i][1]);
           
            }
    int gh_temperature = weather.greenhouseTemp();
     Serial.print("gh_temp: "); Serial.println(gh_temperature);
   if (gh_temperature >= liveProgram[0][0])
    {
        Serial.print("runprogram >= triggered comparator: ");Serial.println(liveProgram[0][0]);
        targetVentPosition = liveProgram[0][1];
    }
    else if (gh_temperature <= liveProgram[4][0])
        {
         Serial.print("runprogram <= triggered comparator: ");Serial.println(liveProgram[4][0]);
        targetVentPosition = liveProgram[4][1];
        }
    else
    {
    for (int i=1; i<4; i++){
        if (gh_temperature <= liveProgram[i][0] && gh_temperature > liveProgram[i+1][0])
            {
                Serial.print("runprogram == triggered comparator: ");Serial.println(liveProgram[i][0]);
                targetVentPosition = liveProgram[i][1];
                break;
            }
        }
        
    }
    Serial.print("vent position : ");Serial.println(targetVentPosition);
} 

//--------------------------
// used to set program parameters based upon date and time 


void setProgramSeason()
{
    if (Time.month() < 4 || Time.month() > 11 || (Time.month() == 11 && Time.day() >9)) // wintertime setbacks
    {
        AMsetback = 7;
        PMsetback = 23;
        dayProgram = 2;
        nightProgram = 4;
        Serial.println("wintertime program");
    }
    else if ((Time.month() > 6 && Time.month() < 10) || (Time.month() == 10 && Time.day() < 6)) // summertime setbacks
    {
        AMsetback = 10;
        PMsetback = 15;
        dayProgram = 1;
        nightProgram = 3;
        Serial.println("summertime program");
        
    }
    else
    {
        AMsetback = 9;
        PMsetback = 20;
        dayProgram = 2;
        nightProgram = 4;
        Serial.println("spring/fall program");
    }
}   
void initializeDefaultSetbacks()
{
 if (Time.hour() >= PMsetback ||  Time.hour() < AMsetback)
    {
        ventProgram(nightProgram);
        Serial.println("vents set to night program");
    }
    else
    {
        ventProgram(dayProgram);
        Serial.println("vents set to day program");
    }
}   

//---------------------------   
//motor functions here

void ventMotor(int currentVentPosition, int targetVentPosition)
{
    if (currentVentPosition == targetVentPosition)
    {
        motor.setM1Speed(0);
        motorStatus = 0;
    }
    else if (currentVentPosition > targetVentPosition)
    {
        motor.setM1Speed(-255);
        motorStatus = 1;
        Alarm.timerOnce(10*(currentVentPosition-targetVentPosition), tempVentIncrementDown);
        ventdiff = currentVentPosition-targetVentPosition;
    }
    else if (currentVentPosition < targetVentPosition)
    {
        motor.setM1Speed(255);
        motorStatus = 1;
        Alarm.timerOnce(10*(targetVentPosition-currentVentPosition), tempVentIncrementUp);
        ventdiff = targetVentPosition-currentVentPosition;
    }
    else
    Serial.println("ventMotor error");
}

// write function to set motor back to position zero at startup

// write function to check if torque is too high, stop motor and throw error

// write funcion to sense  position 0 and 5 to stop motor and throw error


void tempVentIncrementUp()
{
    currentVentPosition =  currentVentPosition + ventdiff;
    Serial.println("ventMotor run forward for "); Serial.print(ventdiff); Serial.println(" positions");
}

void tempVentIncrementDown()
{
    currentVentPosition = currentVentPosition - ventdiff;
    Serial.print("ventMotor run backward for "); Serial.print(ventdiff); Serial.println(" positions");
}



