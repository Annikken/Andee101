/************************************************************
  [ Andee - with Arduino/Genuino 101 ]
  ================
  Lesson 06
  Creating A Analog Display
  by Robin Ha <robin@annikken.com>

  Check out our Resources section for more information and 
  ideas on what you can do with the Annikken Andee 101!
  http://resources.annikken.com
************************************************************/ 

// Always include these libraries. Annikken Andee needs them
// to work with the Arduino!
#include <CurieBLE.h>
#include <Andee101.h>

// Every object that appears on your smartphone's screen
// needs to be declared like this:
Andee101Helper objectA;
Andee101Helper objectB;
int H, M, S;
char text[64];
int value = 50;

// We're creating two objects

// The setup() function is meant to tell Arduino what to do
// only when it starts up.
void setup()
{
  Serial.begin(9600);
  Andee101.setName("Andee101");
  Andee101.begin();  // Setup communication between  Andee and Arduino
  Andee101.clear();  // Clear the screen of any previous displays
  setInitialData();  // Define object types and their appearance
}

// This is the function meant to define the types and the apperance of
// all the objects on your smartphone
void setInitialData()
{ 
  memset(text, 0x00, 32);
  objectA.setId(0);  // Each object must have a unique ID number
  objectA.setType(ANALOG_DIAL_OUT);  // This defines your object as a display box
  objectA.setCoord(10, 10, 80, 60);
  /* setCoord (x-coord, y-coord, width, height)
     x-coord: From 0.0 (left-most) to 100.0 (right-most)
     y-coord: From 0.0 (top-most)  to 100.0 (bottom-most)
     width:   10.0 (10% of screen width) to 100.0 (100% of screen width)
     height:  10.0 (10% of screen height) to 100.0 (100% of screen height)
  */
  sprintf(text, "%i", value);
  objectA.setTitle("Analog Dial");
  objectA.setData(text);
  objectA.setUnit("Units");
  objectA.setTitleTextColor("FF000000");
  objectA.setTitleColor("50FFFFFF");
  objectA.setTextColor("FF000000");
  objectA.setColor("FFFF8000");
  objectA.setMinMax(0, 100);

}

// Arduino will run instructions here repeatedly until you power it off.
void loop()
{
  if (Andee101.isConnected()==true){ 
  objectA.update(); // Call update() to refresh the display on your screen
  value = random(0, 100);
  sprintf(text, "%i", value);
  objectA.setData(text);  
  delay(2000);
  }
}



