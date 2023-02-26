// PROJECT  :WireLevelRTC
// PURPOSE  :Part of ICS3U-E's Wire Level I2C Introduction
// AUTHOR   :C. D'Arcy
// DATE     :2018 04 24. 2020 02 08. 2023 01 27.
// uC       :328p     //use TinyWire.h for 84 & 85
// REFERENCE:http://darcy.rsgc.on.ca/ACES/TEI3M/WireLevelRTCExercises.html#time

#include <Wire.h>
char firstDigitArray[256], secondDigitArray[256];
unsigned char x, y;
unsigned int z;

struct timeDate {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t date;
  uint8_t mon;
  uint8_t YEAR;
};

#define RTCADDRESS 0x68
#define completePin 8

#define TC74ADDRESS 0x4D  //0x4D(B100_1101 or 0x48(B100_1000)
#define DURATION 0        //pacing


uint8_t dump;
//if eepromRead is being used to print and no return is needed then 'dump' is set the the output

boolean triggered = false;

char buff[40];

boolean STOP = false;

uint8_t EEPROM = 0x50;

uint32_t interruptCount = 0;

uint16_t address = 0x03;

uint16_t timeStamp = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("DS1307RTC Read Test");
  Serial.println("~~~~~~~~~~~~~~~~~~~");

  pinMode(A2, OUTPUT);
  pinMode(A0, OUTPUT);

  digitalWrite(A2, 0);
  digitalWrite(A0, 1);

  uint8_t state = digitalRead(A1);
  while (digitalRead(A1) == state)
    ;

  attachInterrupt(digitalPinToInterrupt(3), increment, CHANGE);
  pinMode(completePin, OUTPUT);

  Wire.begin();
  config_lcd();

  Wire.beginTransmission(RTCADDRESS);
  Wire.write(0x07);
  Wire.write(0b10010000);
  Wire.endTransmission();

  Wire.beginTransmission(RTCADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(RTCADDRESS, 7);
  while (!Wire.available())
    ;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t date;
  uint8_t mon;
  uint8_t YEAR;
  getAll(hours, minutes, seconds, date, mon, YEAR);
  sprintf(buff, "Time = %02d:%02d:%02d,", bcd2dec(hours), bcd2dec(minutes), bcd2dec(seconds));
  Serial.print(buff);

  delay(1000);

  eepromWrite(EEPROM, 0x00, bcd2dec(hours));
  eepromWrite(EEPROM, 0x01, bcd2dec(minutes));
  eepromWrite(EEPROM, 0x02, bcd2dec(seconds));
}

uint8_t bcd2dec(uint8_t bcd) {
  return 10 * (bcd >> 4) + (bcd & 0xF);
}

void getAll(uint8_t &ho, uint8_t &mi, uint8_t &se, uint8_t &da, uint8_t &mo, uint8_t &ye) {
  se = Wire.read();
  mi = Wire.read();
  ho = Wire.read();
  Wire.read();  // ignore DAY
  da = Wire.read();
  mo = Wire.read();
  ye = Wire.read();
}

void loop() {
  if (!STOP && (interruptCount % 4) == 0) {

    boolean startRead = digitalRead(3);

    if (address == 31998) {
      address = 0;
      EEPROM++;
    }

    Wire.requestFrom(TC74ADDRESS, 1);  // ...request number of bytes from register
    while (Wire.available() == 0)
      ;                            // pause & wait for the data to be sent
    int8_t celsius = Wire.read();  // read the byte(s) returned on the I2C bus

    float displayTemp;
    digitalRead(A1) ? displayTemp = celsius : displayTemp = celsius * 1.8 + 32;
    int8_t displayTempInt = int8_t(displayTemp);

    display(displayTempInt);
    /*
    eepromWrite(EEPROM, address, celsius);
    Serial.print("Temp: ");
    eepromRead(EEPROM, address, dump);
    address++;
    Serial.println("-----------------");
    Serial.println("Timestamps");

    eepromWrite(EEPROM, address, timeStamp >> 8);
    uint8_t highBit;
    Serial.print("HighBit: ");
    eepromRead(EEPROM, address, highBit);
    address++;

    eepromWrite(EEPROM, address, timeStamp & 0xFF);
    uint8_t lowBit;
    Serial.print("LowBit: ");
    eepromRead(EEPROM, address, lowBit);

    uint16_t collatedStamp = (highBit << 8) | lowBit;
    Serial.print("TimeStamp: ");
    Serial.println(collatedStamp);

    if (collatedStamp >= 43200) STOP = ~STOP;

    address++;
    timeStamp++;

    Serial.println("-----------------");
    Serial.print("EEPROM Chip: ");
    Serial.println(EEPROM - 0x50 + 1);
    Serial.print("Address: ");
    Serial.println(address);
    Serial.println("-----------------");
    while (digitalRead(3) == startRead)
      //Serial.println("In Loop");
      ;*/

    delay(DURATION);
  } else if (STOP) {
    Serial.println("Readings Complete!");
    Serial.println("You May Now Run ReadData.ino");
    digitalWrite(completePin, 1);
    while (true)
      ;
  }
}

