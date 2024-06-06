import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

PFont spaceAgeFont;
//Create the font

PImage logo;
PImage graphBackground;
//Create the images

Serial myPort;
//Define Serial port

String data="";
//This is the variable that stores the incomming serial data
float roll, pitch, acceleration, velocity, altitude;
//Assorted declarations
float prevVelocity = 0;
float prevAcceleration = 0;
float prevAltitude = 0;
int prevTime = 0;

float g = 9.81;

String page = "home";
//page variable is used to check the current page
String prevPage = "home";
String prevPrevPage = "home";
//Basic back button

boolean tripOver = false;
boolean serialConnection = true;
boolean start = false;
//Booleans

public class Button {
  //Used for all buttons
  int X1;
  int Y1;
  int X2;
  int Y2;
  color colour;
  color pressedColour;
  color strokeColour;
  String buttonText;
  color textColour;
  int textSize;
}

Button homeButton = new Button();
Button backButton = new Button();
Button aButton = new Button();
Button vButton = new Button();
Button dButton = new Button();
Button allButton = new Button();
Button repButton = new Button();
Button liveButton = new Button();
Button startButton = new Button();
//Create all buttons

color backgroundColour = color(48,48,48);

float[] accelerationHistory = new float[1280];
float[] velocityHistory = new float[1280];
float[] altitudeHistory = new float[1280];
//Used to store past data and for graphing

float tenthTime = 0;
int pastTenthTime = 0;
int count = 1;
//More variables

void setup() {
  size(1280, 720, P3D);
  //Create the window and specify the Processing-3D rendering engine
  
  try {
  myPort = new Serial(this, "COM3", 9600);
  myPort.bufferUntil('\n');
  //Atempt to start serial connection
  } catch (RuntimeException e) {
    println("No Serial Connection! Live Data Tracking is Unavailable");
    serialConnection = false;
    //Catch if no connection can be established
  }
  
  background(backgroundColour);
  //Set background colour
  
  spaceAgeFont = createFont("spaceAge.ttf", 64);
  textFont(spaceAgeFont);
  //Load the font
  
  logo = loadImage("rocket.png");
  graphBackground = loadImage("graphBackground.png");
  //Load images
  
  
  accelerationHistory[0] = 0;
  velocityHistory[0] = 0;
  altitudeHistory[0] = 0;
  //Set first data-point to 0
}

//public class Page extends Button {
 // Button button;
//}

void draw() {
  background(backgroundColour);
  //set background colour
  
  //logReadings();
  logForExport();
  
  if (page == "home") {
   drawHome(); 
  } else if(page == "acceleration") {      //page.equals("acceleration");
    drawAcceleration();
  } else if(page == "velocity") {
    drawVelocity();
  } else if(page == "altitude") {
    drawAltitude();
  } else if(page == "allChannels") {
    drawAllChannels();
  } else if(page == "report") {
    drawReport();
  } else if (page == "live") {
    drawLive();
  }
  //Horrible if-else ladder to draw the correct page
  //I really need to learn classes but I don't know Java well
  
  tenthTime = millis() / 100.0;
  //This integer increments every 0.1 seconds
  
  if (pastTenthTime < int(tenthTime) && count < 1100 && start) {
    //If the start button has been pressed and tenthTime has increased and it is less than 1100...
    accelerationHistory[count] = acceleration;
    velocityHistory[count] = velocity;
    altitudeHistory[count] = altitude;
    ++count;
    //Update all records every tenth of a second, and increment count
  }
  
  pastTenthTime = int(tenthTime);
  //Update tenthTime
  
}

void button(int X1, int Y1, int X2, int Y2, color colour, color pressedColour, color strokeColour, String buttonText, color textColour, int textSize) {
  //Used to actually draw buttons because I am bad at Java
  boolean mouseOverButton;
  
  if(mouseX > X1 && mouseX < X2 && mouseY < Y2 && mouseY > Y1) {
   mouseOverButton = true;
   fill(pressedColour);
   //If the mouse is over the set to the pressed colour
  } else {
    mouseOverButton = false;
    fill(colour);
    //Else use the non-pressed colour
  }
  
  stroke(strokeColour);
  rect(X1, Y1, X2 - X1, Y2 - Y1);
  //Draw the rectangle from the coordinates
  
  textAlign(CENTER, CENTER);
  
  fill(textColour);
  textSize(textSize);
  text(buttonText, (X1 + X2) / 2, (Y1 + Y2) / 2);
  //Write the text on the button
}

