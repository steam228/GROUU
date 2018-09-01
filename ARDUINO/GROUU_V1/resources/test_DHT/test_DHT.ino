
//test DHT on 

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            2        
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);

float temp;
float humid;


void setup() {
  Serial.begin(115200);

}

void loop() {
  /// DHT22 values
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    temp = event.temperature;
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C");
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    humid = event.relative_humidity;
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println("%");


  }
  delay(400);

}
