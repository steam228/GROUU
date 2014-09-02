const int numReadings = 20;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int pHPin = A3;


void setup() {

  Serial.begin(9600);
   for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}


void loop() {
  
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(pHPin); 
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
  
  Serial.println(average); 

  float pH = (0.02*average)-2.92;


  Serial.println(pH,1);
  
  delay(200); 
}



