// Project   : Medium ISP - The WireFrame Rotator
// Purpose   : Rotates 3D wireframes on an LCD display
// Course    : ICS3U
// Author    : Liam McCartney
// Date      : 2023 03 10
// MCU       : ATmega328p
// Status    : Working

// include TFT and SPI libraries
#include <TFT.h>
#include <SPI.h>
#include "Keypad.h"

#define ROWS 4  //Keypad setup
#define COLS 3

#define Pre1 2  //Preset pins
#define Pre2 3

#define negativeAllowed false
#define positiveAllowed true
//Determines whether or not negative inputs are allowed

char keys[ROWS][COLS] = {
  { '#', '0', '*' },
  { '9', '8', '7' },
  { '6', '5', '4' },
  { '3', '2', '1' }
};
//Keypad setup

uint8_t rowPins[ROWS] = { 5, 6, 7, 12 };
uint8_t colPins[COLS] = { A5, A4, 4 };
//More keypad setup

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//Keypad initialization

#define cs 10
#define dc 9
#define rst 8
//LCD display pins

TFT TFTscreen = TFT(cs, dc, rst);
//Setting up LCD

uint8_t numVertices = 0;

#define angle 0.0872665
//Angle in radians, this determines speed

#define signPin A0
//Pin that determines sign (+/-) of inputs

uint16_t scale = 8;
//Every point is multiplied by scale

uint16_t currentMillis;
//Used for delay

boolean first = true;
//Needed for setup

//--------------PRESETS--------------//
float cubeVertices[8][3] = {
  { 2, 2, 2 },
  { 2, 2, -2 },
  { 2, -2, 2 },
  { 2, -2, -2 },
  { -2, 2, 2 },
  { -2, 2, -2 },
  { -2, -2, 2 },
  { -2, -2, -2 }
};

int16_t cubeEdgeTable[12][2] = {
  { 0, 1 },
  { 0, 2 },
  { 0, 4 },
  { 1, 3 },
  { 1, 5 },
  { 2, 3 },
  { 2, 6 },
  { 3, 7 },
  { 4, 5 },
  { 4, 6 },
  { 5, 7 },
  { 6, 7 }
};

float aceVertices[12][3] = {
  { 0, 0, 1.6 },
  { 0.5, 0, 0.9 },
  { 1.3, 0, 0 },
  { 1.3, 0, -0.3 },
  { 1, 0, -0.5 },
  { 0, 0, -0.7 },
  { 0.2, 0, -1.25 },
  { -0.2, 0, -1.25 },
  { -1, 0, 1 },
  { -1.3, 0, -0.3 },
  { -1.3, 0, 0 },
  { -0.5, 0, 0.9 }
};

int16_t aceEdgeTable[12][2] = {
  { 0, 1 },
  { 1, 2 },
  { 2, 3 },
  { 3, 4 },
  { 4, 5 },
  { 5, 6 },
  { 6, 7 },
  { 7, 5 },
  { 5, 9 },
  { 9, 10 },
  { 10, 11 },
  { 0, 11 },
};
//--------------PRESETS--------------//

uint8_t numLines = 0;

int8_t edgeTable[20][2];
float vertices[20][3];

float screenCoords[20][2];
float backupScreenCoords[20][2];
//Setting up storage

#define x 0
#define y 1
#define z 2
//Used for navigating arrays

#define focalLength 7.00

#define inputPin A2

float sine;
float cosine;
//global variables for sin and cos, but they need a function

