// Project   : Wireless Communication
// Purpose   : Transmits data to receiver
// Course    : ICS3U
// Author    : Liam McCartney
// Date      : 2023 05 29
// MCU       : 328P (Nano)
// Status    : Working
// Reference : http://darcy.rsgc.on.ca/ACES/TEI3M/2223/Tasks.html#wireless

#define comPin 2 //Pin used to power coil through 555

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("-----------");
  Serial.println("Hello World");
  pinMode(comPin, OUTPUT);
  digitalWrite(comPin, 0);
}

void loop() {
  if (digitalRead(3)) digitalRead(A5) ? sendByte(0b11111111) : sendByte(0b00000000);
  //Sends either on or off cod depending on switch position
}

/**************
Protocol:

1. Send 50 ms HIGH to alert receiver
2. After 50 ms HIGH, send bit for 150 ms
3. Delay for receiver to catch up if needed

**************/


void sendBit(uint8_t bit) {
  if (bit == 0) {
    Serial.println("Sending 0");
    digitalWrite(comPin, 1);
    delay(50);
    digitalWrite(comPin, 0);
    delay(250);
  } else {
    Serial.println("Sending 1");
    digitalWrite(comPin, 1);
    delay(50);
    digitalWrite(comPin, 1);
    delay(150);
    digitalWrite(comPin, 0);
    delay(100);
  }
}

void sendByte(uint8_t mybyte) {
  Serial.print("Sending Byte: ");
  Serial.println(mybyte);
  uint16_t mask = 1;

  for (int i = 0; i < 8; i++) {
    uint8_t bit = mybyte & mask;
    mask = mask << 1;
    sendBit(bit);
    //Send the byte bit by bit using a mask
  }
}