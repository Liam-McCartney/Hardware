// Author  : Liam McCartney
/* Purpose : This sketch is used to control the Robot arm I made for my medium ISP

This sketch can set multiple points for the robot to make trips through, or after the
trips have been made will allow the user to take over and control the robot from the
Serial monitor

Controls are sent as numbers. Each control starts with the number of the servo, from 0
to 4, and is followed by the three digit degree angle for the servo. Example to move
servo b to 90 degrees:

2090

*/
// Status  : Working
// Date    : April 12, 2024

#include <Servo.h>

#define pi 3.141592
//yum

#define PIN_P 3
#define PIN_A 5
#define PIN_B 6
#define PIN_C 9
#define PIN_EF 10
//Servo PWM pins

struct vect {
  float x;
  float y;
  float z;
};

vect origin = { 0, 0, 0 };
vect target = { 17, 17, 0 };

#define l1 10
#define l2 10
#define l3 10
//Length of arms

#define limit 15
//This is the number of iterations of FABRIK that will be completed

vect p0 = origin;
vect p1 = { 0, 0, l1 };
vect p2 = { 0, 0, l1 + l2 };
vect p3 = { 0, 0, l1 + l2 + l3 };
//Initialize to straight up position

struct coord2D {
  float x;
  float y;
};

coord2D P0 = { 0, 0 };
coord2D P1, P2, P3;

Servo pServo, aServo, bServo, cServo, efServo;

float a, b, c, p, ef;

float prevP = 0;
//Used for smooth motion

const byte numChars = 32;
char receivedChars[numChars];
//An array to store the received data from Serial monitor
boolean newData = false;
boolean first = true;

vect vectorSub(vect a, vect b) {
  vect r;
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  r.z = a.z - b.z;

  return (r);
}

float vectLength(vect v) {
  float l = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
  return (l);
}

vect unitVect(vect v) {
  vect r;
  r.x = v.x / vectLength(v);
  r.y = v.y / vectLength(v);
  r.z = v.z / vectLength(v);
  return (r);
}

vect scalar(vect v, float k) {
  vect r;
  r.x = k * v.x;
  r.y = k * v.y;
  r.z = k * v.z;
  return (r);
}

vect vectSum(vect a, vect b) {
  vect r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  r.z = a.z + b.z;
  return (r);
}

void convertTo2D() {
  //Takes a vector and converts it to a 2D point
  P1.x = sqrt(pow(p1.x, 2) + pow(p1.y, 2));
  P1.y = p1.z;
  P2.x = sqrt(pow(p2.x, 2) + pow(p2.y, 2));
  P2.y = p2.z;
  P3.x = sqrt(pow(p3.x, 2) + pow(p3.y, 2));
  P3.y = p3.z;
}

void iterate(vect target) {
  //Completes a single iteration of FABRIK
  float reach = l1 + l2 + l3;
  if (reach < vectLength(target)) {
    Serial.println("Not Possible! Coordinate is too far!");
  } else {
    p3 = target;
    //Set p3 to the target
    p2 = vectSum(scalar(unitVect(vectorSub(p2, p3)), l3), p3);
    //Set p2 to l3 away from p3 on the line from p3 to p2
    p1 = vectSum(scalar(unitVect(vectorSub(p1, p2)), l2), p2);
    //Set p1 to l2 away from p2 on the line from p2 to p1

    p1 = vectSum(scalar(unitVect(vectorSub(p1, p0)), l1), p0);
    //Set p1 to l1 away from p0 on the line from p0 to p1
    p2 = vectSum(scalar(unitVect(vectorSub(p2, p1)), l2), p1);
    //Set p2 to l2 away from p1 on the line from p1 to p2
    p3 = vectSum(scalar(unitVect(vectorSub(p3, p2)), l3), p2);
    //Set p3 to l3 away from p2 on the line from p2 to p3
  }
}

float getA() {
  return (float(atan(P1.y / P1.x)));
}

float getB() {
  //Use cosine law to return b
  return (acos((pow(P2.x, 2) + pow(P2.y, 2) - pow(l1, 2) - pow(l2, 2)) / (-2 * l1 * l2)));
}

float getC() {
  return (acos((pow(sqrt(pow(P3.x - P1.x, 2) + pow(P3.y - P1.y, 2)), 2) + -1 * (pow((P2.x - P1.x), 2) + pow((P2.y - P1.y), 2) + pow((P3.x - P2.x), 2) + pow((P3.y - P2.y), 2))) / (-2 * sqrt(pow((P2.x - P1.x), 2) + pow((P2.y - P1.y), 2)) * sqrt(pow((P3.x - P2.x), 2) + pow((P3.y - P2.y), 2)))));
  //Well that's a magic formula if there ever was one...
  //This is acutally just cosine law but we it was not behaving with the length variables.
  //I was probably (definitely) just making a mistake somewhere but I cut out the middle man and took the lengths manually
}

float getP() {
  return (atan(target.y / target.x));
}

float getFinalP(vect v) {
  return (atan(v.y / v.x));
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Hello World");
  delay(3000);

  a = 90;
  b = 0;
  c = 180;
  ef = 180;
  //Starting angles for straight up position

  pServo.attach(PIN_P);
  aServo.attach(PIN_A);
  bServo.attach(PIN_B);
  cServo.attach(PIN_C);
  efServo.attach(PIN_EF);
}