void setup() {
  TFTscreen.begin();
  //Start LCD
  TFTscreen.background(0, 0, 0);
  TFTscreen.setTextSize(2);
  //Clear screen and set text size

  sine = sin(angle);
  cosine = cos(angle);
  //Setting sin and cos values

  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text("Welcome to", 20, 20);
  TFTscreen.text("The", 60, 38);
  TFTscreen.text("WireFrame", 25, 56);
  TFTscreen.text("Rotator", 40, 74);
  //Print start screen

  attachInterrupt(digitalPinToInterrupt(Pre1), setCube, RISING);
  attachInterrupt(digitalPinToInterrupt(Pre2), setAce, RISING);
  //Preset interrupts

  while (digitalRead(inputPin))
    ;
  //Keep the start screen until button pressed

  TFTscreen.background(0, 0, 0);

  TFTscreen.text("How many", 33, 32);
  TFTscreen.text("vertices", 35, 48);
  TFTscreen.text("would you", 28, 64);
  TFTscreen.text("like?", 55, 82);
  //Go to next screen

  currentMillis = millis() + 1000;
  while (millis() < currentMillis)
    ;
  //Essentially just delay (delay() does not always work)

  TFTscreen.background(0, 0, 0);
  TFTscreen.setTextSize(1);
  TFTscreen.text("Input # vertices:", 3, 3);
  TFTscreen.setTextSize(2);
  //Shrink text and move to top left

  uint8_t NumVertices = 0;
  uint8_t numDigits = 0;
  //Needed values

  while (digitalRead(inputPin)) {
    //Until the SELECT button is pressed
    char key = keypad.getKey();
    //Get a key
    if (key != NO_KEY) {
      char verticesTxt[numDigits + 1];

      TFTscreen.stroke(0, 0, 0);
      TFTscreen.text('NumVertices', 60, 40);
      //Print over the previous text in black
      TFTscreen.stroke(255, 255, 255);

      NumVertices = NumVertices * 10 + key - 48;
      //Find new number (total)

      sprintf(verticesTxt, "%d", NumVertices);
      TFTscreen.text(verticesTxt, 70, 50);
      //Print new number
      numDigits++;
    }
  }
  numVertices = NumVertices;
  //Global and local continuity

  TFTscreen.background(0, 0, 0);
  TFTscreen.textSize(1);
  TFTscreen.text("Enter Points:", 3, 3);
  TFTscreen.textSize(2);
  //Move to next screen

  int8_t inputVertices[NumVertices][3];
  //Setup storage

  for (uint8_t i = 0; i < NumVertices; i++) {
    //Repeat for every vertex
    for (uint8_t n = 0; n < 3; n++) {
      //Repeat once for x, y, z
      inputVertices[i][n] = getKey(negativeAllowed);
      //Set vertex to the key pressed

      char value[1];
      sprintf(value, "%d", inputVertices[i][n]);
      TFTscreen.text(value, 50 + 30 * n, 40);
      //Print the full vertex coordinate
    }

    currentMillis = millis() + 2000;
    while (millis() < currentMillis)
      ;
    //Delay

    if (i != NumVertices) {
      TFTscreen.background(0, 0, 0);
      TFTscreen.textSize(1);
      TFTscreen.text("Enter Points:", 3, 3);
      TFTscreen.textSize(2);
    }
    //If not finished, then clear current vertex and move to next vertex
  }

  TFTscreen.background(0, 0, 0);
  TFTscreen.setTextSize(1);
  TFTscreen.text("Input # Edges", 35, 30);
  //Next Screen

  currentMillis = millis() + 3000;
  while (millis() < currentMillis)
    ;
  //Delay

  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text("Input # Edges", 35, 30);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text("Input # Edges", 3, 3);
  //Shirnk to top left

  uint8_t NumEdges = 0;
  uint8_t numEdgeDigits = 0;
  //Setting up storage

  TFTscreen.textSize(2);
  while (digitalRead(inputPin)) {
    //Until SELECT button is pressed
    char key = keypad.getKey();

    if (key != NO_KEY) {
      //Set the number of edges to the number pressed
      char edgeTxt[numEdgeDigits + 1];

      TFTscreen.stroke(0, 0, 0);
      TFTscreen.text('NumEdges', 60, 40);
      TFTscreen.stroke(255, 255, 255);
      //Print over previous number in black

      NumEdges = NumEdges * 10 + key - 48;

      sprintf(edgeTxt, "%d", NumEdges);
      TFTscreen.text(edgeTxt, 70, 50);
      //Print new number
      numEdgeDigits++;
    }
  }
  numLines = NumEdges;
  //Local-Global continuity

  TFTscreen.background(0, 0, 0);
  TFTscreen.textSize(1);
  TFTscreen.text("Input EdgeTable:", 35, 30);
  //Next frame

  currentMillis = millis() + 3000;
  while (millis() < currentMillis)
    ;
  //Delay

  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text("Input EdgeTable:", 35, 30);
  TFTscreen.stroke(255, 255, 255);
  //Past over previous screen

  TFTscreen.text("Input EdgeTable:", 3, 3);
  //Write new text

  for (uint8_t i = 0; i < numLines; i++) {
    //Once for every line...
    TFTscreen.background(0, 0, 0);
    TFTscreen.text("Input EdgeTable:", 3, 3);
    //Print text
    for (uint8_t n = 0; n < 2; n++) {
      //Input 2 indicies to be linked...
      edgeTable[i][n] = getKey(positiveAllowed);
      //Get a number with no negatives allowed

      TFTscreen.textSize(2);

      char toPrint[1];
      sprintf(toPrint, "%d", (edgeTable[i][n]));
      TFTscreen.text(toPrint, 70 + 20 * n, 30);
      //Print value to screen

      TFTscreen.textSize(1);

      if (n == 1) {
        currentMillis = millis() + 1000;
        while (millis() < currentMillis)
          ;
      }
      //Delay, but only for first number
    }
  }
  TFTscreen.background(0, 0, 0);
  //Clear screen, settup is complete
}

//-------------------Loop Starts-------------------//

