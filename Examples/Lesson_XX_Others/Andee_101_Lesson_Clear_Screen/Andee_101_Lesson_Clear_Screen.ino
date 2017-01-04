/************************************************************
  [ Andee - with Arduino/Genuino 101 ]
  ================
  Clear Screen

  Use the Clear Screen command to everything on the Dashboard
  by Robin Ha <robin@annikken.com>

  Check out our Resources section for more information and
  ideas on what you can do with the Annikken Andee 101!
  http://resources.annikken.com
************************************************************/

// Always include these libraries.  Andee needs them
// to work with the Arduino!
#include <CurieBLE.h>
#include <Andee101.h>

Andee101Helper ClearButton;
Andee101Helper objectA;
Andee101Helper objectB;
Andee101Helper objectC;
boolean updateUI = false;
void setup()
{
  Serial.begin(9600);
  Andee101.setName("Andee101"); // Max 8 characters only
  Andee101.begin();  // Setup the Arduino 101 to start broadcasting as an Annikken Andee101 peripheral
  setInitialData();  // Define the UI objects and customise their appearance
}
 
void setInitialData()
{
 
  ClearButton.setId(0);
  ClearButton.setType(BUTTON_IN);
  ClearButton.setCoord(79, 79, 20, 20);
  ClearButton.setTitle("Clear Screen");
  ClearButton.setColor("FFFF8000");

  objectA.setId(1);
  objectA.setType(DATA_OUT);
  objectA.setCoord(2, 2, 96, 23);
  objectA.setTitle("Obj A");
  objectA.setData("");
  objectA.setUnit("");
  objectA.setTitleTextColor("FF53868B");
  objectA.setTitleColor("FF00C5CD");
  objectA.setTextColor("FF53868B");
  objectA.setColor("FF00F5FF");

  objectB.setId(2);
  objectB.setType(DATA_OUT);
  objectB.setCoord(2, 27, 96, 23);
  objectB.setTitle("Obj B");
  objectB.setData("");
  objectB.setUnit("");
  objectB.setTitleTextColor("FF53868B");
  objectB.setTitleColor("FF00C5CD");
  objectB.setTextColor("FF53868B");
  objectB.setColor("FF00F5FF");

  objectC.setId(3);
  objectC.setType(DATA_OUT);
  objectC.setCoord(2, 52, 96, 23);
  objectC.setTitle("Obj C");
  objectC.setData("");
  objectC.setUnit("");
  objectC.setTitleTextColor("FF53868B");
  objectC.setTitleColor("FF00C5CD");
  objectC.setTextColor("FF53868B");
  objectC.setColor("FF00F5FF");
}
 
void loop()
{

  if (Andee101.isConnected() == true)
  {
    if (updateUI == true) {
      // Draw the UI
      objectA.update();
      objectB.update();
      objectC.update();
      ClearButton.update();
      updateUI = false;
    }
    if (ClearButton.isPressed()) {
      ClearButton.ack();
      Andee101.clear(); // Remove all the UI on the Screen
      updateUI = true; // set the flag to redraw the UI to True
      delay(3000); // Wait for 3s before redrawing
    }
  }
  else {
    updateUI = true;
  }



}



