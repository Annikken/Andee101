// Andee101.cpp - Arduino 101 Library
// Annikken Pte Ltd
// Author: Muhammad Hasif

#include <CurieBle.h>
#include <Andee101.h>
#include <TimeLib.h>
#include <stdlib.h>

char Andee101Version[5] = {'1','.','3','0','0'};

int nameFlag = 0;
int buttonNumber = 24;
char buttonBuffer[50];

char sensorsBuffer[64];

char readBuffer[128];
char readPartBuffer[64];
char phoneBuffer[64];
char sliderBuffer[20][20];


char JoystickBufferX [4];
char JoystickBufferY [4];

bool AndeeConnected = false;
bool versionAndClear = false;
bool resetBLEFlag = true;
bool AndeeAlive = false;

bool dataLog = true;

BLEPeripheral Andee101Peripheral;
BLEService Andee101Service("0BD51666-E7CB-469B-8E4D-2742F1ACC355");
BLECharacteristic Andee101Write ("E7ADD780-B042-4876-AAE1-1A51F5353CC1", BLEWrite | BLENotify, 20);
BLECharacteristic Andee101Read ("E7ADD780-B042-4876-AAE1-1A51F5353CC2", BLEWrite | BLENotify, 20); 
															   
Andee101Class Andee101;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//                                          General Functions                                         //
//                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////

void printDEC(char* buffer)
{
	if(dataLog == true)
	{
		Serial.print("DEC:");
		for(unsigned int v = 0; v < (strlen(buffer)) ; v++)
		{
			Serial.print(buffer[v] & 0xFF,DEC);Serial.print(" ");
		}
		Serial.println("");
	}
	
}

void printHEX(char* buffer)
{
	if(dataLog == true)
	{
		Serial.print("HEX:");
		for(unsigned int v = 0; v < (strlen(buffer)) ; v++)
		{
			Serial.print(buffer[v] & 0xFF,HEX);Serial.print(" ");
		}
		Serial.println("");	
	}
	
}

void sendToPhone( char*UI )
{
  char partialUI[18] = {};
  int msgLen = 0;
  msgLen = strlen(UI);
  if(AndeeConnected == true)
  {
	  if (msgLen >= 19)
	  {
		for (int i = 0; i <= msgLen;)
		{
		  memset(partialUI,0x00,18);
		  memcpy(partialUI, UI + i, 18);
		  
		  Andee101Write.setValue((const unsigned char*)partialUI,18);
		  
		  i = i + 18;		
			
		}
	  }
	  else
	  {		
		Andee101Write.setValue((const unsigned char*)UI,msgLen);
			 
	  }  
  }
	  
}

void systemTime(void)
{
	char msgToPhone[18] = {ASTART,TIMEEPOCH,AEND,0x00,0x00,0x00,
						   0x00,0x00,0x00,0x00,0x00,0x00,
						   0x00,0x00,0x00,0x00,0x00,0x00};
	sendToPhone(msgToPhone);
	//delay(5);
	memset(msgToPhone,0x00,18);
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
    
  AndeeConnected = true;
  versionAndClear = false;
    
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
    
  AndeeConnected = false;
  resetBLEFlag = false;
    
}

