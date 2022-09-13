#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <LowPower.h>

const int D22 = 9;
const int BTN = 2;
const int LED = 13;
const int SPWR = 7;

const int blinks = 10;
const int blinkL = 50;
const int screenL = 10000;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht22(D22, DHT22);

void setup() {
  pinMode(BTN, INPUT);
  pinMode(D22, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(SPWR, OUTPUT);

  dht22.begin();

  lcd.init();
  lcd.noBacklight();
  lcd.clear();
}

void loop() {
  attachInterrupt(digitalPinToInterrupt(BTN), empt, RISING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(BTN));

  digitalWrite(SPWR, HIGH);
  lcd.init();
  lcd.backlight();

  float h = dht22.readHumidity();
  float t = dht22.readTemperature();

  lcd.setCursor(0, 0);
  lcd.print(F("T:"));
  lcd.print(t, 1);
  lcd.print(F("c  "));

  lcd.print(F("H:"));
  lcd.print(h, 1);
  lcd.print(F("% "));

  lcd.setCursor(0, 1);
  lcd.print(F("  Feels:"));
  lcd.print(dht22.computeHeatIndex(t, h, false));
  lcd.print(F("c  "));

  for (int i = 0; i < blinks; i++)
  {
    digitalWrite(LED, HIGH);
    delay(blinkL);
    digitalWrite(LED, LOW);
    delay(blinkL);
  }

  delay(constrain(screenL - blinkL * blinks, 0, screenL));

  lcd.noBacklight();
  digitalWrite(SPWR, LOW);
}

static void empt() {}
