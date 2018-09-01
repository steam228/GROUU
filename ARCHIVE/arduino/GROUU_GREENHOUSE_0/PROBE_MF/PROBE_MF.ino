
#include "DHT.h"
#include "OneWire.h"
#include "math.h"
//#include <Wire.h>
#include "floatToString.h"

//Sensors location


#define DHTPIN 4     
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
int ledPin = 13;
int moistPin = A0;
int wetPin = A1;
int lumPin = A2;
//int phPin = A3;
int soiltempPin = 5;
OneWire ds(soiltempPin);

//Variables

  float h = 0;
  float t = 0;
  float l = 0;
  int m = 0;
  int w = 0;
  int pH = 0;
  float sT = 0;
  
//Analog calibration Values for Maping (input here)

  
  int mMax=600;
  int mMin=0;
  
  int wMax=1013;
  int wMin=100;
  
  float lMax=1023/2;
  int lMin=0;
  
//  int pHMax=1023;
//  int pHMin=0;
  


void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht.begin();
  
//  Wire.begin();                
  
  pinMode (ledPin, OUTPUT);
}

void loop() {
  //Reading Sensores
  h = dht.readHumidity();
  t = dht.readTemperature();
  l = analogRead(lumPin);
  m = analogRead(moistPin);
  w = analogRead(wetPin);
//  pH = analogRead(phPin);
  sT = getTemp();
  
  
  //Converting Units
  
  //l = map(l,lMin,lMax,3,70000);
  l = exp(0.02463*l-0.5522); 
  
  m = map(m,mMin,mMax,0,100);
  w = map(w,wMin,wMax,100,0);
//  int ph = map(pH, pHMin, pHMax,0,7);
 
  

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
//  if (isnan(t) || isnan(h) {
//    Serial.println("Failed to read from DHT");
//  } else {
    Serial.print("Relative Humidity: ");
    Serial.println(h);
    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Luminosity: ");
    Serial.println(l);
    Serial.print("Moisture: ");
    Serial.println(m);
    Serial.print("Soil Temperature: ");
//    Serial.print(pH);
//    Serial.print("%s/");
    Serial.println(sT);
    Serial.print("Leaf Wetness: ");
    Serial.println(w);
    Serial.println("///////////////////////////////");
    Serial.println("///////////////////////////////");
//    Wire.beginTransmission(4); // transmit to device #4
//    Wire.write("floatToString(buffer,h,5);");
//    Wire.write("floatToString(buffer,t,5);");
//    Wire.write("floatToString(buffer,l,5);");
//    Wire.write("floatToString(buffer,m,5);");
//    Wire.write("floatToString(buffer,sT,5);");
//    Wire.write("floatToString(buffer,w,5);");
//    Wire.endTransmission();    // stop transmitting

    
    if (t < 20) {
      digitalWrite (ledPin, HIGH);   
    }
    else {
      digitalWrite (ledPin, LOW);   
    }
    
    delay(4000);
   
//  }
}



float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}


