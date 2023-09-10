//Project : CHUMP Compiler
//Date    : Sep 9, 2023
//Author  : Liam McCartney

/***************************************************************************************************
Hi,

I have no idea how many people will ever see this, but seeing as this tool could be useful in the
future for others working on CHUMP I may send it to some people who ask.

If that is you, then here is a brief description of this tool.

This is a CHUMPanese compiler (or assembler), which means it lets you program in higher level
assembly and this program will convert it into CHUMP machine code for you. The output will go to
processing's terminal/console, but you can of course alter the code to do some more useful stuff
with it.

The basic CHUMP instruction set comes with 7 different commands, each with two flavours. The first
is a constant instruction, denoted with a c suffix (e.g. ADDc). This means the operand will be the
value of the low nibble of the instruction. The other flavour is memory, denoted with m (e.g. ADDm)
where the operand is the value stored at the address of the low nibble of the instruction in memory.

The 7 instructions (technically 14 with the two flavours) are as follows:

LOAD, ADD, SUB, STORE, READ, GOTO, IFZ

An example instruction set would look like something below:

LOADc 1
ADDc 14
STOREc 6
ADDm 6

Note that the program uses decimal, not hex (sorry hex people).

Finally, once you are done your program, select the number of lines you want to compile and press
the compile button to get the machine code.

That should be all you need to know. By the way, if you take a look at the code you will notice that
it is horribly put together. That is because this is my first ever GUI application and I threw it
together in around 4 hours, so please don't judge.

Anyways, have fun!
***************************************************************************************************/


String userInput[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""}; // Store the user's input
int offset = 15;
int current = 0;
int counter = 0;

String LOADc = "0000";
String LOADm = "0001";

String ADDc = "0010";
String ADDm = "0011";

String SUBc = "0100";
String SUBm = "0101";

String STOREc = "0110";
String STOREm = "0111";

String READc = "1000";
String READm = "1001";

String GOTOc = "1100";
String GOTOm = "1101";

String IFZc = "1110";
String IFZm = "1111";

boolean button1Pressed;
boolean button2Pressed;
boolean button3Pressed;

int lines = 0;


void setup() {
  size(720, 390);

  background(255, 255, 255);

  stroke(150, 216, 240);
  rect(0, 0, 720, 64);

  fill(0);
  textSize(20);
  text("Enter Your Code (Remember, a Maximum of 16 Lines): ", 20, 20);
}

//PFont codeFont = loadFont("Menlo-Regular-48.vlw");
//PFont normFont = loadFont("ArialHebrewScholar-Bold-48.vlw");

void draw() {
  textAlign(LEFT, BASELINE);
  background(220);
  fill(180, 180, 180);
  rect(0, 48, 720, 342);
  
  fill(150, 216, 240);
  rect(0, 0, 720, 48);
  rect(390, 193, 310, 145);
  
  fill(165, 165, 165);
  noStroke();
  rect(20, 52 + 20 * current, 340, 20);
  stroke(0);
  fill(0);
  
  text("Suffixes:", 395, 95);
  text("c = Const", 395, 120);
  text("m = mem", 395, 138);
  
   text("Enter Your Code (Remember, a Maximum of 16 Lines): ", 20, 30);
  
  text("Instructions (OP-Codes):", 395, 215);
  
  
  if (lines != 1) {
    text("Lines", 604, 175);
  } else {
    text("Line", 609, 175);
  }
  //It bugs me when people are too lazy to fix stuff with 1 and then a plural
  
  text("Output goes to Processing console*", 390, 370);
  
  text("LOAD", 410, 255);
  text("ADD", 535, 255);
  text("SUB", 645, 255);
  
  text("STORE", 410, 285);
  text("READ", 530, 285);
  text("GOTO", 640, 285);
  
  text("IFZ", 537, 315);
  
  //textFont(codeFont);
  text(userInput[0], 20, 55 + offset);
  text(userInput[1], 20, 75 + offset);
  text(userInput[2], 20, 95 + offset);
  text(userInput[3], 20, 115 + offset);
  text(userInput[4], 20, 135 + offset);
  text(userInput[5], 20, 155 + offset);
  text(userInput[6], 20, 175 + offset);
  text(userInput[7], 20, 195 + offset);
  text(userInput[8], 20, 215 + offset);
  text(userInput[9], 20, 235 + offset);
  text(userInput[10], 20, 255 + offset);
  text(userInput[11], 20, 275 + offset);
  text(userInput[12], 20, 295 + offset);
  text(userInput[13], 20, 315 + offset);
  text(userInput[14], 20, 335 + offset);
  text(userInput[15], 20, 355 + offset);

  String typePlace = "|";
  String placeHolder = userInput[current] + typePlace;

  if (counter > 25) {

    
    fill(165, 165, 165);
    noStroke();

    rect(20, 60 + offset + 20 * (current - 1), 340, 15);

    fill(0);
    stroke(0);

    text(placeHolder, 20, 55 + offset + 20 * current);
  }
  counter = (counter + 1) % 51;

  stroke(0);

  line(20, 71, 360, 71);
  line(20, 91, 360, 91);
  line(20, 111, 360, 111);
  line(20, 131, 360, 131);
  line(20, 151, 360, 151);
  line(20, 171, 360, 171);
  line(20, 191, 360, 191);
  line(20, 211, 360, 211);
  line(20, 231, 360, 231);
  line(20, 251, 360, 251);
  line(20, 271, 360, 271);
  line(20, 291, 360, 291);
  line(20, 311, 360, 311);
  line(20, 331, 360, 331);
  line(20, 351, 360, 351);
  line(20, 371, 360, 371);


  textSize(30);
  if (lines < 10) {
    text(lines, 618, 152);
  } else {
    text(lines, 611, 152);
  }
  textSize(20);


  // Draw the button
  fill(100, 100, 200); // Button color
  rect(575, 75, 100, 40); // Button rectangle


  // Check if the mouse is over the button
  if (mouseX > 575 && mouseX < 675 && mouseY > 75 && mouseY < 115) {
    fill(150, 150, 255); // Highlight the button when the mouse is over it
  } else {
    fill(100, 100, 200); // Use the default button color
  }
  rect(575, 75, 100, 40); // Button rectangle

  rect(575, 130, 25, 25);
  
  
  if (mouseX > 575 && mouseX < 600 && mouseY > 130 && mouseY < 155) {
    fill(150, 150, 255); // Highlight the button when the mouse is over it
  } else {
    fill(100, 100, 200); // Use the default button color
  }

  rect(575, 130, 25, 25);

  rect(650, 130, 25, 25);

  if (mouseX > 650 && mouseX < 675 && mouseY > 130 && mouseY < 155) {
    fill(150, 150, 255); // Highlight the button when the mouse is over it
  } else {
    fill(100, 100, 200); // Use the default button color
  }

  rect(650, 130, 25, 25);

  // Display button text
  fill(255); // Text color
  textAlign(CENTER, CENTER);
  text("Compile", 626, 95);

  if (button1Pressed) {
    compile();
    button1Pressed = false;
  }

  if (button2Pressed) {
    if (lines > 0) lines--;
    button2Pressed = false;
  }

  if (button3Pressed) {
    if (lines < 16) lines++;
    button3Pressed = false;
  }
  fill(150, 150, 255);
  triangle(581, 142, 594, 136, 594, 148);
  triangle(656, 148, 656, 136, 669, 142);
  fill(150, 150, 255);

  
}

