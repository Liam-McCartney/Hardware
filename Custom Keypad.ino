//Description : Keypad algorithm using internal pullup resistors
//Date        : 2023 04 01
//Author      : Liam McCartney

#define COLS 3
#define ROWS 4

uint8_t rowPins[ROWS] = { 9, 10, 11, 12 };  //connect to the row pinouts of the keypad
uint8_t colPins[COLS] = { 6, 7, 8 };        //connect to the column pinouts of the keypad

char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

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
}

void loop() {
  for (uint8_t n = 0; n < COLS; n++) digitalWrite(colPins[n], 0);
  //Set all the rows to 0, resets froms previous read
  char key = getKey();
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