void eepromWrite(uint8_t chipAddress, uint16_t writeAddress, uint8_t writeData) {
  Wire.beginTransmission(chipAddress);

  Wire.write(writeAddress >> 8);    //Top 8
  Wire.write(writeAddress & 0xFF);  //Bottom 8

  Wire.write(writeData);   //Write Data
  Wire.endTransmission();  //Bye!

  delay(5);  //Let the chip finish its write cycle
}

void eepromRead(uint8_t chipAddress, uint16_t readAddress, uint8_t &returnVar) {
  Wire.beginTransmission(chipAddress);

  Wire.write(readAddress >> 8);
  Wire.write(readAddress & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(chipAddress, 1);
  while (!Wire.available())
    ;
  int dataRead = Wire.read();

  returnVar = dataRead;

  Serial.println(dataRead);
}

void increment() {
  interruptCount++;
  triggered = true;
}

void send_command(unsigned char cmd) {
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(cmd);
  Wire.endTransmission();
}

void config_lcd() {
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0xae);
  Wire.write(0xd5);
  Wire.write(0x80);
  Wire.write(0xa8);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0x1f);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0xd3);
  Wire.write(0x00);
  Wire.write(0x40);
  Wire.write(0x8d);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0x14);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0x20);
  Wire.write(0x00);
  Wire.write(0xa1);
  Wire.write(0xc8);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0xda);
  Wire.write(0x02);
  Wire.write(0x81);
  Wire.write(0x8f);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0xd9);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0xf1);
  Wire.endTransmission();
  //
  Wire.beginTransmission(0x3c);
  Wire.write(0x00);
  Wire.write(0xdb);
  Wire.write(0x40);
  Wire.write(0xa4);
  Wire.write(0xa6);
  Wire.write(0x2e);
  Wire.write(0xaf);
  Wire.endTransmission();
}

void display(int temp) {

  void (*setDigitFunctions[10])(int) = { setZero, setOne, setTwo, setThree, setFour, setFive, setSix, setSeven, setEight, setNine };

  int firstDigit = temp / 10;
  int secondDigit = temp - firstDigit * 10;

  for (int i = 0; i < 10; i++) {
    if (firstDigit == i || secondDigit == i) {
      if (firstDigit == i) setDigitFunctions[i](1);
      if (secondDigit == i) setDigitFunctions[i](2);
    }
  }

  char data[512];
  for (int i = 0; i < 256; i++) data[i + 256] = firstDigitArray[i];
  for (int i = 0; i < 256; i++) data[i] = secondDigitArray[i];

  z = 0;
  for (x = 0; x < 32; ++x) {
    Wire.beginTransmission(0x3c);
    Wire.write(0x40);
    for (y = 0; y < 16; ++y) {
      Wire.write(data[z]);
      ++z;
    }
    Wire.endTransmission();  // stop transmitting
  }
}