void drawHome() {
  //For drawing home page...
  if(!serialConnection) {
    //If there is no serial connection tell the user
   textSize(20);
   fill(255,40,40);
   text("WARNING: No Serial Data Connection", 1015, 700); 
  }
  
  //This next eyesore is where all the buttons are given their properties
  
  homeButton.X1 = 0;
  homeButton.Y1 = 0;
  homeButton.X2 = 125;
  homeButton.Y2 = 35;
  homeButton.colour = color(0,0,0);
  homeButton.pressedColour = color(48,48,48);
  homeButton.strokeColour = color(240,240,240);
  homeButton.buttonText = "Home";
  homeButton.textColour = color(248,248,248);
  homeButton.textSize = 25;
  
  backButton.X1 = 125;
  backButton.X2 = 250;
  backButton.buttonText = "Back";

  backButton.Y1 = 0;

  backButton.Y2 = 35;
  backButton.colour = color(0,0,0);
  backButton.pressedColour = color(48,48,48);
  backButton.strokeColour = color(240,240,240);

  backButton.textColour = color(248,248,248);
  backButton.textSize = 25;
  
  aButton.X1 = 750;
  aButton.Y1 = 130;
  aButton.X2 = 1265;
  aButton.Y2 = 190;
  aButton.colour = color(0,0,0);
  aButton.pressedColour = color(48,48,48);
  aButton.strokeColour = color(240,240,240);
  aButton.buttonText = "Acceleration";
  aButton.textColour = color(248,248,248);
  aButton.textSize = 45;
  
  vButton.X1 = 750;
  vButton.Y1 = 200;
  vButton.X2 = 1265;
  vButton.Y2 = 260;
  vButton.colour = color(0,0,0);
  vButton.pressedColour = color(48,48,48);
  vButton.strokeColour = color(240,240,240);
  vButton.buttonText = "Velocity";
  vButton.textColour = color(248,248,248);
  vButton.textSize = 45;
  
  dButton.X1 = 750;
  dButton.Y1 = 270;
  dButton.X2 = 1265;
  dButton.Y2 = 330;
  dButton.colour = color(0,0,0);
  dButton.pressedColour = color(48,48,48);
  dButton.strokeColour = color(240,240,240);
  dButton.buttonText = "Altitude";
  dButton.textColour = color(248,248,248);
  dButton.textSize = 45;
  
  allButton.X1 = 750;
  allButton.Y1 = 340;
  allButton.X2 = 1265;
  allButton.Y2 = 400;
  allButton.colour = color(0,0,0);
  allButton.pressedColour = color(48,48,48);
  allButton.strokeColour = color(240,240,240);
  allButton.buttonText = "All Channels";
  allButton.textColour = color(248,248,248);
  allButton.textSize = 45;
  
  repButton.X1 = 750;
  repButton.Y1 = 410;
  repButton.X2 = 1265;
  repButton.Y2 = 470;
  repButton.colour = color(0,0,0);
  repButton.pressedColour = color(48,48,48);
  repButton.strokeColour = color(240,240,240);
  repButton.buttonText = "Flight Report";
  repButton.textColour = color(248,248,248);
  repButton.textSize = 45;
  
  liveButton.X1 = 750;
  liveButton.Y1 = 480;
  liveButton.X2 = 1265;
  liveButton.Y2 = 540;
  liveButton.colour = color(0,0,0);
  liveButton.pressedColour = color(48,48,48);
  liveButton.strokeColour = color(240,240,240);
  liveButton.buttonText = "Live View";
  liveButton.textColour = color(248,248,248);
  liveButton.textSize = 45;
  
  startButton.X1 = 1130;
  startButton.Y1 = 0;
  startButton.X2 = 1280;
  startButton.Y2 = 45;
  startButton.colour = color(0,0,0);
  startButton.pressedColour = color(48,48,48);
  startButton.strokeColour = color(240,240,240);
  startButton.buttonText = "Start";
  startButton.textColour = color(248,248,248);
  startButton.textSize = 30;
  //aaaaaand done
  
  image(logo, 30, 40);
  //Draw the logo
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  button(aButton.X1, aButton.Y1, aButton.X2, aButton.Y2, aButton.colour, aButton.pressedColour, aButton.strokeColour, aButton.buttonText, aButton.textColour, aButton.textSize);
  button(vButton.X1, vButton.Y1, vButton.X2, vButton.Y2, vButton.colour, vButton.pressedColour, vButton.strokeColour, vButton.buttonText, vButton.textColour, vButton.textSize);
  button(dButton.X1, dButton.Y1, dButton.X2, dButton.Y2, dButton.colour, dButton.pressedColour, dButton.strokeColour, dButton.buttonText, dButton.textColour, dButton.textSize);
  button(allButton.X1, allButton.Y1, allButton.X2, allButton.Y2, allButton.colour, allButton.pressedColour, allButton.strokeColour, allButton.buttonText, allButton.textColour, allButton.textSize);
  button(repButton.X1, repButton.Y1, repButton.X2, repButton.Y2, repButton.colour, repButton.pressedColour, repButton.strokeColour, repButton.buttonText, repButton.textColour, repButton.textSize);
  button(liveButton.X1, liveButton.Y1, liveButton.X2, liveButton.Y2, liveButton.colour, liveButton.pressedColour, liveButton.strokeColour, liveButton.buttonText, liveButton.textColour, liveButton.textSize);
  //Draw the needed buttons
}

