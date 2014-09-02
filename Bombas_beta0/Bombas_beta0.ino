
#define motorPin 3

int calibrador = A0;
int treshold = 0;



void setup(){
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
  Serial.println("Iniciar Programa!");

 
}


void loop(){

analogWrite(motorPin,170);
delay(30000); 




 
}
