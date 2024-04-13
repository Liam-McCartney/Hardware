final float pi = 3.1415926535;

final int l1 = 10;
final int l2 = 10;
final int l3 = 12;

float[] coords = {15, 10};
//float X = coords[0];
//float Y = coords[1];

//float theta = atan(Y/X);

float error = 100;
//Big number

float a, b, c, A, B, C;
boolean flag = true;

float[] p1 = {10, 7};
float[] p2 = {14, 12};

int steps = 40;

float[][] trip = new float[steps][2];
float[][] tripAngles = new float[steps][3];

final int Width = 700;
final int Height = 450;

boolean newWindow = true;

void setup() {

  size(700, 450);
  background(255);
  fill(255);
  rect(0, 0, 700, 450);

  //approxAngles(X, Y);
  println("---------------------------");
  //solveAngles(X, Y);

  //solveAngles(15, 10);
  //renderAngle(a, b, c);
  planTrip(0, 19, 22, 19, steps);
  //renderTrip();
  //solveAngles(15, 15);
}

int n = 0;

void draw() {

  setUpGUI();
  solveAngles(trip[n][0],trip[n][1]);
  renderAngle(a,b,c);
  /*
  if (newWindow) {
    renderAngle(tripAngles[n][0], tripAngles[n][1], tripAngles[n][2]);
    n = (n + 1) % steps;
  } else {
    renderAngle(a, b, c);
  }
  */
  n = (n + 1) % steps;
}

void solveAngles(float X, float Y) {
  for (a = 0; a < 180; a++) {
    //float currentError = l1*sin(acos((X - l2*cos(theta) + l3*cos(theta + i*pi/180))/l1))+ l2*sin(theta) - l3*sin(theta+i*pi/180) - Y;
    //This equation makes sense just trust
    for (c = 0; c < 180; c++) {
      float theta = atan(Y/X);
      float xTest = l1*cos(a*pi/180) + l2*cos(theta) - l3*cos(theta + c*pi/180);
      float yTest = l1*sin(a*pi/180) + l2*sin(theta) - l3*sin(theta + c*pi/180);

      float currentError = dist(X, Y, xTest, yTest);

      if (flag) {
        //If first time...
        A = a*pi/180;
        B = atan(Y/X) + pi - A;
        C = c*pi/180;

        error = currentError;

        flag = false;
      }

      if (error > currentError) {
        //If this is the most accurate result...
        A = a*pi/180;
        B = atan(Y/X) + pi - A;
        C = c*pi/180;

        error = currentError;
      }
    }
  }
  //println("Here Are The Results:");
  a = A;
  b = B;
  c = C;
  /*println("a = " + a + ", b = " + b + ", c = " + c);
  println("Error was: " + error);
  println("Target: ("+X+","+Y+")");
  println("Real: ("+(l1*cos(a)-l2*cos(a + b)+l3*cos(a + b + c))+","+(l1*sin(a)-l2*sin(a + b)+l3*sin(a + b + c))+")");
  println("-------------------------");*/
  error = 100;
  flag = true;
}

void linearInterp(float x1, float y1, float x2, float y2, int steps) {
  float dy = y2 - y1;
  float dx = x2 - x1;
  float slope = ((dy)/(dx));
  float step = (abs(x2 - x1))/(steps - 1);

  for (int i = 0; i < (steps - 1); i++) {
    trip[i][0] = x1 + i*step;
    trip[i][1] = y1 + slope*(x1 + i*step);
    println("("+trip[i][0]+","+trip[i][1]+")");
  }
  trip[steps - 1][0] = x2;
  trip[steps - 1][1] = y2;
  println("("+trip[steps - 1][0]+","+trip[steps - 1][1]+")");
}

void planTrip(int x1, int y1, int x2, int y2, int steps) {
  linearInterp(x1, y1, x2, y2, steps);
  for (int i = 0; i < steps; i++) {
    //solveAngles(trip[i][0], trip[i][1]);
    solveAngles(trip[i][0], trip[i][1]);
    tripAngles[i][0] = a;
    tripAngles[i][1] = b;
    tripAngles[i][2] = c;
  }
}

