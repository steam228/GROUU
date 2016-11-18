// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

//Leaf wettness

int lw = A2;
int moist = A0;
int light = A4;


void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  delay(1000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int lwv = analogRead(lw);
  lwv = map(lwv, 1023,0,0,15);
  int moisture = analogRead(moist);
  float luxs = analogRead(light);
  luxs = map(luxs, 0, 1023, 0.003, 70);
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h) || isnan(lwv) || isnan(moisture)) {
    Serial.println("Failed to read from DHT or LW or MOIST");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    Serial.print(lwv);
    Serial.println(" LWSC");
    Serial.print(moisture);
    Serial.println(" moist");
    Serial.print(luxs);
    Serial.println(" klx");
    
  }
}