void readBLEBuffer(BLECentral& central, BLECharacteristic& characteristic)
{
  unsigned char pressBuffer;
  int mLen;
  char buffer[20];
  
    
  if(Andee101Read.value())
  {
	memset(buffer,0x00,19);
      
	memcpy(buffer,(char*)Andee101Read.value(),(strlen((const char*)Andee101Read.value())));
    mLen = strlen(buffer);
	buffer[18] = '\0';
     
	
	if(readPartBuffer[0] != 0x00)
	{
		memcpy(readPartBuffer+(strlen(readPartBuffer)),buffer,mLen);
	}
	else
    {
		memset(readPartBuffer,0x00,64);				
		memcpy(readPartBuffer,buffer,mLen);			
	}
	
	for(unsigned int b = 0; b < strlen(readPartBuffer);b++)
	{
        
		if(readPartBuffer[b] == 0x0D)
		{
            
			readPartBuffer[b+1] = '\0';
			memset(readBuffer,0x00,128);
			memcpy(readBuffer,readPartBuffer,strlen(readPartBuffer));
			memset(readPartBuffer,0x00,64);
		}
	}
	
	printHEX(readBuffer);
	
	 
	pressBuffer = 0;
	
	if (readBuffer[0] == BUTTON_IN || readBuffer[0] == KEYBOARD_IN|| readBuffer[0] == TIME_IN || readBuffer[0] == DATE_IN)		
	{
		buttonBuffer[0] = 'B';
		buttonBuffer[1] = 'U';
		 for(int i = 0;i< (buttonNumber+1);i++)
		{				
		
				
			if(buttonBuffer[((i*2)+2)] == readBuffer[2])
			{
				pressBuffer = buttonBuffer[((i*2)+3)];				
				pressBuffer++;
				
				sprintf(buttonBuffer + ((i*2)+3),"%c",pressBuffer);
				break;
			}
			else if(buttonBuffer[((i*2)+2)] == 0x00)
			{
				sprintf(buttonBuffer + ((i*2)+2), "%c%c",readBuffer[2],'1');				
				break;
			}
		}
		
		if(readBuffer[0] == KEYBOARD_IN || readBuffer[0] == TIME_IN || readBuffer[0] == DATE_IN)
		{
			memset(phoneBuffer,0x00,64);
			int buffLen = 0;
			
			buffLen = strlen(readBuffer);
			readBuffer[buffLen-2] = '\0';			
			memcpy(phoneBuffer,readBuffer + 4,(buffLen - 1));			
			
		}
		memset(readBuffer,0x00,128);
		return;
	}
	else if(readBuffer[0] == SLIDER_IN)
	{
		char value[18];
		char iid;
		unsigned int i = 0;
		
		iid = readBuffer[2];
		
		for(unsigned int i = 0; i < strlen(readBuffer);i++)
		{
			if(readBuffer[i+4] != P_SEP)
			{
				value[i] = readBuffer[i+4];
			}
			else 
			{
				value[i]='\0';
				break;
			}				
		}
		
		for(i = 0;i<20;i++)
		{
			if(sliderBuffer[i][0] == 0x00)
			{
				sliderBuffer[i][0] = iid;
				strcpy(sliderBuffer[i]+1,value);
				break;
			}
			else if(sliderBuffer[i][0] == iid)
			{
				memset(sliderBuffer[i],0x00,20);
				sliderBuffer[i][0] = iid;
				strcpy(sliderBuffer[i]+1,value);
				break;
			}
		}
    
		memset(readBuffer,0x00,128);
	}
	else if(readBuffer[0] == TIMEEPOCH)
	{
		char buffer[11];
		char gmtBuffer[6];		
		unsigned long sysTime;
		long gmt,dst;
		char *ptr,*dls;
		int pLen,dLen;
		
		ptr = strchr(readBuffer+2,P_SEP);
		ptr++;
				
		dls = strchr(ptr,P_SEP);
		dls++;
		if(ptr != NULL)
		{
			pLen = strlen(ptr);
			dLen = strlen(dls);
			strncpy(gmtBuffer,ptr,pLen-dLen);
			gmtBuffer[pLen - 1] = 0x00;
			gmt = atol(gmtBuffer);
		}
		else
		{
			gmt = 0.00;
		}		
		if(dls !=NULL)
		{
			dst = atol(dls);
		}
		else
		{
			dst = 0.00;
		}
		memcpy(buffer,readBuffer+2,10);
		sysTime = atol(buffer) + (gmt*3600) + (dst*3600) ;
		sprintf(phoneBuffer,"%ld",sysTime);
		setTime(sysTime);
		memset(readBuffer,0x00,128);
	}
	else if(readBuffer[0] == JOYSTICK)
	{
		memset(phoneBuffer,0x00,64);
		memcpy(phoneBuffer,readBuffer,10);
		phoneBuffer[9] = '\0';
		memset(readBuffer,0x00,128);		
	}
	else if(readBuffer[0] == GYRO)
	{
		memset(sensorsBuffer,0x00,64);		
		memcpy(sensorsBuffer,readBuffer+2,strlen(readBuffer));
		memset(readBuffer,0x00,128);
	}
	else if(readBuffer[0] == LAC)
	{
		memset(sensorsBuffer,0x00,64);		
		memcpy(sensorsBuffer,readBuffer+2,strlen(readBuffer));
		memset(readBuffer,0x00,128);
	}
	else if(readBuffer[0] == GRAV)
	{
		memset(sensorsBuffer,0x00,64);		
		memcpy(sensorsBuffer,readBuffer+2,strlen(readBuffer));
		memset(readBuffer,0x00,128);
	}
	else if(readBuffer[0] == GPS)
	{
		memset(sensorsBuffer,0x00,64);		
		memcpy(sensorsBuffer,readBuffer+2,strlen(readBuffer));
		memset(readBuffer,0x00,128);
	}
	
	else if(readBuffer[0] == WATCH)
	{
		if (readBuffer[4] == 'T')
		{
			buttonBuffer[0] = 'B';
			buttonBuffer[1] = 'U';
			for(int i = 0; i < (buttonNumber+1); i++)
			{
					
				if(buttonBuffer[((i*2)+2)] == (readBuffer[2] + 32))
				{
					pressBuffer = buttonBuffer[((i*2)+3)];				
					pressBuffer++;
					sprintf(buttonBuffer + ((i*2)+3),"%c",pressBuffer);
					break;
				}		
				else if(buttonBuffer[((i*2)+2)] == 0x00)
				{
					sprintf(buttonBuffer + ((i*2)+2), "%c%c",(readBuffer[2] + 32),'1');
                    
					break;
				}				
			}
			if (readBuffer[2] == 76)
			{
				memset(phoneBuffer,0x00,64);
				int buffLen = 0;
				
				buffLen = strlen(readBuffer);
				readBuffer[buffLen-3] = '\0';			
				memcpy(phoneBuffer,readBuffer + 4,(buffLen - 1));			
				
			}
		}
		memset(readBuffer,0x00,128);
	}
	 else if(readBuffer[0] == VERSION)
	{
		AndeeAlive= true;		
	}
	
  } 
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//                                        Annikken Andee101Class                                         //
//                                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////

void Andee101Class::versionClear()
{
	if(versionAndClear == false)
	{
		delay(1800);
		Andee101.sendVersion();	
		Andee101.clear();
		versionAndClear = true;
	}
}

bool Andee101Class::isConnected()
{
	if(AndeeAlive == true)
	{
		Andee101.sendVersion();
		AndeeAlive = false;
	}
    return AndeeConnected;
}

void Andee101Class::resetBLE()
{		
	if(resetBLEFlag == false)
	{
		
		Andee101.broadcast();
		memset(buttonBuffer,0x00,50);
		memset(phoneBuffer,0x00,64);
		memset(readBuffer,0x00,128);
		memset(readPartBuffer,0x00,64);
		memset(sliderBuffer,0x00,64);
		resetBLEFlag = true;
	}
}

void Andee101Class::begin()
{
	if (nameFlag == 0)
	{
		Andee101Peripheral.setLocalName("Andee101");
		Andee101Peripheral.setDeviceName("Andee101");
	}
	Andee101Peripheral.setAppearance(384);
	Andee101Peripheral.setAdvertisedServiceUuid(Andee101Service.uuid());
	Andee101Peripheral.addAttribute(Andee101Service);
	Andee101Peripheral.addAttribute(Andee101Read);
	Andee101Peripheral.addAttribute(Andee101Write);
	Andee101Read.setEventHandler(BLEWritten, readBLEBuffer);
	Andee101Peripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
	Andee101Peripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
	Andee101Peripheral.begin();	
	delay(5);
	Andee101.broadcast();
	
	memset(buttonBuffer,0x00,50);
	memset(phoneBuffer,0x00,64);
	memset(readBuffer,0x00,128);
	memset(readPartBuffer,0x00,64);
	memset(sliderBuffer,0x00,64);
}

void Andee101Class::broadcast()
{
	BLECentral central = Andee101Peripheral.central();
}

void Andee101Class::poll()
{
	Andee101Peripheral.poll();
}

void Andee101Class::setName(const char* name)
{
	Andee101Peripheral.setLocalName(name);
	Andee101Peripheral.setDeviceName(name);
	nameFlag = 1;
	delay(5);
}

void Andee101Class::clear()
{
	char msgToPhone[18] = {ASTART, CLEAR, AEND, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 	
	sendToPhone(msgToPhone);
	delay(5);
	memset(msgToPhone,0x00,18);
}

void Andee101Class::getDeviceTime(int* h,int* m, int* s)
{
	systemTime();
	delay(50);
	
	*h = hour();
	*m = minute();
	*s = second();
}

void Andee101Class::getDeviceDate(int* d, int* m, int* y )
{
	systemTime();
	delay(50);
	
	*d = day();
	*m = month();
	*y = year();
}

long Andee101Class::getDeviceTimeStamp(void)
{
	long buffer;
	systemTime();
	delay(50);
	buffer = atol(phoneBuffer);
	return buffer;
}

void Andee101Class::sendVersion(void)
{
	char buffer[10];
	memset(buffer,0x00,10);
	buffer[0] = ASTART;
	buffer[1] = VERSION;
	buffer[2] = P_SEP;
	buffer[3] = Andee101Version[0];
	buffer[4] = Andee101Version[1];
	buffer[5] = Andee101Version[2];
	buffer[6] = Andee101Version[3];
	buffer[7] = Andee101Version[4];
	buffer[8] = AEND;
	buffer[9] = '\0';
	
	sendToPhone(buffer);
	delay(5);	
}

void Andee101Class::disconnect(void){
    char buffer[4];
    memset(buffer,0x00,4);
    buffer[0] = ASTART;
    buffer[1] = DC;
    buffer[2] = AEND;
    buffer[3] = '\0';
    
    sendToPhone(buffer);
    delay(5);
}
/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
//                                      Text To Speech                                     //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

void Andee101Class::textToSpeech(char accent, float speed, float pitch, char* speech)
{
	char buffer[128];
	sprintf(buffer, "%c%c%c%s%c%.01f%c%.01f%c%c%c", ASTART, TTS, P_SEP, speech, P_SEP, speed, P_SEP, pitch, P_SEP, accent, AEND);
	sendToPhone(buffer);	
}

void Andee101Class::stopTTS(void)
{
	char buffer[20];
	sprintf(buffer, "%c%c%c|%c0%c0%c0%c", ASTART, TTS, P_SEP,  P_SEP, P_SEP,   P_SEP, AEND);
    

    sendToPhone(buffer);
    delay(2);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                               Control for Smartphone Camera                              //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void Andee101Class::takePhoto(char cameraType, char autoFocus, char flash)
{
	char buffer[11];
	
	sprintf(buffer, "%c%c%c%c%c%c%c%c%c", ASTART, CAMERA, P_SEP, cameraType, P_SEP,	autoFocus, P_SEP, flash, AEND);
	sendToPhone(buffer);
	delay(2);
}
///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//            Gyroscope, Linear Accelerometer, Gravity and GPS data from Smartphone          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////

void Andee101Class::gyroInit(int interval, int iteration)
{
	char buffer[64];	
	sprintf(buffer,"%c%c%c%c%d%c%d%c",ASTART,GYRO,'0',P_SEP,interval,P_SEP,iteration,AEND);
	sendToPhone(buffer);
	delay(50);
}

void Andee101Class::gyroStop()
{
	char buffer[6];	
	sprintf(buffer,"%c%c%c%c",ASTART,GYRO,'1',AEND);
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::getGyroReading(float* x,float* y,float* z)
{
	char* pEnd;	
	
	
	*x = strtod (sensorsBuffer, &pEnd);
	*y = strtod (pEnd, &pEnd);
	*z = strtod (pEnd, NULL);
	
	return;	
}

void Andee101Class::lacInit(int interval, int iteration)
{
	char buffer[64];	
	sprintf(buffer,"%c%c%c%c%d%c%d%c",ASTART,LAC,'0',P_SEP,interval,P_SEP,iteration,AEND);
	sendToPhone(buffer);
	delay(50);
}

void Andee101Class::lacStop()
{
	char buffer[6];	
	sprintf(buffer,"%c%c%c%c",ASTART,LAC,'1',AEND);
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::getLacReading(float* x,float* y,float* z)
{
	char* pEnd;	
	
	
	*x = strtod (sensorsBuffer, &pEnd);
	*y = strtod (pEnd, &pEnd);
	*z = strtod (pEnd, NULL);
	
	return;	
}

void Andee101Class::gravInit(int interval, int iteration)
{
	char buffer[64];	
	sprintf(buffer,"%c%c%c%c%d%c%d%c",ASTART,GRAV,'0',P_SEP,interval,P_SEP,iteration,AEND);
	sendToPhone(buffer);
	delay(50);
}

void Andee101Class::gravStop()
{
	char buffer[6];	
	sprintf(buffer,"%c%c%c%c",ASTART,GRAV,'1',AEND);
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::getGravReading(float* x,float* y,float* z)
{
	char* pEnd;	
	
	
	*x = strtod (sensorsBuffer, &pEnd);
	*y = strtod (pEnd, &pEnd);
	*z = strtod (pEnd, NULL);
	
	return;	
}

void Andee101Class::gpsInit(int interval, int iteration)
{
	char buffer[64];	
	sprintf(buffer,"%c%c%c%c%d%c%d%c",ASTART,GPS,'0',P_SEP,interval,P_SEP,iteration,AEND);
	sendToPhone(buffer);
	delay(50);
}

void Andee101Class::gpsStop()
{
	char buffer[6];	
	sprintf(buffer,"%c%c%c%c",ASTART,GPS,'1',AEND);	
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::getGpsReading(float* x,float* y,float* z)
{
	char* pEnd;	
	
	
	*x = strtod (sensorsBuffer, &pEnd);
	*y = strtod (pEnd, &pEnd);
	*z = strtod (pEnd, NULL);
	
	return;	
}

void Andee101Class::changeScreen(int screen)
{	
	char buffer[8];
	sprintf(buffer,"%c%c%c%c%c%i%c",ASTART,WATCH,P_SEP,'C',P_SEP,screen,AEND);	
	buffer[7] = '\0';
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::showScreen()
{
	char buffer[6];
	sprintf(buffer,"%c%c%c%c%c",ASTART,WATCH,P_SEP,'S',AEND);
	buffer[5] = '\0';
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::hideScreen()
{
	char buffer[6] = {ASTART,WATCH,P_SEP,'H',AEND};
	buffer[5] = '\0';
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::textInput()
{
	char buffer[6] = {ASTART,WATCH,P_SEP,'X',AEND};
	buffer[5] = '\0';
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::sendSMS(char* number,char* message)
{
	char buffer[64];
	sprintf(buffer,"%c%c%c%s%c%s%c",ASTART,SMS,P_SEP,number,P_SEP,message,AEND);
	sendToPhone(buffer);
	delay(2);
}

void Andee101Class::vibrate()
{
	char buffer[5];
	sprintf(buffer,"%c%c%c",ASTART,VIBRATE,AEND);
	sendToPhone(buffer);
	delay(2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//                              Annikken Andee101Helper Functions                              //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////


void Andee101Helper::setId(int value)
{
	id = value + 32;
	
	sprintf(dataBuffer,"%s"," ");
	sprintf(titleBuffer,"%s"," ");
	sprintf(unitBuffer,"%s"," ");
	sprintf(titleBGBuffer,"%s","FF00FF00");
	sprintf(titleFontBuffer,"%s","FFFF0000");
	sprintf(bodyFontBuffer,"%s","FF0000FF");
	sprintf(bodyBGBuffer,"%s","FF83A4C8");
	sprintf(maxBuffer,"%s"," ");
	sprintf(minBuffer,"%s"," ");
	inputBuffer = '0';
	subBuffer = '0';
	flashBuffer = '0';		
	
	if (value == WATCH_TITLE)
	{
		watchBuffer = '0';
	}	
	else if(value == WATCH_BUTTON1)
	{
		watchBuffer = '1';
	}
	else if(value == WATCH_BUTTON2)
	{
		watchBuffer = '2';
	}
	else if(value == WATCH_BUTTON3)
	{
		watchBuffer = '3';
	}
	else if(value == WATCH_BUTTON4)
	{
		watchBuffer = '4';
	}
}

void Andee101Helper::setType(char type)
{
	if(type == DATA_OUT)
	{
		sprintf(bleBuffer , "%c%c", (ASTART), (DATA_OUT));
		subBuffer = '0';
	}
	else if(type == DATA_OUT_CIRCLE)
	{		
		sprintf(bleBuffer, "%c%c",ASTART,DATA_OUT);
		subBuffer = '1';
	}
	else if(type == DATA_OUT_HEADER)
	{		
		sprintf(bleBuffer, "%c%c",ASTART,DATA_OUT);
		subBuffer = '2';
	}
	
	else if(type == BUTTON_IN)
	{		
		sprintf(bleBuffer, "%c%c",ASTART,BUTTON_IN);
		subBuffer = '0';
	}
	else if(type == CIRCLE_BUTTON)
	{		
		sprintf(bleBuffer, "%c%c",ASTART,BUTTON_IN);
		subBuffer = '1';
	}
	
	else if(type == ANALOG_DIAL_OUT)
	{
		sprintf(bleBuffer,"%c%c", ASTART, ANALOG_DIAL_OUT);
	}
	
	else if(type == KEYBOARD_IN)
	{		
		sprintf(bleBuffer, "%c%c", ASTART, KEYBOARD_IN);
	}
	
	else if(type == DATE_IN)
	{
		sprintf(bleBuffer, "%c%c", ASTART, DATE_IN);
	}
	
	else if(type == TIME_IN)
	{
		sprintf(bleBuffer,"%c%c", ASTART, TIME_IN);
	}
	
	else if(type == SLIDER_IN)
	{
		sprintf(bleBuffer, "%c%c", ASTART,SLIDER_IN);
	}	
	
	else if (type == TTS)
	{
		sprintf(bleBuffer, "%c%c", ASTART, TTS);
	}		
	
	else if (type == JOYSTICK)
	{
		sprintf(bleBuffer,"%c%c", ASTART,JOYSTICK);
	}
	
	else if (type == WATCH)
	{
		sprintf(bleBuffer,"%c%c", ASTART,WATCH);
	}

	else
	{
		
	}
}
void Andee101Helper::setCoord(float x, float y, float w, float h)
{
	if(x > C_HLIMIT)
        x = C_HLIMIT;
    else if(x < C_LLIMIT)
        x = C_LLIMIT;
    
    if(y > C_HLIMIT)
        y = C_HLIMIT;
    else if(y < C_LLIMIT)
        y = C_LLIMIT;
    
    
    if(w > C_HLIMIT)
        w = C_HLIMIT;
    else if(w < C_LLIMIT)
        w = C_LLIMIT;
    
    
    if(h > C_HLIMIT)
        h = C_HLIMIT;
    else if(h < C_LLIMIT)
        h = C_LLIMIT;
	
	x = ((x/200)*400) + 32;
	y = ((y/200)*400) + 32;	
	w = ((w/200)*400) + 32;	
	h = ((h/200)*400) + 32;
	

		
	memset(xywhBuffer,0x00,13);
	sprintf(xywhBuffer, "%03i%03i%03i%03i", (int)x, (int)y, (int)w, (int)h);
}

////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                               Input Mode and Sub Type                              //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

void Andee101Helper::setInputMode(char type)
{
	if(type < 32)
		inputBuffer = type + 48;
	
	else
		inputBuffer = type;
	
}

void Andee101Helper::setSubType(char type)
{
	if(type < 32)
		subBuffer = type + 48;
	
	else
		subBuffer = type;
	
	
}

/////////////////////////////////////////////////////////////////////
//                                                                 //
//                           Set Colours                           //
//                                                                 //
/////////////////////////////////////////////////////////////////////

void Andee101Helper::setTitleColor(const char* color)
{
	
	memset(titleBGBuffer,0x00,9);
	if(strlen(color) == 6)
	{
		sprintf(titleBGBuffer,"FF%s",color);
	}
	else
	{
		sprintf(titleBGBuffer, "%s", color);
	}
}
void Andee101Helper::setTitleColor(const char color)
{	
	memset(titleBGBuffer,0x00,9);	
	sprintf(titleBGBuffer, "%s", &color);
}


void Andee101Helper::setTitleTextColor(const char* color)
{		
	memset(titleFontBuffer,0x00,9);
	if(strlen(color) == 6)
	{
		sprintf(titleFontBuffer, "FF%s", color);
	}
	else
	{
		sprintf(titleFontBuffer, "%s", color);
	}
}
void Andee101Helper::setTitleTextColor(const char color)
{
	memset(titleFontBuffer,0x00,9);
	sprintf(titleFontBuffer, "%s", &color);
}


 void Andee101Helper::setColor(const char* color)
{
	memset(bodyBGBuffer,0x00,9);
	if(strlen(color) == 6)
	{
		sprintf(bodyBGBuffer, "FF%s", color);
	}
	else
	{
		sprintf(bodyBGBuffer, "%s", color);
	}
} 
void Andee101Helper::setColor(const char color)
{
	memset(bodyBGBuffer,0x00,9);	
	sprintf(bodyBGBuffer, "%s", &color);
}


void Andee101Helper::setTextColor(const char* color)
{
	memset(bodyFontBuffer,0x00,9);
	if(strlen(color) == 6)
	{
		sprintf(bodyFontBuffer, "FF%s", color);
	}
	else
	{
		sprintf(bodyFontBuffer, "%s", color);
	}
} 
void Andee101Helper::setTextColor(const char color)
{
	memset(bodyFontBuffer,0x00,9);	
	sprintf(bodyFontBuffer, "%s", &color);
} 

//////////////////////////////////////////////////////////////////////
//                                                                  //
//                  SetData, Unit, Title & MinMax                   //
//                                                                  //
//////////////////////////////////////////////////////////////////////

void Andee101Helper::setData(const char* data)
{
	memset(dataBuffer,0x00,64);
	if(data == 0x00)
		{
			data = " ";
		}	
    sprintf(dataBuffer, "%s", data);
}

void Andee101Helper::setData(int data)
{		
	memset(dataBuffer,0x00,64);			
	sprintf(dataBuffer, "%d", data);
}

void Andee101Helper::setData(float data,char decPlace)
{		
	memset(dataBuffer,0x00,64);		
	dtostrf(data,3,decPlace,dataBuffer);
}

void Andee101Helper::setData(double data,char decPlace)
{		
	memset(dataBuffer,0x00,64);	
	dtostrf(data,3,decPlace,dataBuffer);
}



void Andee101Helper::setTitle(const char* title)
{
	memset(titleBuffer,0x00,64);
	if(title == 0x00)
	{
		title = " ";
	}
	sprintf(titleBuffer, "%s", title);
}

void Andee101Helper::setTitle(int title)
{
	memset(titleBuffer,0x00,64);	
	sprintf(titleBuffer, "%d", title);
}

void Andee101Helper::setTitle(float title, char decPlace)
{
	memset(titleBuffer,0x00,64);	
	dtostrf(title,3,decPlace,titleBuffer);
}

void Andee101Helper::setTitle(double title, char decPlace)
{
	memset(titleBuffer,0x00,64);	
	dtostrf(title,3,decPlace,titleBuffer);
}



void Andee101Helper::setUnit(const char* unit)
{
	memset(unitBuffer,0x00,64);
    
	if(unit == 0x00)
	{
		unit = " ";
	}
	sprintf(unitBuffer, "%s", unit);
}

void Andee101Helper::setUnit(int unit)
{
	memset(unitBuffer,0x00,64);	
	sprintf(unitBuffer, "%d", unit);
}

void Andee101Helper::setUnit(float unit, char decPlace)
{
	memset(unitBuffer,0x00,64);	
	dtostrf(unit,3,decPlace,unitBuffer);
}

void Andee101Helper::setUnit(double unit, char decPlace)
{
	memset(unitBuffer,0x00,64);	
	dtostrf(unit,3,decPlace,unitBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//                         Miscellaneous Data Functions                             //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

void Andee101Helper::setMinMax(int min, int max)
{
	memset(minBuffer,0x00,5);	
	memset(maxBuffer,0x00,5);	
	sprintf(minBuffer,"%d", min);
	sprintf(maxBuffer,"%d", max);
}

void Andee101Helper::setMinMax(float min,float max,char decPlace)
{
	memset(minBuffer,0x00,5);	
	memset(maxBuffer,0x00,5);
	dtostrf(min,3,decPlace,minBuffer);
	dtostrf(max,3,decPlace,maxBuffer);
}

void Andee101Helper::setMinMax(double min,double max,char decPlace)
{
	memset(minBuffer,0x00,5);	
	memset(maxBuffer,0x00,5);
	dtostrf(min,3,decPlace,minBuffer);
	dtostrf(max,3,decPlace,maxBuffer);
}

/* void Andee101Helper::setKeyboardType(char type)
{
	inputBuffer = type;
} */

void Andee101Helper::getKeyboardMessage(char* message)
{	
	phoneBuffer[(strlen(phoneBuffer)-1)] = '\0';
	sprintf(message,"%s", phoneBuffer);
	return;	
}

void Andee101Helper::setDefaultDate(int date, int month, int year)
{
	sprintf(dataBuffer,"%02i%02i%04i", date%31, month%12, year);
}

void Andee101Helper::getDateInput(int* d,int* m, int* y)
{
	*y = atoi(phoneBuffer+4);
	phoneBuffer[4] = 0x00;
	*m = atoi(phoneBuffer+2);
	phoneBuffer[2] = 0x00;
	*d = atoi(phoneBuffer);
	return;
}

void Andee101Helper::setDefaultTime(int hour,int min, int sec)
{
	sprintf(dataBuffer,"%02i%02i%02i", hour%24, min%60, sec%60);
}

void Andee101Helper::getTimeInput(int* h,int* m,int* s)
{
	*s = atoi(phoneBuffer+4);
	phoneBuffer[4] = 0x00;
	*m = atoi(phoneBuffer+2);
	phoneBuffer[2] = 0x00;
	*h = atoi(phoneBuffer);
	
	return;
}


/////////////////////////////////////////////////////////////////////
//                                                                 //
//                            BUTTONS                              //
//                                                                 //
/////////////////////////////////////////////////////////////////////

int Andee101Helper::isPressed(void)
{
	
	for(int i = 0; i<buttonNumber;i++)
	{
		int pos = (i*2)+2;
		if(id == buttonBuffer[pos])
		{
			if(buttonBuffer[pos+1] > '0')
			{
				flashBuffer = buttonBuffer[pos+1];
				buttonBuffer[pos+1] = 0x30;				
				
				if(inputBuffer == 48)
				{
					return '1';
				}
				else
				{
					return flashBuffer - '0';
				}				
			}
		}
	}
	flashBuffer = 0x00;
	return 0;
}
 void Andee101Helper::ack(void)
{
	 
	if(bleBuffer[1] == BUTTON_IN)
	{
		if(inputBuffer == '0')
		{
			char* buffer = new char[18];
			buffer[0] = AEND;
			buffer[1] = ASTART;
			buffer[2] = ACKN;
			buffer[3] = P_SEP;
			buffer[4] = id;
			buffer[5] = AEND;
			for(int l = 6; l < 18; l++)
			{
				buffer[l] = 0x00;
			}
			sendToPhone(buffer);
		}

	}
	else
	{
		char* buffer = new char[18];
			buffer[0] = AEND;
			buffer[1] = ASTART;
			buffer[2] = ACKN;
			buffer[3] = P_SEP;
			buffer[4] = id;
			buffer[5] = AEND;
			for(int l = 6; l < 18; l++)
			{
				buffer[l] = 0x00;
			}
			
		 
			sendToPhone(buffer);
	}
	
}

int Andee101Helper::pressCounter()
{		
	return(int)(flashBuffer-48);
}

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//                                   SLIDERS                                      //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

void Andee101Helper::setSliderInitialValue(int value)
{
	sprintf(dataBuffer,"%d",value);
	sprintf(tempBuffer,"%d",value);
}

void Andee101Helper::setSliderInitialValue(float value,char decPlace)
{
	dtostrf(value,3,decPlace,dataBuffer);
	dtostrf(value,3,decPlace,tempBuffer);
}
void Andee101Helper::setSliderInitialValue(double value,char decPlace)
{
	dtostrf(value,3,decPlace,dataBuffer);
	dtostrf(value,3,decPlace,tempBuffer);
}



void Andee101Helper::setSliderNumIntervals(int numInterval)
{
	sprintf(unitBuffer,"%d",numInterval);
}


void Andee101Helper::moveSliderToValue(int value)
{
	flashBuffer = (char)value;
}



void Andee101Helper::getSliderValue(int* x)
{	
	char buffer[20];
	unsigned int i = 0;
	memset(buffer,0x00,20);
	
	for(i = 0; i < 20; i++)
	{
		if(sliderBuffer[i][0] == id)
		{
			strcpy(buffer,sliderBuffer[i] + 1);			
		}
	}
	
	if(buffer[0] == 0x00)
	{
		*x = atoi(tempBuffer);
	}		
	else
	{
		*x = atoi(buffer);
	}
	
}

void Andee101Helper::getSliderValue(float* f)
{
	char buffer[20];
	unsigned int i = 0;
	memset(buffer,0x00,20);
	for(i = 0; i < 20; i++)
	{
		if(sliderBuffer[i][0] == id)
		{
			strcpy(buffer,sliderBuffer[i] + 1);
		}
	}
	if(buffer[0] == 0x00)
	{
		*f = atof(tempBuffer);
	}		
	else
	{
		*f = atof(buffer);
	}	
}

void Andee101Helper::getSliderValue(double* d)
{
	char buffer[20];
	unsigned int i = 0;
	memset(buffer,0x00,20);
	for(i = 0; i < 20; i++)
	{
		if(sliderBuffer[i][0] == id)
		{
			strcpy(buffer,sliderBuffer[i] + 1);
		}
	}
	if(buffer[0] == 0x00)
	{
		*d = strtod(tempBuffer,NULL);
	}		
	else
	{
		*d = strtod(buffer,NULL);
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                                    Joystick Control                                  //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

void Andee101Helper::getJoystick(int* x,int* y)
{
	char bufferX[4];
	char bufferY[4];
	
	for(int i = 0;i<3;i++)
	{
		bufferX[i] = phoneBuffer[i+3];
	}
	bufferX[3] = '\0';
	
	for(int j = 0;j<3;j++)
	{
		bufferY[j] = phoneBuffer[j+6];
	}
	bufferY[3] = '\0';
	 
	
	if(id == phoneBuffer[1])
	{
		*x = atoi(bufferX); //- (int)100;
		*y = atoi(bufferY); //- (int)100;		
	}	
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void Andee101Helper::update(void)
{
	Andee101.versionClear();
	Andee101.resetBLE();
	Andee101.isConnected();
	
	if(bleBuffer[1] == DATA_OUT)	
	{
		sprintf(bleBuffer , "%c%c%c%c%s%c%c%s%s%s%s%c%s%c%s%c%s%c", (ASTART),(DATA_OUT),subBuffer,(id), xywhBuffer,(inputBuffer),P_SEP,titleBGBuffer,titleFontBuffer,bodyBGBuffer,bodyFontBuffer,P_SEP,	titleBuffer,P_SEP,unitBuffer,P_SEP,dataBuffer,(AEND));
	}
	
	else if(bleBuffer[1] == BUTTON_IN)
	{
		sprintf(bleBuffer, "%c%c%c%c%s%c%c%s%s%c%s%c",ASTART,BUTTON_IN,subBuffer, id,xywhBuffer,inputBuffer,P_SEP,bodyBGBuffer,bodyFontBuffer,P_SEP,titleBuffer,AEND);
	}
	
	else if(bleBuffer[1] == ANALOG_DIAL_OUT)
	{
		if (strcmp (maxBuffer," ") == 0)
		{
			sprintf(maxBuffer,"%s","255");
		}
		if (strcmp (minBuffer," ") == 0)
		{
			sprintf(minBuffer,"%c",'0');
		}
		sprintf(bleBuffer,"%c%c%c%c%s%c%c%s%s%c%s%c%s%c%s%c%s%c%s%c", ASTART, ANALOG_DIAL_OUT,subBuffer, id, xywhBuffer,inputBuffer, P_SEP, titleBGBuffer,bodyBGBuffer, P_SEP, titleBuffer, P_SEP, unitBuffer,P_SEP, dataBuffer, P_SEP, maxBuffer, P_SEP, minBuffer, AEND);
	}	
	
	else if(bleBuffer[1] == KEYBOARD_IN)
	{
		sprintf(bleBuffer, "%c%c%c%c%s%c%c%s%c%s%c%s%c", ASTART, KEYBOARD_IN,subBuffer, id, xywhBuffer, inputBuffer,P_SEP, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, dataBuffer, AEND);
	}
	
	else if(bleBuffer[1] == DATE_IN)
	{
		sprintf(bleBuffer, "%c%c%c%c%s%c%c%s%c%s%c%s%c", ASTART, DATE_IN, subBuffer, id,xywhBuffer, inputBuffer,P_SEP, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, dataBuffer, AEND);
	}
	
	else if(bleBuffer[1] == TIME_IN)
	{
		sprintf(bleBuffer,"%c%c%c%c%s%c%c%s%c%s%c%s%c", ASTART, TIME_IN,subBuffer,  id,xywhBuffer, inputBuffer,P_SEP, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, dataBuffer, AEND);
	}
	
	else if(bleBuffer[1] == SLIDER_IN)
	{
		sprintf(bleBuffer, "%c%c%c%c%s%c%c%s%s%c%s%c%s%c%s%c%s%c%s%c%c%c", ASTART,SLIDER_IN,subBuffer, id,xywhBuffer,inputBuffer,P_SEP,titleBGBuffer,bodyBGBuffer,P_SEP,titleBuffer,P_SEP,dataBuffer,P_SEP,maxBuffer,P_SEP,minBuffer,P_SEP,unitBuffer,P_SEP,flashBuffer,AEND);
	}	
	
	else if(bleBuffer[1] == TTS)
	{
		sprintf(bleBuffer, "%c%c%c%s%c%s%c%s%c%c%c", ASTART, TTS, P_SEP, dataBuffer, P_SEP, titleBuffer, P_SEP, unitBuffer, P_SEP, inputBuffer, AEND);
	}
	
	else if(bleBuffer[1] == JOYSTICK)
	{
		sprintf(bleBuffer,"%c%c%c%c%s%c%c%s%s%s%c%s%c", ASTART,JOYSTICK,subBuffer, id,xywhBuffer,inputBuffer,P_SEP,	titleBGBuffer,titleFontBuffer,bodyBGBuffer,P_SEP,titleBuffer,AEND);
	}
	
	else if(bleBuffer[1] == WATCH)
	{
		sprintf(bleBuffer,"%c%c%c%c%c%s%s%c%s%c", ASTART,WATCH,P_SEP,watchBuffer,P_SEP,titleBGBuffer,titleFontBuffer,P_SEP,titleBuffer,AEND);
	}
	printHEX(bleBuffer);
	sendToPhone(bleBuffer);
	delay(2);
	
	
}

void Andee101Helper::updateData(char* string)
{
	char* tempBuffer = new char[64];
	tempBuffer = string;
	if(bleBuffer[1] == DATA_OUT)
	{
		sprintf(bleBuffer , "%c%c%c%s%c%c%s%s%s%s%c%s%c%s%c%s%c", (ASTART), (DATA_OUT), (id), xywhBuffer,(inputBuffer),P_SEP,titleFontBuffer,titleBGBuffer,bodyFontBuffer,bodyBGBuffer,P_SEP,titleBuffer,P_SEP,unitBuffer,P_SEP,tempBuffer,(AEND));
	}
	
	else if(bleBuffer[1] == BUTTON_IN)
	{
		sprintf(bleBuffer, "%c%c%c%s%c%c%s%c%s%c",ASTART,BUTTON_IN, id,xywhBuffer,inputBuffer,P_SEP,bodyBGBuffer,P_SEP,titleBuffer,AEND);
	}
	
	else if(bleBuffer[1] == ANALOG_DIAL_OUT)
	{
		sprintf(bleBuffer,"%c%c%c%s%c%c%s%s%c%s%c%s%c%s%c%s%c%s%c", ASTART, ANALOG_DIAL_OUT, id, xywhBuffer,inputBuffer, P_SEP, titleBGBuffer, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, unitBuffer,	P_SEP, tempBuffer, P_SEP, maxBuffer, P_SEP, minBuffer, AEND);
	}
	
	else if(bleBuffer[1] == KEYBOARD_IN)
	{		
		sprintf(bleBuffer, "%c%c%c%s%c%c%s%c%s%c%s%c", ASTART, KEYBOARD_IN, id, xywhBuffer, inputBuffer,P_SEP, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, tempBuffer, AEND);
	}
	
	else if(bleBuffer[1] == DATE_IN)
	{
		sprintf(bleBuffer, "%c%c%c%s%c%c%s%c%s%c%s%c", ASTART, DATE_IN, id, xywhBuffer, inputBuffer,P_SEP, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, tempBuffer, AEND);
	}
	
	else if(bleBuffer[1] == TIME_IN)
	{
		sprintf(bleBuffer,"%c%c%c%s%c%c%s%c%s%c%s%c", ASTART, TIME_IN, id, xywhBuffer, inputBuffer,P_SEP, bodyBGBuffer, P_SEP, titleBuffer, P_SEP, tempBuffer, AEND);
	}
	
	else if(bleBuffer[1] == SLIDER_IN)
	{
		sprintf(bleBuffer, "%c%c%c%c%s%c%c%s%s%s%c%s%c%s%c%s%c%s%c%s%c%c%c", ASTART,SLIDER_IN,subBuffer, id,xywhBuffer,inputBuffer,P_SEP,titleFontBuffer,titleBGBuffer,bodyBGBuffer,P_SEP,titleBuffer,P_SEP,tempBuffer,P_SEP,maxBuffer,P_SEP,minBuffer,P_SEP,unitBuffer,
				P_SEP,subBuffer,AEND);
	}	
	
	else if(bleBuffer[1] == TTS)
	{
		sprintf(bleBuffer, "%c%c%c%s%c%s%c%s%c%c%c", ASTART, TTS, P_SEP, tempBuffer, P_SEP, titleBuffer, P_SEP, unitBuffer, P_SEP, inputBuffer, AEND);
	}
	 
	sendToPhone(bleBuffer);
	delay(2);
}

void Andee101Helper::updateData(int data)
{
	char buffer[24];
	sprintf(buffer,"%d",data);
	updateData(buffer);	
}
void Andee101Helper::updateData(float data,char decPlace)
{
	char buffer[24];
	dtostrf(data,3,decPlace,buffer);
	updateData(buffer);	
}
void Andee101Helper::updateData(double data,char decPlace)
{
	char buffer[24];
	dtostrf(data,3,decPlace,buffer);
	updateData(buffer);	
}



void Andee101Helper::remove()
{
	char removeUI[6] = {ASTART, REMOVE, P_SEP, (char)id, AEND, 0x00};
	
	sendToPhone(removeUI);
	delay(2);
}


