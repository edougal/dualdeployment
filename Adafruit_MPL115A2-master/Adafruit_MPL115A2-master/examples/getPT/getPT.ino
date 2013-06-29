#include <Wire.h>
#include <Adafruit_MPL115A2.h>

Adafruit_MPL115A2 mpl115a2;
float BASEALTITUDE = 0;
float altitude;
float pressureKPA = 0, temperatureC = 0;    
float static sealevelPressure = 101.325;
float measurements[5] = {0,0,0,0,0};
float avg = 0;
float cum = 0;
float last_val = 0;
int falling_count = 0;
boolean armed = false;

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
    cum = val + cum;
  }

  
  BASEALTITUDE = cum/100; 
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
  cum = 0;
  
  for(int i = 0; i < 50; i++)
  {
    float val = get_altitude();
    cum = val + cum;
  }

  avg = cum/50;
  
  if (avg > 500 && armed == false)
  {
    armed = true;
    Serial.println("ARMED");
  }
  
  Serial.print("Average is:");
  Serial.println(avg);
  if (avg < last_val)
    { 
      falling_count ++;
      if (falling_count == 3 && armed == true)
      {
        Serial.print("Parachute Launched!");
        Serial.print("Stopped at time: ");
        Serial.print(millis());
        while (true)
        {
          delay(10);
        }
      }
  }
 else
 {
   falling_count = 0;
 }
  Serial.print("Falling Count: ");
  Serial.println(falling_count);
  last_val = avg;
 
}