void printOutputs() {
  Serial.println("-------------");
  Serial.print("p = ");
  Serial.println(p / 180 * pi);
  Serial.print("a = ");
  Serial.println(a / 180 * pi);
  Serial.print("b = ");
  Serial.println(b / 180 * pi);
  Serial.print("c = ");
  Serial.println(c / 180 * pi);
  Serial.println("-------------");
}

void printVect(vect v) {
  Serial.print("(");
  Serial.print(v.x);
  Serial.print(",");
  Serial.print(v.y);
  Serial.print(",");
  Serial.print(v.z);
  Serial.println(")");
}

void loop() {

  if (first) {
    //Only execute once to allow the code to eventually go to user control
    vect start = { 5, 5, 25 };
    vect finish = { 5, 25, 15 };
    int pathLength = 50;
    //Number of waypoints to be calculated

    trip(start, finish, pathLength);
    delay(1000);

    start = finish;
    finish = { 5, 5, 25 };
    trip(start, finish, pathLength);
    delay(1000);

    start = finish;
    finish = { 20, 5, 10 };
    trip(start, finish, pathLength);
    delay(1000);

    start = finish;
    finish = { 5, 5, 25 };
    trip(start, finish, pathLength);
    delay(1000);
  }

  first = false;

  recvWithEndMarker();
  showNewData();
  //Read from Serial monitor
  updatePositions();
}

void trip(vect start, vect finish, int pathLength) {
  //Moves from start to finish

  vect path[pathLength];
  for (int i = 0; i < limit; i++) {
    iterate(start);
  }
  convertTo2D();

  printVect(p1);
  printVect(p2);
  printVect(p3);

  a = getA();
  b = getB();
  c = getC();
  p = atan(start.y / start.x);
  p = p * 180 / pi;
  a = a * 180 / pi;
  b = 180 - b * 180 / pi;
  c = c * 180 / pi;
  Serial.println("Start Positon:");
  printOutputs();
  updatePositions();
  //This first section moves the robot to the start position

  delay(2000);

  Serial.println("----------------------------------------");

  interp(start, finish, path, pathLength);
  //Interpolates the path

  Serial.println("----------------------------------------");

  for (int i = 0; i < pathLength; i++) {
    printVect(path[i]);
  }

  Serial.println("----------------------------------------");

  for (int n = 0; n < pathLength; n++) {
    for (int i = 0; i < limit; i++) {
      iterate(path[n]);
    }
    convertTo2D();
    a = getA();
    b = getB();
    c = getC();
    p = prevP + (getFinalP(finish) - prevP) / pathLength * n;

    p = p * 180 / pi;
    a = a * 180 / pi;
    b = 180 - b * 180 / pi;
    c = c * 180 / pi;
    printVect(path[n]);
    printOutputs();

    updatePositions();
    //Moves to every point along the path
  }
  prevP = getFinalP(finish);
}

void interp(vect a, vect b, vect path[], int size) {
  /*
  I am unsure as to why but when I originally made this code in pure c++
  I got compilation errors when passing in pointers to the first index in path[].
  I am aware this is a syntactical abomination but it is functional
  */
  path[0] = a;
  path[size - 1] = b;
  //set first and last point

  vect r = vectorSub(b, a);
  vect unit = unitVect(r);

  Serial.print("Unit vector: ");
  printVect(unit);

  float scale;

  if (r.x) {
    scale = r.x / unit.x;
  } else if (r.y) {
    scale = r.y / unit.y;
  } else if (r.z) {
    scale = r.z / unit.z;
  }

  float multiplier = scale / size;

  for (int t = 1; t < size - 1; t++) {
    path[t] = vectSum(scalar(unit, t * multiplier), a);
    //Populate the path with coordinates in between points
  }
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() && !newData) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ++ndx;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';
      //terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    Serial.println(receivedChars);
    int temp = int(receivedChars[1]) * 100 - 2 * 528 - 4272 + int(receivedChars[2]) * 10 + int(receivedChars[3]);
    //Get integer values from ascii values
    if (receivedChars[0] == '0') {
      Serial.println("pivot");
      p = temp;
    } else if (receivedChars[0] == '1') {
      Serial.println("a");
      a = temp;
    } else if (receivedChars[0] == '2') {
      Serial.println("b");
      b = 180 - temp;
      //this one is on backwards
    } else if (receivedChars[0] == '3') {
      Serial.println("c");
      c = temp;
    } else if (receivedChars[0] == '4') {
      Serial.println("ef");
      ef = temp;
    }
    updatePositions();
    printOutputs();

    //Serial.println(p);
    newData = false;
  }
}

void updatePositions() {
  pServo.write(map(int(p), 0, 180, 0, 150));
  aServo.write(map(int(a), 0, 180, 4, 131));
  bServo.write(int(map(int(b), 0, 180, 0, 128)));
  cServo.write(int(c));
  efServo.write(ef);
  //Almost none of the servos worked without mapping them to seemingly random ranges
}