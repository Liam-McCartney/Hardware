import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

PFont spaceAgeFont;
PImage logo;
PImage graphBackground;

Serial myPort;

String data="";
float roll, pitch, acceleration, velocity, altitude;
float prevVelocity = 0;
float prevAcceleration = 0;
float prevAltitude = 0;
int prevTime = 0;

float g = 9.81;

String page = "home";
String prevPage = "home";
String prevPrevPage = "home";

boolean tripOver = false;
boolean serialConnection = true;

Button homeButton = new Button();
Button backButton = new Button();
Button aButton = new Button();
Button vButton = new Button();
Button dButton = new Button();
Button allButton = new Button();
Button repButton = new Button();
Button liveButton = new Button();

color backgroundColour = color(48,48,48);

float[] accelerationHistory = new float[1280];
float[] velocityHistory = new float[1280];
float[] altitudeHistory = new float[1280];

float tenthTime = 0;
int pastTenthTime = 0;
int count = 1;

void setup() {
  size(1280, 720, P3D);
  
  try {
  myPort = new Serial(this, "COM3", 9600); // starts the serial communication
  myPort.bufferUntil('\n');
  } catch (RuntimeException e) {
    println("No Serial Connection! Live Data Tracking is Unavailable");
    serialConnection = false;
  }
  
  background(backgroundColour);
  
  spaceAgeFont = createFont("spaceAge.ttf", 64);
  textFont(spaceAgeFont);
  
  logo = loadImage("rocket.png");
  graphBackground = loadImage("graphBackground.png");
  
  
  accelerationHistory[0] = 0;
  velocityHistory[0] = 0;
  altitudeHistory[0] = 0;
}

public class Button {
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

//public class Page extends Button {
 // Button button;
//}

void draw() {
  background(backgroundColour);
  
  logReadings();
  
  if (page == "home") {
   drawHome(); 
  } else if(page == "acceleration") {      // page.equals("acceleration");
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
  
  tenthTime = millis() / 100.0;
  
  if (pastTenthTime < int(tenthTime) && count < 1100) {
    accelerationHistory[count] = acceleration;
    velocityHistory[count] = velocity;
    altitudeHistory[count] = altitude;
    ++count;
  }
  
  pastTenthTime = int(tenthTime);
  
}

void button(int X1, int Y1, int X2, int Y2, color colour, color pressedColour, color strokeColour, String buttonText, color textColour, int textSize) {
  
  boolean mouseOverButton;
  
  if(mouseX > X1 && mouseX < X2 && mouseY < Y2 && mouseY > Y1) {
   mouseOverButton = true;
   fill(pressedColour);
  } else {
    mouseOverButton = false;
    fill(colour);
  }
  
  stroke(strokeColour);
  rect(X1, Y1, X2 - X1, Y2 - Y1);
  
  textAlign(CENTER, CENTER);
  
  fill(textColour);
  textSize(textSize);
  text(buttonText, (X1 + X2) / 2, (Y1 + Y2) / 2);
}

void drawHome() {
  
  if(!serialConnection) {
   textSize(20);
   fill(255,40,40);
   text("WARNING: No Serial Data Connection", 1015, 700); 
  }
  
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
  
  
  image(logo, 30, 40);
  
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  button(aButton.X1, aButton.Y1, aButton.X2, aButton.Y2, aButton.colour, aButton.pressedColour, aButton.strokeColour, aButton.buttonText, aButton.textColour, aButton.textSize);
  button(vButton.X1, vButton.Y1, vButton.X2, vButton.Y2, vButton.colour, vButton.pressedColour, vButton.strokeColour, vButton.buttonText, vButton.textColour, vButton.textSize);
  button(dButton.X1, dButton.Y1, dButton.X2, dButton.Y2, dButton.colour, dButton.pressedColour, dButton.strokeColour, dButton.buttonText, dButton.textColour, dButton.textSize);
  button(allButton.X1, allButton.Y1, allButton.X2, allButton.Y2, allButton.colour, allButton.pressedColour, allButton.strokeColour, allButton.buttonText, allButton.textColour, allButton.textSize);
  button(repButton.X1, repButton.Y1, repButton.X2, repButton.Y2, repButton.colour, repButton.pressedColour, repButton.strokeColour, repButton.buttonText, repButton.textColour, repButton.textSize);
  button(liveButton.X1, liveButton.Y1, liveButton.X2, liveButton.Y2, liveButton.colour, liveButton.pressedColour, liveButton.strokeColour, liveButton.buttonText, liveButton.textColour, liveButton.textSize);
}

void drawAcceleration() {
  image(graphBackground, 0, 0);
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphAcceleration();
}

void drawVelocity() {
  image(graphBackground, 0, 0);
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphVelocity();
}

void drawAltitude() {
  image(graphBackground, 0, 0);
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphAltitude();
}

void drawAllChannels() {
  image(graphBackground, 0, 0);
  button(backButton.X1, backButton.Y1, backButton.X2, backButton.Y2, backButton.colour, backButton.pressedColour, backButton.strokeColour, backButton.buttonText, backButton.textColour, backButton.textSize);
  button(homeButton.X1, homeButton.Y1, homeButton.X2, homeButton.Y2, homeButton.colour, homeButton.pressedColour, homeButton.strokeColour, homeButton.buttonText, homeButton.textColour, homeButton.textSize);
  graphAcceleration();
  graphVelocity();
  graphAltitude();
}

void drawReport() {
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
  translate(width/2, height/2, 0);
  background(48);
  textSize(22);
  textAlign(CENTER, CENTER);
  fill(225);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch), 0, 265);
  
