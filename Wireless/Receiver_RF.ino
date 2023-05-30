// Project   : Wireless Communication
// Purpose   : Receives data from transmitter
// Course    : ICS3U
// Author    : Liam McCartney
// Date      : 2023 05 29
// MCU       : 328P (Nano)
// Status    : Working
// Reference : http://darcy.rsgc.on.ca/ACES/TEI3M/2223/Tasks.html#wireless

#define recPin A0  //Input pin
#define outPin 2   //Output pin

uint8_t thresh = 0;
//Reading is only used if it is over the threshold, filters noise

#define threshPin A2
#define threshHigh A3
#define threshLow A1

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("-----------");
  Serial.println("Hello World");

  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, 0);

  pinMode(threshHigh, OUTPUT);
  pinMode(threshLow, OUTPUT);

  digitalWrite(threshHigh, 1);  //Pot pins
  digitalWrite(threshLow, 0);
}


//-------------------------------------//
//Variables for loop():
uint32_t currentTime = 0;
bool proceed = false;

uint8_t mybyte = 0b0;
uint8_t bit = 0;

bool sign;
//-------------------------------------//

void loop() {
  thresh = analogRead(threshPin) >> 6;
  //Find the threshold

  if (analogRead(recPin) > thresh) {
    //Interrupt impossible as analogRead() is needed

    delay(75);
    //If signal is detected, wait for initial alert to end

    currentTime = millis();
    while (100 > millis() - currentTime) {
      //For exactly 100 ms check the bit after the alert
      if (analogRead(recPin) > thresh) {
        sign = true;
      }
    }

    delay(50);
    //Delay to prevent overlap

    Serial.println("---------");
    Serial.println("Received!");
    if (sign) {
      //If the bit is HIGH...
      mybyte = mybyte | (1 << bit);
      sign = false;
    }
    ++bit;
    //Next bit
    Serial.println(mybyte);


    if (bit == 8) {
      //If byte has been transmitted...
      bit = 0;
      Serial.println("Setting LED");
      mybyte ? digitalWrite(outPin, 1) : digitalWrite(outPin, 0);
      //Turns the relay off or on depending on the code sent
      mybyte = 0;
      //Reset byte
    }
  }
}