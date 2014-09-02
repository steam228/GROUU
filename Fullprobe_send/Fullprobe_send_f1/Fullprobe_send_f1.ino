
#include "DHT.h"
#include "OneWire.h"
#include "math.h"

//Sensors location


#define DHTPIN 3     
#define DHTTYPE DHT22   // DHT 22  (AM2302)

const int numReadings = 20;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


DHT dht(DHTPIN, DHTTYPE);
int ledPin = 13;
int moistPin = A0;
int wetPin = A1;
int lumPin = A2;
int phPin = A3;
int soiltempPin = 2;
OneWire ds(soiltempPin);

//Variables

  float h = 0;
  float t = 0;
  float l = 0;
  int m = 0;
  int w = 0;
  float pH = 0;
  float sT = 0;
  
//Analog calibration Values for Maping (input here)

  
  int mMax=693;
  int mMin=0;
  
  int wMax=1013;
  int wMin=100;
  
  float lMax=1023/2;
  int lMin=0;
  
  int pHMax=1023;
  int pHMin=0;
  


void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht.begin();
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
  
  pinMode (ledPin, OUTPUT);
}

void loop() {
  //Reading Sensores
  h = dht.readHumidity();
  t = dht.readTemperature();
  l = analogRead(lumPin);
  m = analogRead(moistPin);
  w = analogRead(wetPin);
    // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(phPin); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;         
  // send it to the computer as ASCII digits
  

  pH = (0.02*average)-2.92;
  sT = getTemp();
  
  
  //Converting Units
  

  l = exp(0.02639*l-0.7512); 
  
  m = map(m,mMin,mMax,0,100);
  w = map(w,wMin,wMax,100,0);

 
  

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
//  if (isnan(t) || isnan(h) {
//    Serial.println("Failed to read from DHT");
//  } else {
    Serial.print("A"); 
    Serial.print(h);
    Serial.print("G");
    Serial.print(t);
    Serial.print("P");
    Serial.print(l);
    Serial.print("Q");
    Serial.print(m);
    Serial.print("R"); 
    Serial.print(pH,1);
    Serial.print("S");
    Serial.print(sT);
    Serial.print("T");
    Serial.print(w);
    Serial.print("U");
    
    if (t < 20) {
      digitalWrite (ledPin, HIGH);   
    }
    else {
      digitalWrite (ledPin, LOW);   
    }
    
    delay(50);
   
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

void getPH() {
  



 
  
}
