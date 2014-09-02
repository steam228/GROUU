
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" 
#include "DHT.h"
#include "OneWire.h"
#include "math.h"

//Sensores

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
  

const unsigned long RUN_INTERVAL_MILLIS = 900000;
unsigned long lastRun = (unsigned long)-900000;
String data;


void setup() {
  Bridge.begin();
  Console.begin();
  delay(4000);
//  while(!Console);
  Console.print("Initializing the bridge...");
  Bridge.begin();
  Console.println("Bridge done!");
  delay(1000);
  Console.println("DHTxx test");
  dht.begin();
  delay(1000);
  Console.println("DHTxx test done!");
  
  pinMode (ledPin, OUTPUT);
}

void loop() {
    
    unsigned long now = millis();
    
    h = getValues(2);
    t = getValues(1);
    l = getValues(3);
    m = getValues(4);
    sT = getValues(5);
    w = getValues(6);
    
    Console.print("Relative Humidity: ");
    Console.println(h);
    Console.print("Temperature: ");
    Console.println(t);
    Console.print("Luminosity: ");
    Console.println(l);
    Console.print("Moisture: ");
    Console.println(m);
    Console.print("Soil Temperature: ");
    Console.println(sT);
    Console.print("Leaf Wetness: ");
    Console.println(w);
    Console.println("///////////////////////////////");
    Console.println("///////////////////////////////");
    
    delay(1000);
    
    if (now - lastRun >= RUN_INTERVAL_MILLIS) {
      
    lastRun = now;  
    Console.println("Reading Sensors");
    

    
    
    // escreve na String CSV
    String Valores = getTime();
    Valores += ",";
    Valores += String(t);
    Valores += ",";
    Valores += String(h);
    Valores += ",";
    Valores += String(l);
    Valores += ",";
    Valores += String(m);
    Valores += ",";
    Valores += String(sT);
    Valores += ",";
    Valores += String(w);
    
    TembooChoreo AppendRowChoreo;

    // Invoke the Temboo client
    AppendRowChoreo.begin();
    
    // Set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // Set credential to use for execution
    AppendRowChoreo.setCredential("STEAM228");
    
    // Set Choreo inputs
    AppendRowChoreo.addInput("SheetName", "GROUU_EXP");
    AppendRowChoreo.addInput("RowData", Valores);
    AppendRowChoreo.addInput("SpreadsheetTitle", "GROUU_EXP");
    
    // Identify the Choreo to run
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
    
    // Run the Choreo; when results are available, print them to serial
    AppendRowChoreo.run();
    
    while(AppendRowChoreo.available()) {
      char d = AppendRowChoreo.read();
//      Console.print(d);
    }
    AppendRowChoreo.close();
     Console.println("Waiting...");
    }

  } 


////////////////////////////////////////
// APANHA VALORES //////////////////////
////////////////////////////////////////

float getValues (int option){
  
  //Reading Sensores
  
  float humid = dht.readHumidity();
  float temp = dht.readTemperature();
  float lum = analogRead(lumPin);
  float moist = analogRead(moistPin);
  float wetness = analogRead(wetPin);
  float sTemp = getTemp();
  
  //Analog calibration Values for Maping (input here)
  
  int mMax=600;
  int mMin=0;
  
  int wMax=1013;
  int wMin=100;
  
  float lMax=1023/2;
  int lMin=0;
  
   //Converting Units
    
  lum = exp(0.02639*lum-0.7512);   
  moist = map(moist,mMin,mMax,0,100);
  wetness = map(wetness,wMin,wMax,100,0);

  if (isnan(t) || isnan(h)) {
    Console.println("Failed to read from DHT");
  } else {

  if (option == 1) {
  return temp;
  }
  else if (option == 2) {
  return humid;
  }
  else if (option == 3) {
  return lum;
  }
  else if (option == 4) {
  return moist;
  }
  else if (option == 5) {
  return sTemp;
  }
  else if (option == 6) {
  return wetness;
  }
}
}

String getTime (){
//  Console.println("Running FormatTimestamp");
    
    TembooChoreo FormatTimestampChoreo;

    // Invoke the Temboo client
    FormatTimestampChoreo.begin();
    
    // Set Temboo account credentials
    FormatTimestampChoreo.setAccountName(TEMBOO_ACCOUNT);
    FormatTimestampChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    FormatTimestampChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // Set Choreo inputs
    FormatTimestampChoreo.addInput("Format", "yyyy-MM-dd'T'HH:mm:ss");
    FormatTimestampChoreo.addInput("TimeZone", "UTC");
    
    // Identify the Choreo to run
    FormatTimestampChoreo.setChoreo("/Library/Utilities/Formatting/FormatTimestamp");
    
    // Run the Choreo; when results are available, print them to serial
    FormatTimestampChoreo.run();
    
    while(FormatTimestampChoreo.available()) {
     // read the name of the next output item
      String name = FormatTimestampChoreo.readStringUntil('\x1F');
      name.trim(); // use “trim” to get rid of newlines

      // read the value of the next output item
      String data = FormatTimestampChoreo.readStringUntil('\x1E');
      data.trim(); // use “trim” to get rid of newlines

      if (name == "FormattedDate") {
//      Console.println("The date is " + data);
      return data;
    }
    FormatTimestampChoreo.close();

  }}
  
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
//   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
//   Serial.print("Device is not recognized");
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
