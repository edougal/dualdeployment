#include <Wire.h>
#include <Adafruit_MPL115A2.h>
#include <EEPROMAnything.h>

Adafruit_MPL115A2 mpl115a2;
float BASEALTITUDE = 0;
float altitude;
float pressureKPA = 0, temperatureC = 0;    
float static sealevelPressure = 101.325;
float avg = 0;
float cumulative = 0;
float last_val = 0;
float val = 0;

int state = 1;

int falling_count = 0;
float next_val = 0;

struct record_altitudes
{ 
  float main_altitude;
  float drogue_altitude;
};

record_altitudes records = {0,0};

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Getting altitude...");
  mpl115a2.begin();
  
   
  for(int i = 0; i < 1000; i++)
  {
    float val = get_altitude();
  }
  
  for(int i = 0; i < 100; i++)
  {
    float val = get_altitude();
    cumulative = val + cumulative;
  }

  
  BASEALTITUDE = cumulative/100; 
  Serial.print("Base Altitude is: ");
  Serial.println(BASEALTITUDE);
}  

float get_altitude(){
  float val = 0;
  mpl115a2.getPT(&pressureKPA,&temperatureC);
  val = (44330 * (1.0 - pow(pressureKPA/sealevelPressure,0.1903))*3.28084) - BASEALTITUDE; // altitude in feet
  return val;
}



void loop(void) 
{

 //  mpl115a2.getPT(&pressureKPA,&temperatureC);
 // Serial.print("Pressure (kPa): "); Serial.print(pressureKPA, 4); Serial.print(" kPa  ");
 // Serial.print("Temp (*C): "); Serial.print(temperatureC, 1); Serial.println(" *C both measured together");
  
  val = get_altitude();
  val = last_val + (1/32)*(val - last_val);
  
  Serial.print("Average is:");
  Serial.println(val);

switch(state) 
{
  case 1:
    if (val > 500)
    {
      state = 2;
      Serial.println("ARMED");
    }
  break;  
  
  case 2:
    if (val < last_val)
    {
      falling_count ++;
      if (falling_count == 3)
      {
       // LAUNCH drogue
        Serial.print("Drogue Launched!");
        Serial.print("time: ");
        Serial.println(millis());
        records.drogue_altitude = altitude;
        state = 3;
      }
    }
    else
    {
      falling_count = 0;
    }
  break;
  
 case 3:
   if (val < 500)
   {
     // LAUNCH main    
     records.main_altitude = altitude;
     state = 4;
     Serial.print("Main Launched!");
     Serial.print("time: ");
     Serial.println(millis());
     EEPROM_writeAnything(0,records);
   }
   break;
   
 case 4:
   delay(10);
 break;
 }

  last_val = val;

}
