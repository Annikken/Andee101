/************************************************************
  [ Andee - with Arduino/Genuino 101 ]
  ================
  Lesson 07
  Creating A Slider Widget
  by Robin Ha <robin@annikken.com>

  Check out our Resources section for more information and 
  ideas on what you can do with the Annikken Andee 101!
  http://resources.annikken.com
************************************************************/ 

// Always include these libraries. Annikken Andee needs them
// to work with the Arduino!
#include <CurieBLE.h>
#include <Andee101.h>

Andee101Helper UIText1;
Andee101Helper UIText2;
Andee101Helper UI1;
Andee101Helper UI2;
int lastA, lastB;

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
  UIText1.setId(0);
  UIText1.setType(DATA_OUT);
  UIText1.setSubType(0);
  UIText1.setCoord(2, 2, 96  , 22);
  UIText1.setTitleTextColor("FF000000");
  UIText1.setTitleColor("50FFFFFF");
  UIText1.setTitle("Slider Value");
  UIText1.setData(50);
  UIText1.setUnit(" ");


  UI1.setId(1);
  UI1.setType(SLIDER_IN);
  UI1.setInputMode(ON_FINGER_UP);
  UI1.setSliderNumIntervals(0);
  UI1.setSubType(0);
  UI1.setCoord(2, 26, 96  , 22);
  UI1.setColor("FFFF9900");
  UI1.setTitle("Continuous Slider");
  UI1.setMinMax(0, 100);
  UI1.setSliderInitialValue(50);

  UIText2.setId(12);
  UIText2.setType(DATA_OUT);
  UIText2.setSubType(0);
  UIText2.setCoord(2, 51, 96  , 22);
  UIText2.setTitleTextColor("FF000000");
  UIText2.setTitleColor("50FFFFFF");
  UIText2.setTitle("Slider Value");
  UIText2.setData(35);
  UIText2.setUnit(" ");


  UI2.setId(3);
  UI2.setType(SLIDER_IN);
  UI2.setInputMode(ON_VALUE_CHANGE);
  UI2.setSliderNumIntervals(50);
  UI2.setSubType(0);
  UI2.setCoord(2, 76, 96  , 22);
  UI2.setColor("FFFF9900");
  UI2.setTitle("Discrete Slider");
  UI2.setMinMax(0, 100);
  UI2.setSliderInitialValue(35);
}

// Arduino will run instructions here repeatedly until you power it off.
int makeUI = false;
void loop()
{
  if (Andee101.isConnected() == true) { 
    if (makeUI == true) {
      UI1.update();
      UI2.update();
      UIText1.update();
      UIText2.update();
      makeUI = false;
    }
    int A = 0;
    int B = 0;
    UI1.getSliderValue(&A);
    UI2.getSliderValue(&B);

    if (lastA != A) {
      lastA = A;
      UIText1.setData(A);
      UIText1.update();
    }

    if (lastB != B) {
      lastB = B;
      UIText2.setData(B);
      UIText2.update();
    }

    Serial.print(A);
    Serial.print(" + ");
    Serial.print(B);
    Serial.print(" = ");
    Serial.println((A + B));

    delay(500);
  }
  else {
    makeUI = true;
  }
}



