/************************************************************
  [ Andee - with Arduino 101]
  ================
  Lesson 02a
  Creating Your First Button
  by Robin Ha <robin@annikken.com>

  Check out our Resources section for more information and
  ideas on what you can do with the  Andee!
  http://resources..com

  Check out our Blog for regular updates and fun!
  http://.com/blog
************************************************************/

// Always include these libraries. Annikken Andee needs them
// to work with the Arduino!
#include <CurieBLE.h>
#include <Andee101.h>
#include <Servo.h> 

// Every object that appears on your smartphone's screen
// needs to be declared like this:
Andee101Helper objectA;
Andee101Helper objectB;

int ButtonCounter = 0;

// We're creating two objects

// The setup() function is meant to tell Arduino what to do
// only when it starts up.
void setup()
{
  Serial.begin(9600);
  Andee101.setName("Andee101"); // Max 8 characters only
  Andee101.begin();  // Setup the Arduino 101 to start broadcasting as an Annikken Andee101 peripheral 
  setInitialData();  // Define the UI objects and customise their appearance
}

// This is the function meant to define the types and the apperance of
// all the objects on your smartphone
void setInitialData()
{
  // This is to create the Data_Out Box (See Lesson 01)
  objectA.setId(0);  // Each object must have a unique ID number
  objectA.setType(DATA_OUT);  // This defines your object as a display box
  objectA.setCoord(0, 10, 100  , 20);
  /* setCoord (x-coord, y-coord, width, height)
     x-coord: From 0.0 (left-most) to 100.0 (right-most)
     y-coord: From 0.0 (top-most)  to 100.0 (bottom-most)
     width:   10.0 (10% of screen width) to 100.0 (100% of screen width)
     height:  10.0 (10% of screen height) to 100.0 (100% of screen height)
  */

  objectA.setTitle("The Button has been Pressed");
  objectA.setData(ButtonCounter);
  objectA.setUnit("Time");
  objectA.setTitleTextColor("FFFFFFFF");
  objectA.setTitleColor("50FFFFFF");
  objectA.setTextColor("FFFFFFFF");
  objectA.setColor("FFFF8000");
  
  //// Let's draw the Button
  objectB.setId(1);  // Each object must have a unique ID number
  objectB.setType(BUTTON_IN);  // This defines your object as a BUTTON
  objectB.setCoord(35, 50, 30, 30);
  objectB.setTitle("Click me");
  objectB.setColor("FFFF8000");
}
 
void loop()
{
  if (Andee101.isConnected() == true)
  { 
    objectA.update(); // Call update() to refresh the display on your screen
    objectB.update(); // If you forgot to call update(), your object won't appear

    if (objectB.isPressed()) {
      objectB.ack(); // Ack will clear the black shroud covering the button once it has been pressed
      ButtonCounter++; 
      objectA.setData(ButtonCounter);
      objectA.update();
    } 
  }
}