void renderTrip() {
  for (int i = 0; i < steps; i++) {
    renderAngle(tripAngles[i][0], tripAngles[i][1], tripAngles[i][2]);
    //delay(1000);
  }
}

void renderAngle(float a, float b, float c) {
  //println("Rendering Angle...");
  //println("a = " + a + ", b = " + b + ", c = " + c);
  float a1, a2, b1, b2, c1, c2;
  a1 = l1*cos(a);
  a2 = l2*sin(a);
  b1 = l1*cos(a) - l2*cos(a + b);
  b2 = l1*sin(a) - l2*sin(a + b);
  c1 = l1*cos(a) - l2*cos(a + b) + l3*cos(a + b + c);
  c2 = l1*sin(a) - l2*sin(a + b) + l3*sin(a + b + c);

  //fill(255);
  //rect(0, 0, 700, 450);

  //smartDelay(1);
  stroke(0, 0, 0);
  strokeWeight(4);  // Default

  line(0 + Width/2, 0 + Height/1.5, 10*a1 + Width/2, -10*a2 + Height/1.5);
  //println("Line From (350,225) to (" + 10*a1 + Width/2 + "," + -10*a2 + Height/1.5 + ")");
  line(10*a1 + Width/2, -10*a2 + Height/1.5, 10*b1 + Width/2, -10*b2 + Height/1.5);
  //println("Line From (" + 10*a1 + Width/2 + "," + -10*a2 + Height/1.5 + ") to (" + 10*b1 + Width/2 + "," + -10*b2 + Height/1.5 + ")");
  line(10*b1 + Width/2, -10*b2 + Height/1.5, 10*c1 + Width/2, -10*c2 + Height/1.5);
  //println("Line From (" + 10*b1 + Width/2 + "," + -10*b2 + Height/1.5 + ") to (" + 10*c1 + Width/2 + "," + -10*c2 + Height/1.5 + ")");
}

void smartDelay(float time) {
  int startTime = millis();
  while (millis() < (time*1000 + startTime));
}

void approxAngles(float X, float Y) {
  int min1 = int(atan(Y/X)*(180/pi));
  //Set the minimum angle a to point straight at target

  for (a = min1; a < 180; a++) {
    for (b = 0; b < 180; b++) {
      for (c = 0; c < 180; c++) {
        //Iterate through all angles (hopefully I make an algorithm at some point)
        //Solves for Inverse Kinematics with Forward Kinematics

        float xTest = l1*cos(a*pi/180) - l2*cos((a + b)*pi/180) + l3*cos((a + b + c)*pi/180);
        float yTest = l1*sin(a*pi/180) - l2*sin((a + b)*pi/180) + l3*sin((a + b + c)*pi/180);
        //Compute the forward kinematics from the test

        float currentError = dist(X, Y, xTest, yTest);
        //Find the error between the test and the real point

        if (flag) {
          //If first time...
          A = a*pi/180;
          B = b*pi/180;
          C = c*pi/180;

          error = currentError;

          flag = false;
        }

        if (error > currentError) {
          //If this is the most accurate result...
          A = a*pi/180;
          B = b*pi/180;
          C = c*pi/180;

          error = currentError;
        }
      }
    }
  }
  println("Here Are The Results:");
  a = A;
  b = B;
  c = C;
  println("a = " + a + ", b = " + b + ", c = " + c);
  println("Error was: " + error);
  error = 100;
  flag = true;
}

float findError(float X, float Y) {
  float xTest = l1*cos(a) - l2*cos(a + b) + l3*cos(a + b + c);
  float yTest = l1*sin(a) - l2*sin(a + b) + l3*sin(a + b + c);
  //Compute the forward kinematics from the test

  float testError = dist(X, Y, xTest, yTest);
  return(testError);
}