//------------------------------------------------------------------------------------------//

void keyPressed() {
  if (key == BACKSPACE && userInput[current].length() > 0) {
    // If the user presses the BACKSPACE key and there's something to delete
    userInput[current] = userInput[current].substring(0, userInput[current].length() - 1);
  } else if (key == BACKSPACE && userInput[current].length() == 0 && current != 0) {
    current--;
  } else if (keyCode == UP && current != 0) {
    current--;
  } else if (keyCode == DOWN && current != 15) {
    current++;
  }
}

void keyTyped() {
  if (key == '\n') {
    current++;
  } else if (key >= ' ' && key <= 'z') {
    // If the typed character is a printable character, add it to userInput
    userInput[current] += key;
  }
}

void mousePressed() {
  // Check if the mouse is over the button when it's pressed
  if (mouseX > 575 && mouseX < 675 && mouseY > 75 && mouseY < 115) {
    button1Pressed = true;
    // You can add your button's action or code here
    // For example, you can perform some action when the button is clicked.
  } else if (mouseX > 575 && mouseX < 600 && mouseY > 130 && mouseY < 155) {
    button2Pressed = true;
  } else if (mouseX > 650 && mouseX < 675 && mouseY > 130 && mouseY < 155) {
    button3Pressed = true;
  } else if (mouseX > 20 && mouseX < 360 && mouseY > 55 && mouseY < 371) {

    int scaledMouse = mouseY - 55;

    int page = scaledMouse / 20;
    current = page;
  }
}

void compile() {
  for (int i = 0; i < lines; i++) {
    String str = userInput[i];
    String[] subStr = str.split(" ");

    if (subStr[0].contains("LOADc")) {
      print(LOADc);
    } else if (subStr[0].contains("LOADm")) {
      print(LOADm);
    } else if (subStr[0].contains("ADDc")) {
      print(ADDc);
    } else if (subStr[0].contains("ADDm")) {
      print(ADDm);
    } else if (subStr[0].contains("SUBc")) {
      print(SUBc);
    } else if (subStr[0].contains("SUBm")) {
      print(SUBm);
    } else if (subStr[0].contains("STOREc")) {
      print(STOREc);
    } else if (subStr[0].contains("STOREm")) {
      print(STOREm);
    } else if (subStr[0].contains("READc")) {
      print(READc);
    } else if (subStr[0].contains("READm")) {
      print(READm);
    } else if (subStr[0].contains("GOTOc")) {
      print(GOTOc);
    } else if (subStr[0].contains("GOTOm")) {
      print(GOTOm);
    } else if (subStr[0].contains("IFZc")) {
      print(IFZc);
    } else if (subStr[0].contains("IFZm")) {
      print(IFZm);
    }

    try {
      int containedInt = Integer.parseInt(subStr[1]);
      String binaryNibble = String.format("%4s", Integer.toBinaryString(containedInt)).replace(' ', '0');
      print(binaryNibble);
    } catch(ArrayIndexOutOfBoundsException e) {
      println("Error Caught! Stop Making Errors Please!");
      println("(This usually means that you are either trying to compile unwanted lines or you have a typo)");
      print("Error was found on line ");
      print(i + 1);
      println(".");
    }
    
  }
}
