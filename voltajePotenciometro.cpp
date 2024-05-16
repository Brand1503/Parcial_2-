#include <Arduino.h>
#define pot1 39
int valor;
float Voltaje=0;
void setup() {
  Serial.begin(9600);
 
}

void loop() {
  valor=analogRead(pot1);
  Voltaje=(3.3/4095.0)*valor;
   Serial.print("\nVoltaje: \n" );
  Serial.print(Voltaje);
  delay(1000);
}
