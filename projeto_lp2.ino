#include <Ultrasonic.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LED_CAIXA_NIVEL_BAIXO 2
#define LED_CAIXA_NIVEL_MEDIO 3
#define LED_CAIXA_NIVEL_ALTO  4

#define LED_TELHADO           7
#define INTENSIDADE_CHUVA     100

#define PORTA_SENSOR_DE_RESPINGO  A0
#define PORTA_SENSOR_DE_LUZ       A1
#define PORTA_SENSOR_UMIDADE_SOLO A2

#define INTENSIDADE_LUZ          65

#define PIN_TRIGGER 5
#define PIN_ECHO    6

#define vcc 9
#define gnd 8

int luz, umiTerra, respingo;
float umiAr, tempAr, nivelAgua;

Ultrasonic ultrasonic(PIN_TRIGGER, PIN_ECHO);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(LED_CAIXA_NIVEL_BAIXO, OUTPUT);
  pinMode(LED_CAIXA_NIVEL_MEDIO, OUTPUT);
  pinMode(LED_CAIXA_NIVEL_ALTO, OUTPUT);
  pinMode(LED_TELHADO, OUTPUT);
  pinMode(PORTA_SENSOR_DE_RESPINGO, INPUT);
  pinMode(PORTA_SENSOR_DE_LUZ, INPUT);
  pinMode(PORTA_SENSOR_UMIDADE_SOLO, INPUT);
  lcd.begin();
  //lcd.backlight();
  lcd.print("Estufa Sabida");
  lcd.setCursor(5,1);
  lcd.print("LP2");
  delay(500);
  //lcd.setBacklight(LOW);
  delay(1000);
}

void loop() {
  setCaixa();
  if (lerLuz() > 60) {
    Serial.println("EH DE DIA");
    setRespingo();
    lerSolo();
  }
  else {
    // de noite
    Serial.println("EH DE NOITE");
    digitalWrite(LED_TELHADO, LOW);
  }
  setDisplay();
  delay(100);
}

void setDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NA: ");
  lcd.setCursor(4, 0);
  lcd.print((int)nivelAgua);
  lcd.setCursor(7, 0);
  lcd.print("US: ");
  lcd.setCursor(11, 0);
  lcd.print(umiTerra);
}

void setRespingo() {
  if (lerRespingo() > INTENSIDADE_CHUVA) {
    digitalWrite(LED_TELHADO, HIGH);
  }
  else {
    digitalWrite(LED_TELHADO, LOW);
  }
}

void setCaixa() {
  float nivel = lerNivel();
  if(nivel > 70) {
      digitalWrite(LED_CAIXA_NIVEL_ALTO, HIGH);
      digitalWrite(LED_CAIXA_NIVEL_MEDIO, LOW);
      digitalWrite(LED_CAIXA_NIVEL_BAIXO, LOW);
    }
    else if(nivel > 40 && nivel < 70) {
      digitalWrite(LED_CAIXA_NIVEL_ALTO, LOW);
      digitalWrite(LED_CAIXA_NIVEL_MEDIO, HIGH);
      digitalWrite(LED_CAIXA_NIVEL_BAIXO, LOW);
    }
    else {
      digitalWrite(LED_CAIXA_NIVEL_ALTO, LOW);
      digitalWrite(LED_CAIXA_NIVEL_MEDIO, LOW);
      digitalWrite(LED_CAIXA_NIVEL_BAIXO, HIGH);
    }
}

int lerSolo() {
  int anaValue;
  digitalWrite(vcc, LOW);
  digitalWrite(gnd, HIGH);
  delay(2000);
  anaValue = analogRead(PORTA_SENSOR_UMIDADE_SOLO);
  digitalWrite(vcc, HIGH);
  digitalWrite(gnd, LOW);
  delay(2000);
  digitalWrite(vcc, LOW);
  Serial.println("UMISOLOO");
  Serial.println(anaValue);
  anaValue = map(anaValue, 1023, 0, 0, 100);
  umiTerra = anaValue;
  return anaValue;
}

float lerNivel() {
  float cm;
  long microsec = ultrasonic.timing();
  cm = ultrasonic.convert(microsec, Ultrasonic::CM);
  if (cm > 40) {
    cm = 15;
  }
  cm = map(cm, 0, 40, 0, 100);
  nivelAgua = cm;
  return cm;
}

int lerRespingo() {
  int soma = 0;
  int quant_amostras = 20;
  for (int i = 0; i < quant_amostras; i++) {
    soma += analogRead(PORTA_SENSOR_DE_RESPINGO);
    delay(20);
  }
  respingo = soma / quant_amostras;
  return respingo;
}

int lerLuz() {
  int anaValue = 0;
  int quant_amostras = 10;
  for(int i = 0; i < quant_amostras; i++) {     
    anaValue += analogRead(PORTA_SENSOR_DE_LUZ);
    delay(50);   
  }      
  anaValue = anaValue/quant_amostras;
  anaValue = map(anaValue, 55, 1023, 0, 100);
  luz = anaValue;
  return anaValue;  
}