void drawAcceleration() {
  //For drawing the acceleration page
  image(graphBackground, 0, 0);
  //Set to graph background, which is an image made in Illustrator
  
  if (!start) {
    //If the start button has not been pressed draw the start button
     button(startButton.X1, startButton.Y1, startButton.X2, startButton.Y2, startButton.colour, startButton.pressedColour, startButton.strokeColour, startButton.buttonText, startButton.textColour, startButton.textSize); 
  }
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  //Draw the home and back buttons
  graphAcceleration();
  //Call the graphing method (function)
}

void drawVelocity() {
  //Pretty much the same as previous
  image(graphBackground, 0, 0);
  
  if (!start) {
     button(startButton.X1, startButton.Y1, startButton.X2, startButton.Y2, startButton.colour, startButton.pressedColour, startButton.strokeColour, startButton.buttonText, startButton.textColour, startButton.textSize); 
  }
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphVelocity();
}

void drawAltitude() {
  //Same as previous
  image(graphBackground, 0, 0);
  
  if (!start) {
     button(startButton.X1, startButton.Y1, startButton.X2, startButton.Y2, startButton.colour, startButton.pressedColour, startButton.strokeColour, startButton.buttonText, startButton.textColour, startButton.textSize); 
  }
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphAltitude();
}

void drawAllChannels() {
  //Same as previous but will altitude, velocity, and acceleration
  image(graphBackground, 0, 0);
  
  if (!start) {
     button(startButton.X1, startButton.Y1, startButton.X2, startButton.Y2, startButton.colour, startButton.pressedColour, startButton.strokeColour, startButton.buttonText, startButton.textColour, startButton.textSize); 
  }
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphAcceleration();
  graphVelocity();
  graphAltitude();
}

void drawReport() {
  //I still actually have to do this one...
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);

  if (tripOver) {
    
  } else {
    textSize(60);
    text("Computing...", 1280/2, 720/2 - 50);
    textSize(20);
    text("Waiting For Flight To End", 1280/2, 720/2);
  }

}

void drawLive() {
  //For drawing the live tilt view
  translate(width/2, height/2, 0);
  //Processing does its magic stuff (translate to 3D render)
  background(48);
  textSize(22);
  textAlign(CENTER, CENTER);
  fill(225);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch), 0, 265);
  //Display roll and pitch
  
  backButton.X1 -= 640;
  backButton.X2 -= 640;
  backButton.Y1 -= 360;
  backButton.Y2 -= 360;
  
  homeButton.X1 -= 640;
  homeButton.X2 -= 640;
  homeButton.Y1 -= 360;
  homeButton.Y2 -= 360;
  
  //annoyingly the coordinate system in the 3D renderer is different and these values must be converted
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);

  backButton.X1 += 640;
  backButton.X2 += 640;
  backButton.Y1 += 360;
  backButton.Y2 += 360;
  
  homeButton.X1 += 640;
  homeButton.X2 += 640;
  homeButton.Y1 += 360;
  homeButton.Y2 += 360;
  
  //Change them back

  rotateX(radians(roll));
  rotateZ(radians(-pitch));
  //Processing does the heavy lifting here, these functions rotate the rocket that is about to be drawn
  
  textSize(30);
  stroke(100);
  fill(145);
  drawCylinder(20, 40, 300);
  //Draw the rocket body
  drawCone(25, 40, 1, 100, -200);
  //Draw the nose
  triangle3D(-100,150,100,150,0,-50);
  //Draw the fin
}