void setZero(uint8_t placeValue) {
  Serial.println("Setting 0");
  char zero[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf8, 0x3c, 0x1c, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e,
      0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x1c, 0x3c, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x3c, 0x38, 0x38, 0x70, 0x70, 0x70, 0x70, 0x70,
      0x70, 0x70, 0x70, 0x70, 0x30, 0x38, 0x38, 0x1e, 0x1f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = zero[i] : secondDigitArray[i] = zero[i];
}

void setOne(uint8_t placeValue) {
  Serial.println("Setting 1");
  char one[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
      0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x1f, 0x1e, 0x18, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = one[i] : secondDigitArray[i] = one[i];
}

void setTwo(uint8_t placeValue) {
  Serial.println("Setting 2");
  char two[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0xf8, 0x3c, 0x1c, 0x0c, 0x0c, 0x1c, 0x1c, 0x3c, 0x78,
      0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1f, 0x3f, 0x78, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x03, 0x07, 0x0f, 0x1e, 0x38, 0x78, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = two[i] : secondDigitArray[i] = two[i];
}

void setThree(uint8_t placeValue) {
  Serial.println("Setting 3");
  char three[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0xf8, 0x3c, 0x1c, 0x1c, 0x1c, 0x1c, 0xf8, 0xf0, 0xf8,
      0x3c, 0x1e, 0x0e, 0x0e, 0x0e, 0x0e, 0x1e, 0x3c, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x3c, 0x38, 0x30, 0x00, 0x00, 0x00, 0x03, 0x03,
      0x00, 0x00, 0x00, 0x00, 0x30, 0x70, 0x78, 0x3c, 0x1f, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = three[i] : secondDigitArray[i] = three[i];
}

void setFour(uint8_t placeValue) {
  Serial.println("Setting 4");
  char four[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x78, 0xf8, 0xf8, 0xf8, 0xb8, 0x38, 0x38, 0x38, 0x38,
      0x38, 0x38, 0x38, 0xfe, 0xfe, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07, 0x0f, 0x1e, 0x1c,
      0x38, 0x70, 0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = four[i] : secondDigitArray[i] = four[i];
}

void setFive(uint8_t placeValue) {
  Serial.println("Setting 5");
  char five[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf8, 0xfc, 0x3c,
      0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x1c, 0x3c, 0xf8, 0xf0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x1f, 0x1c, 0x1c, 0x18, 0x38, 0x3b, 0x3f, 0x3f, 0x38,
      0x10, 0x00, 0x00, 0x00, 0x30, 0x70, 0x78, 0x3c, 0x1f, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = five[i] : secondDigitArray[i] = five[i];
}

void setSix(uint8_t placeValue) {
  Serial.println("Setting 6");
  char six[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf8, 0xfc, 0x1c, 0x0e, 0x0e, 0x00, 0x00, 0xf0, 0xf8, 0x7c,
      0x1e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x1e, 0x3c, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x3c, 0x38, 0x70, 0x70, 0x70, 0x77, 0x6f, 0x7e,
      0x78, 0x70, 0x60, 0x60, 0x70, 0x70, 0x38, 0x3c, 0x1f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = six[i] : secondDigitArray[i] = six[i];
}

void setSeven(uint8_t placeValue) {
  Serial.println("Setting 7");
  char seven[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0xfe, 0x1c, 0x3c, 0x38, 0x70, 0x70, 0xe0, 0xe0, 0xc0,
      0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      0x01, 0x03, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = seven[i] : secondDigitArray[i] = seven[i];
}

void setEight(uint8_t placeValue) {
  Serial.println("Setting 8");
  char eight[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf8, 0x3c, 0x1c, 0x1c, 0x1c, 0x1c, 0xf8, 0xf0, 0xf8,
      0x3c, 0x1c, 0x0e, 0x0e, 0x0e, 0x0e, 0x1c, 0x3c, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x3c, 0x38, 0x38, 0x38, 0x38, 0x1f, 0x0f, 0x1f,
      0x3c, 0x38, 0x70, 0x70, 0x70, 0x70, 0x38, 0x3c, 0x1f, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = eight[i] : secondDigitArray[i] = eight[i];
}

void setNine(uint8_t placeValue) {
  Serial.println("Setting 9");
  char nine[256] =

    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0xf8, 0x1c, 0x0c, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x1e,
      0xfe, 0xfe, 0xce, 0x0e, 0x0e, 0x1c, 0x1c, 0x78, 0xf8, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x3c, 0x38, 0x70, 0x70, 0x70, 0x70, 0x70, 0x38,
      0x3f, 0x1f, 0x07, 0x00, 0x00, 0x70, 0x38, 0x3c, 0x1f, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  for (uint16_t i = 0; i < 256; i++) placeValue == 1 ? firstDigitArray[i] = nine[i] : secondDigitArray[i] = nine[i];
}