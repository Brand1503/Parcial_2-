#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define led1 18
#define led2 19

#define pot1 39 

int raw = 0;

String comando = "";
String cabecera = "";
String num_str = "";
String num_pwm = "";
String num_vol = "";
bool cmd_ok = false;
uint16_t res_adc;

#define DAC_CH1 26

#define DUTY_MAX_MOTOR 1023
#define PIN_PWM_MOTOR 32
#define CANAL_PWM_MOTOR 3
#define RES_BITS_MOTOR 10
#define FREC_PWM_MOTOR 100

unsigned long periodo = 15;
unsigned long tinicio = 0;
unsigned long tactual = 0;
int valor; 
float vol_pot=0;
unsigned long acMillis = 0;
const long prevmillis = 1000;
u_int dutyCycle_motor = 0;


void lectura_serial(void);
void dientedesierra(void);
void voltaje_potenciometro(void);
void mostrarEnPantalla(String mensaje);

void setup() {
  ledcSetup(CANAL_PWM_MOTOR, FREC_PWM_MOTOR, RES_BITS_MOTOR); // Configurar el canal PWM con frecuencia de 100 Hz y resolución de 10 bits
  ledcAttachPin(PIN_PWM_MOTOR, CANAL_PWM_MOTOR); // Asociar el pin motor al canal PWM

  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
  pinMode(led2, OUTPUT);
  digitalWrite(led2, LOW);
  Serial2.begin(115200);
  comando.reserve(50);

  // Inicializar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Dirección I2C 0x3C para la pantalla OLED
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();

  mostrarEnPantalla("Iniciando...");
}


void loop() {
  lectura_serial();
  dientedesierra();
  voltaje_potenciometro();

  if (cmd_ok) {
    cabecera = comando.substring(0, 3);
    num_str = comando.substring(3, 5);
    mostrarEnPantalla(comando); // Mostrar el comando en la pantalla OLED

    if (cabecera.equals("LED")) {
      digitalWrite(num_str.toInt(), !digitalRead(num_str.toInt()));
    } else if (cabecera.equals("ADC")) {
      res_adc = analogRead(num_str.toInt());
      Serial2.print(cabecera + num_str + "#" + res_adc + "!");
    }else if (cabecera.equals("POT")){
      num_vol = comando.substring(3,6);
      vol_pot = num_vol.toInt();
    }else if (cabecera.equals("PWM")) {
      num_pwm = comando.substring(3, 6);
      dutyCycle_motor = num_pwm.toInt();
    } else if (cabecera.equals("DAC")) {
      num_str = comando.substring(3, 4);
      int num = num_str.toInt();
      if (num == 2) {
        periodo = 7.81;
      } else if (num == 4) {
        periodo = 15;
      }
    }
    comando = "";
    cmd_ok = false;
  }

  ledcWrite(CANAL_PWM_MOTOR, dutyCycle_motor); // Escribir el ciclo de trabajo en el canal PWM
}

void lectura_serial(void) {
  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    comando += inChar;
    mostrarEnPantalla(comando);
    if (inChar == '!') {
      cmd_ok = true;
    }
  }
}

void dientedesierra(void) {
  tactual = millis();
  if (tactual - tinicio >= periodo) { // Comparar el tiempo actual con el tiempo anterior
    tinicio = tactual; // Guardar el tiempo actual como el último tiempo de muestreo
    dacWrite(DAC_CH1, raw); // Escribir el valor actual al DAC
    raw++; // Incrementar el valor
    if (raw > 255) {
      raw = 0; // Resetear el valor si supera 255
    }
  }
}
 void voltaje_potenciometro(){
    unsigned long millisactual = millis();
  if (millisactual-prevmillis>=prevmillis)
  {
    acMillis=millisactual;
valor=analogRead(pot1);
  vol_pot=(3.3/4095.0)*valor;
   Serial.print("\nVoltaje: \n" );
  Serial.print(vol_pot);
  }
 }
void mostrarEnPantalla(String mensaje) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(mensaje);
  display.display();
}
