#include "particle-functions.h"

int DST_offset = -7;

void particleInit()
{
    Particle.function ("ghData" , ghData);
    Particle.function ("passProgram" , passProgramParam);
    Particle.function ("motor" , motor);
    Particle.function ("dst" , setDST);
    setTimezone();
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

    /*weather.weatherData(weatherTime, greenhouseTemp, greenhouseHumidity, backupGreenhouseTemp,
                        backupGreenhouseHumidity, outsideTemp, outsideHumidity, solar, high, low);
                        */
                        
                        
    Serial.println("data loaded to greenhouseData");
    Serial.println(data);

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
    int dayProgram = atoi(p);
    p = strtok(NULL,",");
    int nightProgram = atoi(p);
    
    Serial.println("data loaded to setProgram");
    Serial.println(data);

return 1;
}

int motor(String data)
{
    Serial.println("motor function called");
    char copyStr[64];
    data.toCharArray(copyStr,64);
    int pass_param = atoi(copyStr); // converts character array into int, subtracts 1 to get index # of program row from programParam array
    //char * param = new char[5]; // creates character array to store program variable from RESTful call
    //strcpy(param, data.c_str()); // copies program variable into character array
    //int pass_param = atoi(param); // converts character array into int, subtracts 1 to get index # of program row from programParam array
        
    //motor.setM1Speed(pass_param);
    Serial.println("data loaded to Test Function");
    Serial.println(data);
    return 1;
}

int setDST(String data)
{
    char copyStr[64];
    data.toCharArray(copyStr,64);
    DST_offset = atoi(copyStr);
    Serial.println("dst function called");
     Serial.println(DST_offset);
}

void setTimezone() // call to set timezone offset from UTC
{
Time.zone(DST_offset);
}