  backButton.X1 -= 640;
  backButton.X2 -= 640;
  backButton.Y1 -= 360;
  backButton.Y2 -= 360;
  
  homeButton.X1 -= 640;
  homeButton.X2 -= 640;
  homeButton.Y1 -= 360;
  homeButton.Y2 -= 360;
  
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

  // Rotate the object
  rotateX(radians(roll));
  rotateZ(radians(-pitch));
  
  // 3D 0bject
  textSize(30);
  stroke(100);
  fill(145);
  drawCylinder(20, 40, 300);
  drawCone(25, 40, 1, 100, -200);
  triangle3D(-100,150,100,150,0,-50);
}

// Read data from the Serial Port
void serialEvent (Serial myPort) {
  
  data = myPort.readStringUntil('\n');

  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, '/');
    if (items.length > 1) {

      //--- Roll,Pitch in degrees
      roll = float(items[0]);
      pitch = float(items[1]);
      acceleration = g * float(items[2]);
      
      if (acceleration == -0.1962) acceleration = 0;
      //This is an annoying value that occurs even when at rest
      
      updateReadings();
      
    }
  }
}

void updateReadings() {
  int currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0;
  
  velocity = 0.3*(velocity + ((acceleration + prevAcceleration) / 2) * dt)+0.7*prevVelocity;
  
  altitude = 0.3*(altitude + ((velocity + prevVelocity) / 2) * dt) + 0.7*prevAltitude;
  
  prevTime = currentTime;
  prevAcceleration = acceleration;
  prevVelocity = velocity;
  prevAltitude = altitude;
}

void logReadings() {
 println("Acceleration: " + acceleration + ", Velocity: " + velocity + ", Altitude: " + altitude); 
}

void mousePressed() {
  if (mouseX > homeButton.X1 && mouseX < homeButton.X2 && mouseY < homeButton.Y2 && mouseY > homeButton.Y1) {
    prevPrevPage = prevPage;
    prevPage = page;
    page = "home";
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
  }
}

