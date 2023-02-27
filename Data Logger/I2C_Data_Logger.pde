import processing.serial.*;
Serial port;    // Create an object from Serial class
String val;     // Data received from the serial port

final int Length = 1200;
final int Height = 800;    //Height, Lenght, and Indent
final int indent = 30;

void setup() {
  size(1200, 800);    //Set Window Size
  background(255);    //Set background colour
  stroke(0);          //Set stroke colour
  frameRate(30);
  port = new Serial(this, "/dev/cu.usbserial-A10L64ER", 9600);
  //Make the port

  line(indent, 0, indent, Height);
  line(Length, Height - indent, 0, Height - indent);
  //Axis

  fill(0);
  textSize(15);
  
  text("50", indent / 2, indent / 2);
  
  text("25", indent / 2, Height - Height / 2 - indent / 2);
  text("0", indent / 2, Height - indent / 2);
  
  text("12h", Length - Length / 2 + indent, Height - indent / 2);
  text("18h", Length - Length / 4 + indent, Height - indent / 2);
  text("6h", Length - 3 * Length / 4 + indent, Height - indent / 2);
  //Axis labels
}

int x = indent;
int intVal;
int n;

void draw() {
  if ( port.available() != 0)
  { // If data is available,
    val = port.readStringUntil('\n');         // read it and store it in val
    if (val != null) {
      val = trim(val);
      intVal = int(val);
      //Convert the read serial data to an int

      float floatVal = map(intVal, 50, 0, Height, 0);
      //Map the int

      point(x++, Height - floatVal + indent);
      //graph the int
    }
  }
}
