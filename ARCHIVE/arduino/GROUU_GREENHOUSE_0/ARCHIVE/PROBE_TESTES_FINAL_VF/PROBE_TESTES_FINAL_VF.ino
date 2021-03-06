
#include "DHT.h"
#include "OneWire.h"
#include "math.h"
#include "I2C_Anything.h"

#include <Wire.h>

//Sensors location


#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int moistPin = A0;
int wetPin = A1;
int lumPin = A2;
int soiltempPin = 5;
OneWire ds(soiltempPin);
int ledPin = 13;

//Variables

float h = 0;
float t = 0;
float l = 0;
int m = 0;
int w = 0;
float sT = 0;

//Analog calibration Values for Maping (input here)


int mMax = 650;
int mMin = 0;

int wMax = 1023;
int wMin = 0;

int lMax = 1023;
int lMin = 0;

int pHMax = 1023;
int pHMin = 0;

const byte SLAVE_ADDRESS = 42;

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  Wire.begin();



  dht.begin();

  pinMode (ledPin, OUTPUT);
}

void loop() {
  //Reading Sensores
  Wire.beginTransmission (SLAVE_ADDRESS);
  h = dht.readHumidity();
  t = dht.readTemperature();
  l = analogRead(lumPin);
  m = analogRead(moistPin);
  w = analogRead(wetPin);
  sT = getTemp();


  //Converting Units

  l = exp(0.02639 * l - 0.7512);
  m = map(m, mMin, mMax, 0, 100);
  w = map(w, wMin, wMax, 100, 0);




  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  //  if (isnan(t) || isnan(h) {
  //    Serial.println("Failed to read from DHT");
  //  } else {

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

  I2C_writeAnything (h);
  I2C_writeAnything (t);
  I2C_writeAnything (l);
  I2C_writeAnything (m);
  I2C_writeAnything (sT);
  I2C_writeAnything (w);



  if (t < 20) {
    digitalWrite (ledPin, HIGH);
  }
  else {
    digitalWrite (ledPin, LOW);
  }

  Wire.endTransmission ();
}



float getTemp() {
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
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

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



