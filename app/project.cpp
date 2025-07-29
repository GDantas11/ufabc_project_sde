#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define THERMISTOR_PIN 13
#define CARTRIDGE_PIN 8
#define AUX_CARTRIDGE_PIN 10
#define DRIVER_1_PIN 6
#define DRIVER_2_PIN 5
#define DRIVER_SPEED_PIN 4

float default_voltage = 5.0;
float initial_temp = 298.15;
float serie_resistance = 4700.0;
float initial_resistance_therm = 100000.0;
float beta = 3950.0;
float aux_resistance = 0.0;
float kelvin_temp_voltage = 0.0;
float celsius_temp = 0.0;
float voltage = 0.0;
float resistance_therm = 0.0;
int seep_pwm = 100;

void setup(){
	Serial.begin(9600);

	lcd.init();
	lcd.backlight();
	lcd.setCursor(0, 0);
	lcd.print("Grupo Extrusora");
	lcd.setCursor(0, 1);
	lcd.print("SPD Engenharia");

	aux_resistance = initial_resistance_therm * exp(-beta / initial_temp);

	pinMode(CARTRIDGE_PIN, OUTPUT);
	pinMode(AUX_CARTRIDGE_PIN, OUTPUT);
	pinMode(DRIVER_1_PIN, OUTPUT);
	pinMode(DRIVER_2_PIN, OUTPUT);
	pinMode(DRIVER_SPEED_PIN, OUTPUT);

	delay(1000);
}

void loop(){
	heating();

	Serial.print("Temperatura: ");
	Serial.print(celsius_temp);
	Serial.print(" C, Resistencia: ");
	Serial.println(resistance_therm);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Temperatura");

	digitalWrite(DRIVER_1_PIN, HIGH);
	digitalWrite(DRIVER_2_PIN, LOW);
	analogWrite(DRIVER_SPEED_PIN, seep_pwm);

	if (celsius_temp > 180.0) {
		lcd.setCursor(0, 1);
		lcd.print("ideal atingida");

		digitalWrite(CARTRIDGE_PIN, LOW);
		digitalWrite(AUX_CARTRIDGE_PIN, LOW);
	}
	else {
		String strTempCelcius = String(celsius_temp, 1) + String((char)223) + "C";

		digitalWrite(CARTRIDGE_PIN, HIGH);
		digitalWrite(AUX_CARTRIDGE_PIN, HIGH);

		lcd.setCursor(0, 1);
		lcd.print(strTempCelcius);
	}

	delay(1000);
}

void heating(){
	voltage = default_voltage * ((float)(analogRead(THERMISTOR_PIN)) / 1023.0);

	resistance_therm = (serie_resistance * voltage / (default_voltage - voltage));
	kelvin_temp_voltage = (beta / log(resistance_therm / aux_resistance));
	celsius_temp = kelvin_temp_voltage - 273.15;
}