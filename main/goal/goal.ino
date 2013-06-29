#include <Wire.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <Adafruit_MPL115A2.h>

Adafruit_MPL115A2 mpl115a2;

float pressureKPA = 0, temperatureC = 0;    
float BASEALTITUDE = 0;
float static sealevelPressure = 101.325;

float altitude = 0;      // variables for loop
float uncompensated_alt = 0;
float prev_alt = 0;

struct record_altitudes
{ 
  float max_alt;
  float main_altitude;
  float drogue_altitude;
};

int falling_count = 0;
int state = 1;
boolean armed = false;

int drogue = 6;    // pin controlling mosfet connected to drogue igniter
int main = 7;

record_altitudes records = {0,0,0};

float get_altitude(){
  float val = 0;
  mpl115a2.getPT(&pressureKPA,&temperatureC);
  val = (44330 * (1.0 - pow(pressureKPA/sealevelPressure,0.1903))*3.28084) - BASEALTITUDE; // altitude in feet
  return val;
}

void setup(void) 
{
  mpl115a2.begin();
  mpl115a2.getPT(&pressureKPA,&temperatureC);
  BASEALTITUDE = (44330 * (1.0 - pow(pressureKPA/sealevelPressure,0.1903))*3.28084); // altitude in feet


  delay(30000);            // 30 second delay

  pinMode(drogue, OUTPUT);
  pinMode(main, OUTPUT);

}

void loop(void) 
{

  switch(state) {
    
    case 1:      // low altitude flight
    altitude = get_altitude();
    if (altitude > 300)
    {
      state = 2;
      armed = true;    
      break;
    }
    
    case 2:
    altitude = get_altitude();
    if (altitude > records.max_alt)
    {records.max_alt = altitude;}
    if (prev_alt > altitude)
    {
      falling_count = falling_count + 1;
      if (falling_count == 10)
      {
        falling_count = 0;
        state = 3;
        digitalWrite(drogue,HIGH);
        records.drogue_altitude = altitude;
      }
    }
    else
    {falling_count = 0;}
    prev_alt = altitude;
    break;
  
    case 3:
    altitude = get_altitude();
    if (altitude < 500)
    {
      falling_count = falling_count + 1;
      if (falling_count == 10)
      {
        state = 4;   
        digitalWrite(main,HIGH);
        records.main_altitude = altitude;
      }
    }
    else
      {falling_count = 0;}
    break;
    
    case 4:
    EEPROM_writeAnything(0,records);
    state = 5;
    break;

    default:
     delay(1); 
  }

delay(1);

}