void drawGraph() {
  stroke(128);
  strokeWeight(3);
  line(80,50,80,670);
  
  line(80,(50+670)/2, 1200, (50+670)/2);
  
  strokeWeight(1);
  
  int repeatHoriz = 7;
  for(int i = 0; i < repeatHoriz; i++) {
    line(80 + ((1200-80)/(repeatHoriz+1))*(i+1), 65, 80 + ((1200-80)/(repeatHoriz+1))*(i+1), 655);
  }
  
  int repeatVert = 5;
  for(int i = 0; i < repeatVert; i++) {
    line(80, ((655-65)/(repeatVert))*(i+1)+5, 1185, ((655-65)/(repeatVert))*(i+1)+5);
  }
}

void drawCylinder(int sides, float r, float h) {
  float angle = 360 / sides;
  float halfHeight = h / 2;
  // draw top shape
  beginShape();
  for (int i = 0; i < sides; i++) {
     float x = cos( radians( i * angle ) ) * r;
     float z = sin( radians( i * angle ) ) * r;
     vertex( x, halfHeight, z );    
  }
  endShape(CLOSE);
  // draw bottom shape
  beginShape();
  for (int i = 0; i < sides; i++) {
     float x = cos( radians( i * angle ) ) * r;
     float z = sin( radians( i * angle ) ) * r;
     vertex( x, -halfHeight, z );    
  }
  endShape(CLOSE);
  beginShape(TRIANGLE_STRIP);
  for (int i = 0; i < sides + 1; i++) {
     float x = cos( radians( i * angle ) ) * r;
     float z = sin( radians( i * angle ) ) * r;
     vertex( x, halfHeight, z);
     vertex( x, -halfHeight, z);    
  }
  endShape(CLOSE);
}

void drawCone( int sides, float r1, float r2, float h, float offset) {
    float angle = 360 / sides;
    float halfHeight = h / 2;
    // top
    beginShape();
    for (int i = 0; i < sides; i++) {
        float x = cos( radians( i * angle ) ) * r1;
        float z = sin( radians( i * angle ) ) * r1;
        vertex( x, halfHeight + offset, z);
    }
    endShape(CLOSE);
    // bottom
    beginShape();
    for (int i = 0; i < sides; i++) {
        float x = cos( radians( i * angle ) ) * r2;
        float z = sin( radians( i * angle ) ) * r2;
        vertex( x, -halfHeight + offset, z);
    }
    endShape(CLOSE);
    // draw body
    beginShape(TRIANGLE_STRIP);
    for (int i = 0; i < sides + 1; i++) {
        float x1 = cos( radians( i * angle ) ) * r1;
        float z1 = sin( radians( i * angle ) ) * r1;
        float x2 = cos( radians( i * angle ) ) * r2;
        float z2 = sin( radians( i * angle ) ) * r2;
        vertex( x1, halfHeight + offset, z1);
        vertex( x2, -halfHeight + offset, z2);
    }
    endShape(CLOSE);
}

void triangle3D(float x1, float y1, float x2, float y2, float x3, float y3) {
  beginShape();
  vertex(x1, y1);
  vertex(x2, y2);
  vertex(x3, y3);
  endShape(CLOSE);
}

void graphAcceleration() {
  for(int i = 0; i < count; i++) {
    stroke(201, 139, 58);
    strokeWeight(4);
    if (i + 1 < 1100) {
    line(i + 1 + 110, accelerationHistory[i + 1] * -25 + 375, i + 110, accelerationHistory[i] * -25 + 375);
    }
    strokeWeight(1);
  }
}

void graphVelocity() {
  for(int i = 0; i < count; i++) {
    stroke(46, 83, 153);
    strokeWeight(4);
    if (i + 1 < 1100) {
      line(i + 1 + 110, velocityHistory[i + 1] * -5 + 375, i + 110, velocityHistory[i] * -5 + 375);
    }
    strokeWeight(1);
  }
}

void graphAltitude() {
  for(int i = 0; i < count; i++) {
    stroke(168, 50, 39);
    strokeWeight(4);
    if (i + 1 < 1100) {
      line(i + 1 + 110, -altitudeHistory[i + 1] + 375, i + 110, altitudeHistory[i] * -1 + 375);
    }
    strokeWeight(1);
  }
}
