

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345.h>
#include <Adafruit_MPL115A2.h>

#include <SD.h>

// #include <Adafruit_GPS.h>
// #include <SoftwareSerial.h>

//SoftwareSerial mySerial(8, 7);
//Adafruit_GPS GPS(&mySerial);

Adafruit_ADXL345 accel = Adafruit_ADXL345(12345);
Adafruit_MPL115A2 mpl115a2;

File logfile;

void setup() 
{
  Serial.begin(9600);
  Serial.println("Starting sensors...");

  mpl115a2.begin();
  
  if(!accel.begin())
  {
//    Serial.println("Accelerometer failure");
    while(1);
  }

  accel.setRange(ADXL345_RANGE_16_G);

  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,INPUT);
  pinMode(13,OUTPUT);

  if (!SD.begin(10,11,12,13))
    {
      Serial.println("Card failure");
      while(1);      
    }
    
  char filename[15];
  strcpy(filename,"Log00.TXT");

  for (uint8_t i = 0; i < 100; i++)
    {
      filename[3] = '0' + i/10;
      filename[4] = '0' + i%10;      
      if(!SD.exists(filename))
        {
          break;
        }      
    }

  logfile = SD.open(filename,FILE_WRITE);

  if(! logfile)
    {
      Serial.print("failed to create file: "); Serial.println(filename);
      while(1);
    }


  Serial.print("Writing to: "); Serial.println(filename);

  logfile.print("Time "); logfile.print("Xaccel ");
  logfile.print("Yaccel "); logfile.print("Zaccel ");
  logfile.print("temperature "); logfile.println("pressure ");  
  logfile.flush();
  
}

void loop()
{
  float pressureKPA = 0, temperatureC = 0, xAccel = 0, yAccel = 0, zAccel = 0;    
  sensors_event_t event; 
    
  accel.getEvent(&event);
  mpl115a2.getPT(&pressureKPA,&temperatureC); 

  logfile.print(millis()); logfile.print("  "); logfile.print(event.acceleration.x); logfile.print("  ");
  logfile.print(event.acceleration.y); logfile.print("  "); logfile.print(event.acceleration.z); logfile.print("  ");
  logfile.print(temperatureC); logfile.print("  "); logfile.println(pressureKPA);  
  logfile.flush();

}



