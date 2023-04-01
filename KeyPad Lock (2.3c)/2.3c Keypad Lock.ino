// Project   : 2.3 Keypad Password
// Purpose   : A device capable of remembering a password and checking if it is correct
// Course    : ICS3U
// Author    : Liam McCartney
// Date      : 2023 01 12
// MCU       : 328p
// Status    : Working
// Reference : http://darcy.rsgc.on.ca/ACES/TEI3M/2223/Tasks.html#BreadboardMega


#define COLS 3
#define ROWS 4

uint8_t rowPins[ROWS] = { A1, A2, A3, A4 };  //connect to the row pinouts of the keypad
uint8_t colPins[COLS] = { 9, 10, A1 };       //connect to the column pinouts of the keypad

char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

volatile boolean unlocked = false;
char key;

#define length 6
uint8_t password[length] = { 8, 7, 8, 9, 9, 9 };
uint8_t history[length];
//The password is initially preset until the user choses one

#define ledRed 7
#define ledGreen 8  //Pin definitions
#define savePassword 2
#define resetPin 3

void setup() {
  for (uint8_t i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
//Rows are inputs with internal pullups

  for (uint8_t i = 0; i < COLS; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], 0);
  }
//Cols are outputs

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  digitalWrite(ledRed, 1);
  digitalWrite(ledGreen, 0);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);

  digitalWrite(0, 1);
  digitalWrite(1, 1);
  //Making the bicolor LED red to show the device is "locked"

  attachInterrupt(digitalPinToInterrupt(resetPin), reset, RISING);
  //This interrupt listens the reset pin and calls the reset function
}

void loop() {
  if (unlocked) {
    digitalWrite(ledGreen, 1);
    digitalWrite(ledRed, 0);
    //Set the LED to green to show that the device is unlocked

    key = 0;
    while (key < 48 || key > 57) {
      for (uint8_t n = 0; n < COLS; n++) digitalWrite(colPins[n], 0);
      char key = getKey();
    }
    //Reads the current key pressed (even if it no key is pressed)

    for (int i = 0; i < length; i++) {
      history[i] = history[i + 1];
    }  //Shift ever index in history back 1

    history[length - 1] = key - 48;
    //Add the current key as the last index

    if (digitalRead(savePassword)) {
      //If the save password button IS pressed (on by default, PBNC)
      for (int i = 0; i < length; i++) password[i] = history[i];
      //Make the password the current history
    }
  } else {
    //If not unloscked
    digitalWrite(ledGreen, 0);
    digitalWrite(ledRed, 1);
    //Make the LED red

    key = 0;
     while (key < 48 || key > 57) {
      for (uint8_t n = 0; n < COLS; n++) digitalWrite(colPins[n], 0);
      char key = getKey();
    }
    //Get the current key

    for (int i = 0; i < length; i++) {
      history[i] = history[i + 1];
    }
    history[length - 1] = key - 48;
    //Again shifting history by 1 index then adding current read

    unlocked = true;  //No matter what, the lock unlocks here

    for (int i = 0; i < length; i++) {
      if (history[i] != password[i]) unlocked = false;
    }
    //But it only stays unlocked if it "survives" this for loop
  }
}

void reset() {
  unlocked = false;
}

uint8_t getRow() {
  for (uint8_t i = 0; i < ROWS; i++) if (!digitalRead(rowPins[i])) return i;
  //If a row is pressed, return the row
  return ROWS;
  //If not, return ROWS which will be out of bounds
}

uint8_t getCol(uint8_t row) {
  for (uint8_t i = 0; i < COLS; i++) {
    for (uint8_t n = 0; n < COLS; n++) digitalWrite(colPins[n], 1);
    digitalWrite(colPins[i], 0);
    //Isolate a single column

    if (!digitalRead(rowPins[row])) return i;
    //Return the column if the row goes low
  }
  return COLS;
  //Otherwise return out of bounds
}

char getKey() {
  uint8_t row = getRow();
  uint8_t col = getCol(row);
  return (keys[row][col]);
  //Return key from lookup array, will be null if no key
}