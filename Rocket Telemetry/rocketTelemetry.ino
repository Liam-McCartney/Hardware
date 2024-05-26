#include <Wire.h>  // Wire library - used for I2C communication
#include <SoftwareSerial.h>

SoftwareSerial HC12(10,11);

#define ADXL345 0x53  // The ADXL345 sensor I2C address


float X_out, Y_out, Z_out, rollF, pitchF;  // Outputs

void setup() {
  Serial.begin(9600);  // Initiate serial communication for printing the results on the Serial monitor
  HC12.begin(9600);
  Wire.begin();        // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345);  // Start communicating with the device
  Wire.write(0x2D);                 // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8);  // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
  Wire.endTransmission();
  delay(10);

  //X-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);  // X-axis offset register
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);
  //Y-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F);  // Y-axis offset register
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);

  //Z-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20);  // Z-axis offset register
  Wire.write(5);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(ADXL345);
  Wire.write(0x31);
  Wire.endTransmission();

  Wire.beginTransmission(ADXL345);
  Wire.write(0x31);
  Wire.write(1 << 1);
  Wire.endTransmission();
}

void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);  // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);        // Read 6 registers total, each axis value is stored in 2 registers
  X_out = (Wire.read() | Wire.read() << 8);  // X-axis value
  Y_out = (Wire.read() | Wire.read() << 8);  // Y-axis value
  Z_out = (Wire.read() | Wire.read() << 8);  // Z-axis value
  X_out = X_out / 64;                       //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Z_out = Z_out / 64;
  Y_out = Y_out / 64;
  // X_out = X_out;                       //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  // Z_out = Z_out;
  // Y_out = Y_out;

  // Serial.print("Xa= ");
  // Serial.print(X_out);
  // Serial.print("   Ya= ");
  // Serial.print(Y_out);
  // Serial.print("   Za= ");
  // Serial.println(Z_out);

  float roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
  float pitch = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;

  //Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;

  float acceleration = sqrt(pow(X_out, 2) + pow(Y_out, 2) + pow(Z_out, 2)) - 1;

  if (Z_out < 0) acceleration = -acceleration;

  //HC12.println(String(rollF) + "/" + String(pitchF) + "/" + String(Z_out));

  HC12.print(rollF);
  HC12.print("/");
  HC12.print(pitchF);
  HC12.print("/");
  HC12.println(acceleration);


  // Serial.print(rollF);
  // Serial.print("/");
  // Serial.print(pitchF);
  // Serial.print("/");
  // Serial.println(Z_out);

  //Serial.println(String(X_out) + "/" + String(Y_out) + "/" + String(Z_out));

}