#include <Arduino.h>
#define pot1 39 

int valor;
float Voltaje=0;
unsigned long acMillis = 0;
const long prevmillis = 1000;
void setup() {
  Serial.begin(9600);
 
}

void loop() { 
  unsigned long millisactual = millis();
  if (millisactual-prevmillis>=prevmillis)
  {
    acMillis=millisactual;
valor=analogRead(pot1);
  Voltaje=(3.3/4095.0)*valor;
   Serial.print("\nVoltaje: \n" );
  Serial.print(Voltaje);
  }
  
}
