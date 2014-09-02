// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h"

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

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 100;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht.begin();
  delay(60000);
  while(!Serial);
  Bridge.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  String Temp = String(t);
  String Humid = String(h);

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    writeHumid(Humid);
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    writeTemp(Temp);
    Serial.println("Waiting...");
    delay (300000);
  }
}

void writeTemp(String Temp){
 Serial.println("Running WriteData - Run #" + String(numRuns++));

    TembooChoreo WriteDataChoreo;

    // Invoke the Temboo client
    WriteDataChoreo.begin();
    
    // Set Temboo account credentials
    WriteDataChoreo.setAccountName(TEMBOO_ACCOUNT);
    WriteDataChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    WriteDataChoreo.setAppKey(TEMBOO_APP_KEY);
    
    
    // Set Choreo inputs
    WriteDataChoreo.addInput("DatastreamID", "Temperature");
    WriteDataChoreo.addInput("Value", Temp);
    WriteDataChoreo.addInput("FeedID", "1172012940");
    WriteDataChoreo.addInput("APIKey", "L98BafXJVVfYGwynAdCuA0OhSanaxeJQO77VIslARdhkyUuD");

    // Identify the Choreo to run
    WriteDataChoreo.setChoreo("/Library/Xively/ReadWriteData/WriteData");
    
    // Run the Choreo; when results are available, print them to serial
    WriteDataChoreo.run();
   
    
    while(WriteDataChoreo.available()) {
      char c = WriteDataChoreo.read();
      Serial.print(c);
    }
    WriteDataChoreo.close();

}

void writeHumid(String Humid){
 Serial.println("Running WriteData - Run #" + String(numRuns++));

    TembooChoreo WriteDataChoreo;

    // Invoke the Temboo client
    WriteDataChoreo.begin();
    
    // Set Temboo account credentials
    WriteDataChoreo.setAccountName(TEMBOO_ACCOUNT);
    WriteDataChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    WriteDataChoreo.setAppKey(TEMBOO_APP_KEY);
    
    
    // Set Choreo inputs
    WriteDataChoreo.addInput("DatastreamID", "Humidity");
    WriteDataChoreo.addInput("Value", Humid);
    WriteDataChoreo.addInput("FeedID", "1172012940");
    WriteDataChoreo.addInput("APIKey", "L98BafXJVVfYGwynAdCuA0OhSanaxeJQO77VIslARdhkyUuD");

    // Identify the Choreo to run
    WriteDataChoreo.setChoreo("/Library/Xively/ReadWriteData/WriteData");
    
    // Run the Choreo; when results are available, print them to serial
    WriteDataChoreo.run();
   
    
    while(WriteDataChoreo.available()) {
      char c = WriteDataChoreo.read();
      Serial.print(c);
    }
    WriteDataChoreo.close();

}
