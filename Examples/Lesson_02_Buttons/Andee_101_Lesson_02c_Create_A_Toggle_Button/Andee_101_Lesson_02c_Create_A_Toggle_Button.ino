/************************************************************
  [ Andee101 - with Arduino 101 ]
  ================
  Lesson 02c
  Creating a Toggle Button
  
  Check out our Resources section for more information and 
  ideas on what you can do with the Annikken Andee!
  http://resources.annikken.com

  Contact us at andee@annikken.com if there are 
  bugs in this sketch or if you need help with the 
  Annikken Andee
************************************************************/
/* A toggle button is a button that changes between two states
  when pressed, e.g. On and off. */

// Always include these libraries. 
#include <CurieBLE.h>
#include <Andee101.h>

// We'll just create one button to toggle
Andee101Helper togglebutton;

int state; // This variable will store the current state


void setup() {
  Andee101.begin();  // Setup the Arduino 101 to start broadcasting as an Annikken Andee101 peripheral 
  Andee101.clear();  // Clear the screen of any previous displays 
  setInitialData(); // Define widget types and their appearance
  
  state = 0; // Initialise your state to zero
}

// This is the function meant to define the types and the appearance of
// all the objects on your smartphone
void setInitialData()
{
  // Let's draw a toggle button
  togglebutton.setId(0); // Don't forget to assign a unique ID number
  togglebutton.setType(BUTTON_IN); // Defines object as a button
  togglebutton.setCoord(0,0,100,25);
  togglebutton.setTitle("Turn On"); // Sets the initial words for button
  togglebutton.setColor(GREEN);
  // You can't use setData() and setUnit() for buttons.
}

// Arduino will run instructions here repeatedly until you power it off
void loop() {
  if(Andee101.isConnected() == true)//Use this to tell the Arduino what to do when the Annikken Andee101 app has connected to it
  {
    // Here's how you code the button action
    if( togglebutton.isPressed() )
    {
      // Prevent user from accidentally pressing the button again
      // until Arduino has sent an acknowledgement 
      togglebutton.ack(); 
      state = !state; // Change state
      if(state == 0)
      {
        togglebutton.setTitle("Turn off"); 
        togglebutton.setColor(RED);   
        // Add additional actions here to turn on something
      }
      else
      {
        togglebutton.setTitle("Turn on"); 
        togglebutton.setColor(GREEN); 
        // Add additional actions here to turn off something
      }  
    }
    
    togglebutton.update(); // Update your button to reflect the change in state
  }
}
