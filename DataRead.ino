#include <Wire.h>

uint8_t address = 3;
uint8_t EEPROM = 0x50;

void setup() {
  //delay(10000);
  Serial.begin(10000);
  Wire.begin();
  uint8_t dump;

  //eepromRead(EEPROM, 0, dump);
  //eepromRead(EEPROM, 1, dump);
  //eepromRead(EEPROM, 2, dump);
}


uint16_t n = 0;
void loop() {
  delay(5000);
  for (uint16_t i = 0; i < 43200; i++) {

    if (!(i % 64)) ++address;

    if (address >= 31998) {
      address = 0;
      ++EEPROM;
    }
    uint8_t dump;

    eepromRead(EEPROM, address, dump);
    if (dump < 30 && dump > 0 && i % 15 == 0) Serial.println(dump);
    address += 3;
    ++n;

    ++address;
  }
  Serial.println("done");
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
