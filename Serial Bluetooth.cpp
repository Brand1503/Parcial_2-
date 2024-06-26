#include <Arduino.h>
#include "BluetoothSerial.h"

#define DAC_CH1 25
#define LED1  18
#define LED2  19

int raw = 0;
unsigned long tinicio,tactual = 0; // Variable para almacenar el tiempo anterior
unsigned long periodo = 15; // Intervalo en milisegundos

String device_name = "ESP32_BLUETOOTH";

BluetoothSerial SerialBT;

String Serialdata = "";
bool dataflag = 0;
String comando = "";
int leds[] = {18,19};

void inicia_leds(void);

void dientedesierra();
void encender_Leds();

unsigned long ti, tf, dt;
String string_pc_a_esp32 = "";
String string_esp32 = "";
String canal = "";
String valor_canal = "";

bool cadena_completa = false;

void escucha_serial2(void);

void setup() {
  ti = tf = dt = 0;
  Serial.begin(115200); // Inicializa el puerto serial 0
  Serial2.begin(115200);  // Inicializa el puerto serial 2
  Serial.println("Iniciando el ESP32");
  string_pc_a_esp32.reserve(50);
  string_esp32.reserve(50);

  inicia_leds();
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("El dispositivo \"%s\" esta listo para emparejar\n", device_name.c_str());
  
}


void loop() {
  escucha_serial2();

  tactual = millis();
  dientedesierra();
  encender_Leds();
  //cuando hay algo para transmitir a bluetooth se hace!!!
  if (cadena_completa) {
    canal = string_esp32.substring(0, 5);
    int indiceInicio = string_esp32.indexOf('#');
    if (indiceInicio != -1) {
      int indiceFin = string_esp32.indexOf('!', indiceInicio);
      if (indiceFin != -1) {
        valor_canal = string_esp32.substring(indiceInicio + 1, indiceFin);
      }
    }
    Serial.println("El canal " + canal + " posee un valor de " + valor_canal);
    string_esp32 = "";
    cadena_completa = false;
  }
}

void encender_Leds() {
  if (dataflag == 1) {
    SerialBT.print(Serialdata);
    Serialdata = "";
    dataflag = 0;
  }

  // Aquí se revisa si algo ha llegado desde el Bluetooth.
  if (SerialBT.available() != 0) {
    while (SerialBT.available() != 0) {
      // Lee cada caracter que se recibe via Bluetooth
      comando += char(SerialBT.read());
    }
    // Envía el comando recibido al puerto serial 2
    Serial2.print(comando);

    // Analiza el string recibido para tomar acción
    if (comando.equals("led_on2")) {
      digitalWrite(LED2, HIGH);
      Serial.println("Encendido el LED19");
      SerialBT.print("Encendido el LED19");
    } else if (comando.equals("led_off2")) {
      digitalWrite(LED2, LOW);
      Serial.println("El LED19 ha sido apagado");
      SerialBT.print("El LED19 ha sido apagado");
    } else if (comando.equals("led_on")) {
      digitalWrite(LED1, HIGH);
      Serial.println("Encendido el LED18");
      SerialBT.print("Encendido el LED18");
    } else if (comando.equals("led_off")) {
      digitalWrite(LED1, LOW);
      Serial.println("El LED18 ha sido apagado");
      SerialBT.print("El LED18 ha sido apagado");
    } else if (comando.equals("DAC2")) {
      digitalWrite(LED1, LOW);
      periodo = 7.81;
    }else if (comando.equals("DAC4")) {
      digitalWrite(LED1, LOW);
      periodo = 15;
    }

    Serial.println(comando);
    comando = "";
  }
}

void serialEvent() {
  while (Serial.available()) {
    char incomingByte = Serial.read();
    string_pc_a_esp32 += incomingByte;
    if (incomingByte == '!') {
      Serial2.print(string_pc_a_esp32);
      string_pc_a_esp32 = "";
    }
  }
}

void escucha_serial2() {
  while (Serial2.available()) {
    char incomingByte = Serial2.read();
    string_esp32 += incomingByte;
    if (incomingByte == '!') {
      cadena_completa = true;
    }
  }
}

void dientedesierra(void){
  // Comparar el tiempo actual con el tiempo anterior
  if (tactual - tinicio >= periodo) {
    // Guardar el tiempo actual como el último tiempo de muestreo
    tinicio = tactual;
    // Escribir el valor actual al DAC
    dacWrite(DAC_CH1, raw);
    // Incrementar el valor
    raw++;
    // Resetear el valor si supera 255
    if (raw > 255) {
      raw = 0;
    }
  }
}

void inicia_leds(void){
  u_int8_t i;
  for(i=0;i<2;++i){
    pinMode(leds[i],OUTPUT);
    digitalWrite(leds[i],LOW);
    }
}
