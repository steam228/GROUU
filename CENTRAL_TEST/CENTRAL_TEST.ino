

#include <Wire.h>
#include <Bridge.h>
#include "I2C_Anything.h"

//#define TWI_BUFFER_LENGTH 64

const byte MY_ADDRESS = 42;

boolean haveData = false;
float h;
float t;
float l;
int m;
float sT;
int w;

void setup()
{
  Wire.begin (MY_ADDRESS);
  Serial.begin (9600);
  
}  // end of setup

void loop()
{
  Wire.onReceive (receiveEvent);
  
  if (haveData)
  {
    
    Serial.print ("Received Humidity= ");
    Serial.println (h);
    Serial.print ("Room Temperature= ");
    Serial.println (t);
    Serial.print ("Light (lumens/m2)= ");
    Serial.println (l);
    Serial.print ("Moisture (%)= ");
    Serial.println (m);
    Serial.print ("Soil Temperature (ºC)= ");
    Serial.println (sT);
    Serial.print ("Leaf Wetness (ºC)= ");
    Serial.println (w);
    Serial.println ("|||||||||||||||||||||||");
    haveData = false;
  }  // end if haveData
  delay(1000);
}  // end of loop

// called by interrupt service routine when incoming data arrives
void receiveEvent (int howMany)
{
  if (howMany >=  (sizeof h) + (sizeof t) + (sizeof l) + (sizeof m) + (sizeof sT) + (sizeof w))
  {
    I2C_readAnything (h);
    I2C_readAnything (t);
    I2C_readAnything (l);
    I2C_readAnything (m);
    I2C_readAnything (sT);
    I2C_readAnything (w);

    haveData = true;
  }  // end if have enough data

}  // end of receiveEvent