void serialEvent (Serial myPort) {
  //For reading incoming serial data
  
  data = myPort.readStringUntil('\n');

  if (data != null) {
    data = trim(data);
    String items[] = split(data, '/');
    //If the data exists then split it at every '/' character
    if (items.length > 1) {

      roll = float(items[0]);
      pitch = float(items[1]);
      acceleration = g * float(items[2]);
      //Assign variables
      
      if (acceleration == -0.1962) acceleration = 0;
      //This is an annoying value that occurs even when at rest and must be corrected
      
      updateReadings();
      //Update velocity and altitude
      
    }
  }
}

void updateReadings() {
  int currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0;
  //Calculus!!!
  
  velocity = 0.3*(velocity + ((acceleration + prevAcceleration) / 2) * dt)+0.7*prevVelocity;
  /*
  A few things happen here:
  
  1. trapazoidal approximation is used to update the velocity from the acceleration reading
  2. a low-pass filter is used to stop any bad readings from changing the velocity too fast,
  there need to be multiple readings to change the velocity by a lot. This is a tradeoff, but
  necessary with low-cost accelerometers
  */
  
  altitude = 0.3*(altitude + ((velocity + prevVelocity) / 2) * dt) + 0.7*prevAltitude;
  //Same for altitude except we intigrate with velocity
  
  prevTime = currentTime;
  prevAcceleration = acceleration;
  prevVelocity = velocity;
  prevAltitude = altitude;
  //Update variables to hold previous readings
}

void logReadings() {
 println("Acceleration: " + acceleration + ", Velocity: " + velocity + ", Altitude: " + altitude); 
}

void logForExport() {
 println(acceleration);
}

void mousePressed() {
  //Every time the mouse is clicked do this...
  if (mouseX > homeButton.X1 && mouseX < homeButton.X2 && mouseY < homeButton.Y2 && mouseY > homeButton.Y1) {
    //If the mouse is over the specified button, then set the page to the page that button directs to
    //Also update the previous page variables
    prevPrevPage = prevPage;
    prevPage = page;
    page = "home";
    //Most mouse-pressed events are the same as this
  } else if (mouseX > backButton.X1 && mouseX < backButton.X2 && mouseY < backButton.Y2 && mouseY > backButton.Y1) {
    page = prevPage;
    prevPage = prevPrevPage;
  } else if (mouseX > aButton.X1 && mouseX < aButton.X2 && mouseY < aButton.Y2 && mouseY > aButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "acceleration";
  } else if (mouseX > vButton.X1 && mouseX < vButton.X2 && mouseY < vButton.Y2 && mouseY > vButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "velocity";
  } else if (mouseX > dButton.X1 && mouseX < dButton.X2 && mouseY < dButton.Y2 && mouseY > dButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "altitude";
  } else if (mouseX > allButton.X1 && mouseX < allButton.X2 && mouseY < allButton.Y2 && mouseY > allButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "allChannels";
  } else if (mouseX > repButton.X1 && mouseX < repButton.X2 && mouseY < repButton.Y2 && mouseY > repButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "report";
  } else if (mouseX > liveButton.X1 && mouseX < liveButton.X2 && mouseY < liveButton.Y2 && mouseY > liveButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "live";
  } else if (mouseX > startButton.X1 && mouseX < startButton.X2 && mouseY < startButton.Y2 && mouseY > startButton.Y1) {
    start = true;
    //If the start button is pressed then set the start boolean to true
  }
}

