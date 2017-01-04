/************************************************************
  [ Andee - with Arduino 101]
  ================
  Lesson 02h
  Displaying the various Button Subtypes
  by Hasif <hasif@annikken.com>

  Check out our Resources section for more information and 
  ideas on what you can do with the Annikken Andee!
  http://resources.annikken.com
************************************************************/
// Always include these libraries. Annikken Andee101 needs them
// to work with the Arduino!
#include <CurieBLE.h>
#include <Andee101.h>

Andee101Helper displayBtn; 
Andee101Helper btn1;
Andee101Helper btn2;
Andee101Helper btn3;
Andee101Helper btn4;
Andee101Helper btn5;
Andee101Helper btn6;

bool UpdateFlag = false;

void setup() {
  Andee101.begin();// Setup communication between Annikken Andee101 and Arduino
  setInitialData();// Define object types and their appearance
}

void setInitialData()
{ 
  
  displayBtn.setId(1); // Each object must have a unique ID number
  displayBtn.setType(DATA_OUT);// Defines object as a display box
  displayBtn.setCoord(3,3,94,42); // Sets the location and size of your object
  displayBtn.setTitle("");// Sets the title of the object
  displayBtn.setData("");  
  displayBtn.setColor("FF00B2B2");//Sets the colour of the UI object
  displayBtn.setTitleColor("FF00D2D2");
  displayBtn.setTextColor(WHITE);
  displayBtn.setTitleTextColor(WHITE);

  btn1.setId(2);
  btn1.setType(BUTTON_IN);
  btn1.setCoord(3,48,29,24);
  btn1.setTitle("Press Me!");
  btn1.setColor(RED);
  btn1.setSubType(ROUND_RECT_NO_BG);

  btn2.setId(3);
  btn2.setType(BUTTON_IN);
  btn2.setCoord(35,48,29,24);
  btn2.setTitle("Press Me!");
  btn2.setColor(BLUE);
  btn2.setSubType(ROUND_RECT_BG);
  
  btn3.setId(4);
  btn3.setType(BUTTON_IN);
  btn3.setCoord(67,48,29,24);
  btn3.setTitle("Press Me!");
  btn3.setColor(GREEN);
  btn3.setSubType(RECT_NO_BG);

  btn4.setId(5);
  btn4.setType(BUTTON_IN);
  btn4.setCoord(3,74,29,24);
  btn4.setTitle("Press Me!");
  btn4.setColor(ORANGE);
  btn4.setSubType(RECT_BG);

  btn5.setId(6);
  btn5.setType(BUTTON_IN);
  btn5.setCoord(35,74,29,24);
  btn5.setTitle("Press Me!");
  btn5.setColor(INDIGO);
  btn5.setSubType(CIRCLE_NO_BG);

  btn6.setId(7);
  btn6.setType(BUTTON_IN);
  btn6.setCoord(67,74,29,24);
  btn6.setTitle("Press Me!");
  btn6.setColor(VIOLET);
  btn6.setSubType(CIRCLE_BG);
}
  

void loop() {
  // put your main code here, to run repeatedly:
  if(Andee101.isConnected() == true)
  {
    if(UpdateFlag == false)
    {
      for(int i = 0; i < 2; i++)
      { 
        displayBtn.update();
        btn1.update();
        btn2.update();
        btn3.update();
        btn4.update();
        btn5.update();
        btn6.update();
      }      
      UpdateFlag = true;
    }
    if(btn1.isPressed())
    {
      btn1.ack();
      displayBtn.setTitle("This is a");
      displayBtn.setData("Rounded Rect with"); 
      displayBtn.setUnit("No Background Color");
      displayBtn.update();
    }
    if(btn2.isPressed())
    {
      btn2.ack();
      displayBtn.setTitle("This is a");
      displayBtn.setData("Rounded Rect with");
      displayBtn.setUnit("Background Color");
      displayBtn.update();
    }
    if(btn3.isPressed())
    {
      btn3.ack();
      displayBtn.setTitle("This is a");
      displayBtn.setData("Rect with");
      displayBtn.setUnit("No Background Color");
      displayBtn.update();
    }
    if(btn4.isPressed())
    {
      btn4.ack();
      displayBtn.setTitle("This is a");
      displayBtn.setData("Rect with");
      displayBtn.setUnit("Background Color");
      displayBtn.update();
    }
    if(btn5.isPressed())
    {
      btn5.ack();
      displayBtn.setTitle("This is a");
      displayBtn.setData("Circle with");
      displayBtn.setUnit("No Background Color");
      displayBtn.update();
    }
    if(btn6.isPressed())
    {
      btn6.ack();
      displayBtn.setTitle("This is a");
      displayBtn.setData("Circle with");
      displayBtn.setUnit("Background Color");
      displayBtn.update();
    } 
  }
  else{
    UpdateFlag = false; 
  }
}
