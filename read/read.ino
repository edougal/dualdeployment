
/*
 * EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it 
 * to the computer.
 * This example code is in the public domain.
 */

#include <EEPROM.h>
#include <EEPROMAnything.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;

struct record_altitudes
{ 
  float max_alt;
  float main_altitude;
  float drogue_altitude;
};


void setup()
{
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
  }
}

void loop()
{
  record_altitudes records = {0,0,0};

  // read a byte from the current address of the EEPROM
  EEPROM_readAnything(0,records);
  
  Serial.print("Max altitude was: \n");
  Serial.print(records.max_alt);
  Serial.print("\n");
  Serial.print("Drogue altitude was: \n");
  Serial.print(records.drogue_altitude);
  Serial.print("\n");
  Serial.print("Main altitude was: \n");
  Serial.print(records.main_altitude);
  Serial.print("\n");

  while(true)
  {
      delay(500);
  }

}