void drawCylinder(int sides, float r, float h) {
  //Drawing a polly-cylinder
  float angle = 360 / sides;
  //Angle is a circle dividied by number of sides used
  float halfHeight = h / 2;
  
  //draw top circle
  beginShape();
  for (int i = 0; i < sides; i++) {
     float x = cos( radians( i * angle ) ) * r;
     float z = sin( radians( i * angle ) ) * r;
     //Use trig to set the x and z components
     //This is a good place to admit that the entire 3D view is technically off-axiz and y and z are swapped
     vertex(x, halfHeight, z);
     //This adds a vertex on the circle at this point
  }
  endShape(CLOSE);
  
  //draw bottom circle
  beginShape();
  //Draws the same circle at the bottom
  for (int i = 0; i < sides; i++) {
     float x = cos( radians( i * angle ) ) * r;
     float z = sin( radians( i * angle ) ) * r;
     vertex( x, -halfHeight, z );    
  }
  endShape(CLOSE);
  
  beginShape(TRIANGLE_STRIP);
  //The sides of the cylinder...
  for (int i = 0; i < sides + 1; i++) {
     float x = cos( radians( i * angle ) ) * r;
     float z = sin( radians( i * angle ) ) * r;
     vertex( x, halfHeight, z);
     vertex( x, -halfHeight, z);
     //makes vertices along the top and bottom circle, processing will fill the sides with triangles
  }
  endShape(CLOSE);
}

void drawCone( int sides, float r1, float r2, float h, float offset) {
  //This one is fun, draws a cone (technically cylinder with variable top and bottom radii)
    float angle = 360 / sides;
    float halfHeight = h / 2;
    
    //top circle
    beginShape();
    for (int i = 0; i < sides; i++) {
        float x = cos( radians( i * angle ) ) * r1;
        float z = sin( radians( i * angle ) ) * r1;
        vertex( x, halfHeight + offset, z);
    }
    endShape(CLOSE);
    
    //bottom circle
    beginShape();
    for (int i = 0; i < sides; i++) {
        float x = cos( radians( i * angle ) ) * r2;
        float z = sin( radians( i * angle ) ) * r2;
        vertex( x, -halfHeight + offset, z);
    }
    endShape(CLOSE);
    //draw sides
    beginShape(TRIANGLE_STRIP);
    for (int i = 0; i < sides + 1; i++) {
        float x1 = cos( radians( i * angle ) ) * r1;
        float z1 = sin( radians( i * angle ) ) * r1;
        float x2 = cos( radians( i * angle ) ) * r2;
        float z2 = sin( radians( i * angle ) ) * r2;
        vertex( x1, halfHeight + offset, z1);
        vertex( x2, -halfHeight + offset, z2);
        //Same as cylinder really but with 2 radii
    }
    endShape(CLOSE);
}

void triangle3D(float x1, float y1, float x2, float y2, float x3, float y3) {
  beginShape();
  vertex(x1, y1);
  vertex(x2, y2);
  vertex(x3, y3);
  endShape(CLOSE);
  //Draws a basic trangle with the 3D engine
}

void graphAcceleration() {
  //Graphs acceleration
  for(int i = 0; i < count; i++) {
    //For every data-point stored so far...
    stroke(201, 139, 58);
    strokeWeight(4);
    //Set the stroke colour and weight
    if (i + 1 < 1100) {
      //If on the page
      line(i + 1 + 110, accelerationHistory[i + 1] * -10 + 375, i + 110, accelerationHistory[i] * -10 + 375);
      //Draw a line from the previous point to the new point (with a scale factor)
    }
    strokeWeight(1);
    //Reset stroke weight
  }
}

void graphVelocity() {
  //Same as previous
  for(int i = 0; i < count; i++) {
    stroke(46, 83, 153);
    strokeWeight(4);
    if (i + 1 < 1100) {
      line(i + 1 + 110, velocityHistory[i + 1] * -2 + 375, i + 110, velocityHistory[i] * -2 + 375);
    }
    strokeWeight(1);
  }
}

void graphAltitude() {
  //Same as previous
  for(int i = 0; i < count; i++) {
    stroke(168, 50, 39);
    strokeWeight(4);
    if (i + 1 < 1100) {
      line(i + 1 + 110, -altitudeHistory[i + 1] + 375, i + 110, -altitudeHistory[i] + 375);
    }
    strokeWeight(1);
  }
}
//Wow... you made it!
//The data-parsing makes a lot more sense if you look at the transmitter code... I hope I remembered to add that to the GitHub :/
