// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" 
#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to

#define DHTTYPE DHT22  


DHT dht(DHTPIN, DHTTYPE);
int ledPin = 13;

const unsigned long RUN_INTERVAL_MILLIS = 900000;
unsigned long lastRun = (unsigned long)-900000;
String data;

void setup() {
  Bridge.begin();
//  Console.begin();
//  delay(4000);
//  while(!Console);
//  Console.print("Initializing the bridge...");
  Bridge.begin();
//  Console.println("Bridge done!");
  delay(1000);
  Console.println("DHTxx test");
  dht.begin();
  delay(1000);
  Console.println("DHTxx test done!");
  
  pinMode (ledPin, OUTPUT);
}

void loop() {
    
    unsigned long now = millis();
    
    if (now - lastRun >= RUN_INTERVAL_MILLIS) {
      
    lastRun = now;  
    Console.println("Reading Sensors");
    
    float humid = getValues(2);
    float temp = getValues(1);
    String Valor = getTime();
    Valor += ",";
    Valor += String(temp);
    Valor += ",";
    Valor += String(humid);
    
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
    AppendRowChoreo.addInput("RowData", Valor);
    AppendRowChoreo.addInput("SpreadsheetTitle", "GROUU_EXP");
    
    // Identify the Choreo to run
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
    
    // Run the Choreo; when results are available, print them to serial
    AppendRowChoreo.run();
    
    while(AppendRowChoreo.available()) {
      char d = AppendRowChoreo.read();
      Console.print(d);
    }
    AppendRowChoreo.close();
     Console.println("Waiting...");
    }

  } 

float getValues (int option){
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(t) || isnan(h)) {
    Console.println("Failed to read from DHT");
  } else {

  if (option == 1) {
  return t;
  }
  else if (option == 2) {
  return h;
  }
}
}

String getTime (){
  Serial.println("Running FormatTimestamp");
    
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
      Console.println("The date is " + data);
      return data;
    }
    FormatTimestampChoreo.close();

  }}
