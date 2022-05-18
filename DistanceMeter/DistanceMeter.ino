#include <LiquidCrystal_I2C.h>
#include <NewPing.h> 
#include "DHT.h"

#define LED 5

#define DHTPIN 6
#define DHTTYPE DHT11

#define TRIGGER_PIN 8
#define ECHO_PIN 7
#define MAX_DISTANCE 400

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  pinMode(LED, OUTPUT);
  
  dht.begin();

  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void loop() {
  int h = dht.readHumidity();
  float t = dht.readTemperature();

  float speedOfSound = 331.4 + (0.6 * t) + (0.0124 * h);
  
  float duration = sonar.ping_median(10);
  duration /= 1000000;
  
  float distance = (speedOfSound * duration)/2;
  distance = distance * 100;

  distance = distance <= 0 ? MAX_DISTANCE : distance;
  
  lcd.setCursor(0, 0);
  lcd.print(F("T:"));
  lcd.print(t);
  lcd.setCursor(6, 0);
  lcd.print('c');
  lcd.print(F(" H:"));
  lcd.print(h);
  lcd.print('%');

  lcd.setCursor(0, 1);
  lcd.print(F("Dist: "));
  lcd.print(distance);
  lcd.print(F("cm"));
  lcd.print(F("   "));

  digitalWrite(LED, distance < 10 ? HIGH : LOW);
  
  delay(500);
}
