#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define THERMISTOR_PIN 13
#define CARTRIDGE_PIN 8
#define AUX_CARTRIDGE_PIN 10
#define DRIVER_1_PIN 5
#define DRIVER_2_PIN 6
#define DRIVER_SPEED_PIN 11

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
float limit_temp = 200.0;
int speed_pwm = 0;
int initial_speed_pwm = 250;
int pause_time = 1000;
int limit_time = 150;
unsigned long end_time = 0;
unsigned long delta_time = 0;

void setup(){
	Serial.begin(9600);

	lcd.init();
	lcd.backlight();

	aux_resistance = initial_resistance_therm * exp(-beta / initial_temp);

	pinMode(CARTRIDGE_PIN, OUTPUT);
	pinMode(AUX_CARTRIDGE_PIN, OUTPUT);
	pinMode(DRIVER_1_PIN, OUTPUT);
	pinMode(DRIVER_2_PIN, OUTPUT);
	pinMode(DRIVER_SPEED_PIN, OUTPUT);

	delay(pause_time);
}

void loop(){
	static unsigned long start_time = millis();

	voltage = default_voltage * ((float)(analogRead(THERMISTOR_PIN)) / 1023.0);
	resistance_therm = (serie_resistance * voltage / (default_voltage - voltage));
	kelvin_temp_voltage = (beta / log(resistance_therm / aux_resistance));
	celsius_temp = kelvin_temp_voltage - 273.15;

	pressMetrics(celsius_temp, resistance_therm, speed_pwm, delta_time);

	String strTempCelcius = "Temperatura: " + String(celsius_temp, 1) + String((char)223) + "C";

	digitalWrite(DRIVER_1_PIN, HIGH);
	digitalWrite(DRIVER_2_PIN, LOW);
	analogWrite(DRIVER_SPEED_PIN, speed_pwm);

	end_time = millis();
	delta_time = (end_time - start_time) / 1000;

	if ((celsius_temp >= limit_temp) & (delta_time <= (limit_time + 2 * (pause_time / 1000)))) {
		if ((celsius_temp > 180.0) & (celsius_temp < (limit_temp - 10.0)) & (speed_pwm != initial_speed_pwm)){
			speed_pwm = initial_speed_pwm;
		}

		while(speed_pwm > 110){
			speed_pwm = speed_pwm - 10;
		}

		displayLcd("Ideal atingido", strTempCelcius);

		digitalWrite(CARTRIDGE_PIN, LOW);
		digitalWrite(AUX_CARTRIDGE_PIN, LOW);
	}
	else {
		displayLcd("Aquecendo.", strTempCelcius);
		displayLcd("Aquecendo..", strTempCelcius);
		displayLcd("Aquecendo...", strTempCelcius);

		digitalWrite(CARTRIDGE_PIN, HIGH);
		digitalWrite(AUX_CARTRIDGE_PIN, HIGH);
	}

	delay(pause_time);
}

void displayLcd(String first_label, String second_label){
	lcd.setCursor(0, 0);
	lcd.print(first_label);
	lcd.setCursor(0, 1);
	lcd.print(second_label);
}

void pressMetrics(float temp, float resistance, int speed, float time){
	String str_temp = "Temperatura: " + String(temp, 1) + String((char)223) + "C";
	String str_resistance = "; Resistência: " + String(resistance, 1) + " ohm";
	String str_speed = "; Velocidade: " + String(speed, 1) + " pwm";
	String str_time = "; Intervalo: " + String(time, 1) + "s";

	Serial.print(str_temp);
	Serial.print(str_resistance);
	Serial.print(str_speed);
	Serial.print(str_time);
	Serial.println("");
}