void loop() {
  mapPoints();
  //Map 3D to 2D

  if (first) {
    //On the first itteration
    for (uint8_t i = 0; i < numLines; i++) {
      //Once for every line
      int8_t x1 = screenCoords[edgeTable[i][0]][0] * scale + 80;
      int8_t y1 = screenCoords[edgeTable[i][0]][1] * scale + 64;

      int8_t x2 = screenCoords[edgeTable[i][1]][0] * scale + 80;
      int8_t y2 = screenCoords[edgeTable[i][1]][1] * scale + 64;
      //Find line start and end coordinates

      TFTscreen.stroke(255, 255, 255);
      //Colour can be changed here

      TFTscreen.line(x1, y1, x2, y2);
      //Draw line
    }
  } else {
    //If it is not the first time
    for (uint8_t i = 0; i < numLines; i++) {
      //For every line...
      int8_t x1 = backupScreenCoords[edgeTable[i][0]][0] * scale + 80;
      int8_t y1 = backupScreenCoords[edgeTable[i][0]][1] * scale + 64;

      int8_t x2 = backupScreenCoords[edgeTable[i][1]][0] * scale + 80;
      int8_t y2 = backupScreenCoords[edgeTable[i][1]][1] * scale + 64;
      //Find the line between the coordinate's previous position

      TFTscreen.stroke(0, 0, 0);
      TFTscreen.line(x1, y1, x2, y2);
      //Paste over the line in black (erase)

      x1 = screenCoords[edgeTable[i][0]][0] * scale + 80;
      y1 = screenCoords[edgeTable[i][0]][1] * scale + 64;

      x2 = screenCoords[edgeTable[i][1]][0] * scale + 80;
      y2 = screenCoords[edgeTable[i][1]][1] * scale + 64;
      //Find the new coordinates of the line

      TFTscreen.stroke(255, 255, 255);
      TFTscreen.line(x1, y1, x2, y2);
      //Draw it
    }
  }
  delay(15);
  rotatePoints();

  for (uint8_t i = 0; i < numVertices; i++) {
    backupScreenCoords[i][0] = screenCoords[i][0];
    backupScreenCoords[i][1] = screenCoords[i][1];
    //Backup the points so that can be erased next cycle
  }

  if (first) {
    TFTscreen.background(0, 0, 0);
    first = false;
    //No longer first
  }
}

//-------------------Loop Done-------------------//

int8_t getKey(boolean sign) {
  while (true) {
    //Until a key is pressed...
    char key = keypad.getKey();
    if (key != NO_KEY) {

      if (!sign) {
        //If negative allowed...
        key = digitalRead(signPin) ? key - 48 : -1 * key + 48;
        //Key is either + or - depending on switch
      } else {
        //Otherwise it is positive
        key -= 48;
      }
      return (key);
      //Break loop with return
    }
  }
}

void rotatePoints() {
  for (uint8_t point = 0; point < numVertices; point++) {
    //Perform the rotation for every point using the rotation matrix
    float newX = vertices[point][x] * cosine - vertices[point][y] * sine;
    float newY = vertices[point][x] * sine + vertices[point][y] * cosine;

    //Update the coordinates
    vertices[point][x] = newX;
    vertices[point][y] = newY;
  }
}

void mapPoints() {
  for (uint8_t reps = 0; reps < numVertices; reps++) {
    //Once for every vertex
    float calcX = vertices[reps][x];
    float calcY = vertices[reps][y];
    float calcZ = vertices[reps][z];
    //Temporary storage variables for readability

    screenCoords[reps][x] = ((calcX * focalLength) / (focalLength + calcY));
    screenCoords[reps][y] = (focalLength * calcZ) / (focalLength + calcY);
    //Use formula to calculate the mapped coordinates
  }
}

//Cube Pre-set ISR
void setCube() {
  noInterrupts();
  //Do not allow any other interrupts (no need to debounce)
  TFTscreen.background(0, 0, 0);
  //Wipe the screen

  for (uint8_t i = 0; i < 8; i++) {
    //Repeat 8 times since a cube has 8 vertices
    for (uint8_t n = 0; n < 3; n++) {
      vertices[i][n] = cubeVertices[i][n];
      //Set vertices table to cube's vertices
    }
  }

  for (uint8_t i = 0; i < 12; i++) {
    //A cube has 12 edges
    for (uint8_t n = 0; n < 2; n++) {
      edgeTable[i][n] = cubeEdgeTable[i][n];
      //Set edges to cube's edges
    }
  }

  numLines = 12;
  numVertices = 8;
  scale = 16;
  //Custom info for cube

  interrupts();
}

//Ace Pre-set ISR
void setAce() {
  noInterrupts();
  //Do not allow any other interrupts (no need to debounce)
  TFTscreen.background(0, 0, 0);
  //Wipe the screen

  for (uint8_t i = 0; i < 12; i++) {
    //The ace has 12 points
    for (uint8_t n = 0; n < 3; n++) {
      vertices[i][n] = aceVertices[i][n];
      //Set vertices table to ace's vertices
    }
  }

  for (uint8_t i = 0; i < 12; i++) {
    //The ace has 12 edges
    for (uint8_t n = 0; n < 2; n++) {
      edgeTable[i][n] = aceEdgeTable[i][n];
      //Set edge table to ace's edges
    }
  }

  numLines = 12;
  numVertices = 12;
  scale = 24;
  //Custom Info
  interrupts();
}