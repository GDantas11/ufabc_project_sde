#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define pinoTermistor 13
#define pinoCartucho 8
#define pinoCartuchoAux 10

#define Vin 5.0
#define T0 298.15
#define Rt 4700.0
#define R0 100000.0

float beta = 3950.0;
float Rinf = 0.0;
float TempKelvin = 0.0;
float TempCelsius = 0.0;
float Vout = 0.0;
float Rout = 0.0;

//motor_A
int IN1 = 6 ;
int IN2 = 5 ;
int velocidadeA = 4;

int vel = 100;

void setup(){

  Serial.begin(9600);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Grupo Extrusora");
  lcd.setCursor(0, 1);
  lcd.print("SPD Engenharia");

  Rinf = R0 * exp(-beta / T0);

  // Cartucho de aquecimento
  pinMode(pinoCartucho, OUTPUT);
  pinMode(pinoCartuchoAux, OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(velocidadeA,OUTPUT);

  delay(2000);
}

void loop(){

  Vout = Vin * ((float)(analogRead(pinoTermistor)) / 1023.0);

  Rout = (Rt * Vout / (Vin - Vout));
  TempKelvin = (beta / log(Rout / Rinf));
  TempCelsius = TempKelvin - 273.15;

  // Imprime os dados no monitor serial, podemos excluir
  Serial.print("Temperatura: ");
  Serial.print(TempCelsius);
  Serial.print(" C, Resistencia: ");
  Serial.println(Rout);
// Exibe no LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura");

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(velocidadeA, vel);

  if (TempCelsius > 180.0) {
    lcd.setCursor(0, 1);
    lcd.print("ideal atingida");

    digitalWrite(pinoCartucho, LOW);
    digitalWrite(pinoCartuchoAux, LOW);
  } else {
    String strTempCelcius = String(TempCelsius, 1) + String((char)223) + "C";
    digitalWrite(pinoCartucho, HIGH);
    digitalWrite(pinoCartuchoAux, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(strTempCelcius);
  }

  delay(1000);
}