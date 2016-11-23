/************************************************************
  [ Andee - with Arduino/Genuino 101 ]
  ================ 
  Take a Photo
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
Andee101Helper objectC;
Andee101Helper objectD;



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

void setInitialData()
{
  objectA.setId(0);  // Each object must have a unique ID number
  objectA.setType(BUTTON_IN);  // This defines your object as a BUTTON
  objectA.setCoord(0, 0, 100, 25);
  objectA.setTitle("Back Camera, Flash On, Auto Focus");
  objectA.setColor("FF00F800");

  objectB.setId(1);  // Each object must have a unique ID number
  objectB.setType(BUTTON_IN);  // This defines your object as a BUTTON
  objectB.setCoord(0, 25, 100, 25);
  objectB.setTitle("Back Camera, Flash Off, No Focus");
  objectB.setColor("FFFF8000");

  objectC.setId(2);  // Each object must have a unique ID number
  objectC.setType(BUTTON_IN);  // This defines your object as a BUTTON
  objectC.setCoord(0, 50, 100, 25);
  objectC.setTitle("Front Camera, Flash Off, Auto Focus");
  objectC.setColor("FF0000F8");

  objectD.setId(3);  // Each object must have a unique ID number
  objectD.setType(BUTTON_IN);  // This defines your object as a BUTTON
  objectD.setCoord(0, 75, 100, 25);
  objectD.setTitle("Front Camera, Flash On, No Focus");
  objectD.setColor("FF00F8F8");
}

// Arduino will run instructions here repeatedly until you power it off.
void loop()
{
  if (Andee101.isConnected() == true) {
    objectA.update(); // Call update() to refresh the display on your screen
    objectB.update(); // If you forgot to call update(), your object won't appear
    objectC.update();
    objectD.update();
    if (objectA.isPressed()) {
      objectA.ack();
      Andee101.takePhoto(CAM_DEFAULT, On, On);
    }
    else if (objectB.isPressed()) {
      objectB.ack();
      Andee101.takePhoto(CAM_DEFAULT, Off, Off);
    }
    else if (objectC.isPressed()) {
      objectC.ack();
      Andee101.takePhoto(FRONT, Off, On);
    }
    else if (objectD.isPressed()) {
      objectD.ack();
      Andee101.takePhoto(FRONT, On, Off);
    }
    // A short delay is necessary to give Andee time to communicate with the smartphone
    delay(500);
  }
}