void setUpGUI() {

  smartDelay(0.0425);
  stroke(255);
  fill(255);
  rect(0, 0, 700, 450);

  stroke(0);
  fill(240);
  strokeWeight(0);
  rect(22, 22, 122, 47);
  rect(22, 152, 122, 47);
  strokeWeight(2);

  if (mouseX > 20 && mouseX < 140 && mouseY > 20 && mouseY < 65) {
    fill(245); //Highlight the button whe.... you get it
    rect(22, 22, 120, 45);
  } else {
    fill(255);
    rect(20, 20, 120, 45);
  }

  fill(255);
  rect(20, 75, 120, 45);


  if (mouseX > 20 && mouseX < 140 && mouseY > 150 && mouseY < 195) {
    fill(245); //Highlight the button whe.... you get it
    rect(22, 152, 120, 45);
  } else {
    fill(255);
    rect(20, 150, 120, 45);
  }

  if (!newWindow) {
    if (mouseX > 20 && mouseX < 45 && mouseY > 400 && mouseY < 425) {
      fill(245); //Highlight the button whe.... you get it
      rect(21, 401, 25, 25);
    } else {
      fill(255);
      rect(20, 400, 25, 25);
    }
    fill(0);
    textSize(35);
    text("<", 24, 425);
  }
  
   fill(255);
   rect(20, 205, 120, 45);

  fill(0);
  textSize(25);
  text("Move To", 37, 51);
  text("Trip", 59, 181);

  text(userInput[0], (56 - 6*(userInput[0].length()-5)), 106);
  text(userInput[1], (56 - 6*(userInput[1].length()-5)), 236);
}

void mousePressed() {
  //Check if the mouse is over the button when it's pressed
  if (mouseX > 20 && mouseX < 140 && mouseY > 75 && mouseY < 120) {
    current = 0;
  } else if (mouseX > 20 && mouseX < 140 && mouseY > 205 && mouseY < 250) {
    current = 1;
  } else if (mouseX > 20 && mouseX < 140 && mouseY > 150 && mouseY < 195) {

    String subStr0[] = userInput[0].split(",");
    subStr0[0] = subStr0[0].replaceAll("\\D", "");
    subStr0[1] = subStr0[1].replaceAll("\\D", "");

    int inputX0 = Integer.parseInt(subStr0[0]);
    int inputY0 = Integer.parseInt(subStr0[1]);

    String subStr1[] = userInput[1].split(",");
    subStr1[0] = subStr1[0].replaceAll("\\D", "");
    subStr1[1] = subStr1[1].replaceAll("\\D", "");

    int inputX1 = Integer.parseInt(subStr1[0]);
    int inputY1 = Integer.parseInt(subStr1[1]);

    planTrip(inputX0, inputY0, inputX1, inputY1, steps);
    renderTrip();
    
  } else if (mouseX > 20 && mouseX < 45 && mouseY > 400 && mouseY < 425) {
    newWindow = true;
  } else if (mouseX > 20 && mouseX < 140 && mouseY > 20 && mouseY < 65) {
    String subStr[] = userInput[0].split(",");
    subStr[0] = subStr[0].replaceAll("\\D", "");
    subStr[1] = subStr[1].replaceAll("\\D", "");

    int inputX = Integer.parseInt(subStr[0]);
    int inputY = Integer.parseInt(subStr[1]);

    solveAngles(inputX, inputY);
    newWindow = false;
  }
}

String[] userInput = {"(x,y)", "(x,y)"};
int current = 0;

void keyPressed() {
  if (key == BACKSPACE && userInput[current].length() > 0) {
    //If the user presses the BACKSPACE key and there is something to delete
    userInput[current] = userInput[current].substring(0, userInput[current].length() - 1);
  }
}

void keyTyped() {
  if (key >= ' ' && key <= 'z') {
    //If the typed character is a printable character, add it to userInput
    userInput[current] += key;
  }
}
