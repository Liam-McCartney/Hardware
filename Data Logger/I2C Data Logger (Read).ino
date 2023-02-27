#include <Wire.h>

uint32_t address = 3;
uint16_t EEPROM = 0x50;  //Variables Declaration
uint16_t n = 0;

#define length 1170 //Processing window size (- indent)

void setup() {
  Wire.begin();
}

void loop() {
  delay(5000);
  for (uint16_t i = 0; i < 43200; i++) {

    if (!(i % 64)) ++address;
    //Due to an issue in the sketch running on Wed when data was collected,
    //The address prematurely incremented every 64 iterations

    if (address >= 31998) {
      address = 0;
      ++EEPROM;
      //Chip roll-over
    }

    uint8_t dump;
    //Return part of eepromRead is not needed

    eepromRead(EEPROM, address, dump);
    //Read EEPROM

    if (i % (43200 / length) == 0) Serial.println(dump);
    //Determines which dat points to print since they cannot all fit on processing

    address += 3;
    ++n;

    ++address;
  }
  Serial.println("Done");
  while (true)
    ;
}

void eepromRead(uint8_t chipAddress, uint8_t readAddress, uint8_t &returnVar) {
  Wire.beginTransmission(chipAddress);

  Wire.write(readAddress >> 8);
  Wire.write(readAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(chipAddress, 1);
  while (!Wire.available())
    ;
  int dataRead = Wire.read();

  returnVar = dataRead;
}
