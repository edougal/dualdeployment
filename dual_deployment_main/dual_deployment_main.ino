

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
unsigned long prev_time;  
float prev_alt, prev_vel, prev_accel;


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
  logfile.print("Temperature "); logfile.print("Pressure ");  
  logfile.print("Altitude ");
  logfile.flush();

/* Initializing values for Kalman Filter */

  prev_time = millis(); // need the time to determine the length of the time-step
  // begin by initialzing prev values
  float pressureKPA = 0, temperatureC = 0;
  mpl115a2.getPT(&pressureKPA,&temperatureC);   
  prev_alt = (pow((pressureKPA/101.325),(1/5.25588)) - 1)/(-.0000225577);  // altitude sensor
  prev_vel = 0;
  prev_accel = 0; // acceleration sensor 

}

void loop()
{
  float pressureKPA = 0, temperatureC = 0, xAccel = 0, yAccel = 0, zAccel = 0, time_step; 
  float temp_alt, temp_vel;
  sensors_event_t event; 
  unsigned long next_time;

  // calculate timestep  
  next_time = millis();
  time_step = .001*(next_time-prev_time);

  // propagate state forward in time
  temp_alt = prev_alt + prev_vel*time_step + prev_accel*.5*time_step^2;
  temp_vel = prev_vel + prev_accel*time_step;
  
  // gets sensor data
  
  accel.getEvent(&event);
  mpl115a2.getPT(&pressureKPA,&temperatureC); 
  accel = sqrt(xAccel^2 + yAccel^2 + zAccel^2);
  alt = (pow((pressure/101.325),(1/5.25588)) - 1)/(-.0000225577);  
  // corrects estimates with new data

  next_alt = temp_alt + .0330*(alt - temp_alt)
  next_vel = temp_vel + .0193*(alt - temp_alt) + .0001*(accel - prev_accel); // use this value for your variometer
  next_accel = prev_accel + .9988*(accel - prev_accel); 

  //prepares for next iteration
    
  prev_alt = next_alt;
  prev_vel = next_vel;
  prev_accel = next_accel;  
  prev_time = next_time; 
    

  logfile.print(millis()); logfile.print("  "); logfile.print(event.acceleration.x); logfile.print("  ");
  logfile.print(event.acceleration.y); logfile.print("  "); logfile.print(event.acceleration.z); logfile.print("  ");
  logfile.print(temperatureC); logfile.print("  "); logfile.print(pressureKPA);
  logfile.println(prev_alt);  
  logfile.flush();

}



