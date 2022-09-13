#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <LowPower.h>

const byte D22 = 9;
const byte PIR = 2;
const byte LED = 13;
const byte SPWR = 7;

const byte blinks = 10;
const byte blinkL = 50;
const int screenL = 10000;

const unsigned long sleepT = 30000;

unsigned long lastMil;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht22(D22, DHT22);

void setup() {
  pinMode(PIR, INPUT);
  pinMode(D22, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(SPWR, OUTPUT);

  digitalWrite(SPWR, HIGH);

  dht22.begin();
  lcd.init();

  showInfo();

  lastMil = millis();
}

void loop() {
  unsigned long currMil = millis();

  if (currMil - lastMil >= sleepT)
  {
    digitalWrite(SPWR, LOW);
    attachInterrupt(digitalPinToInterrupt(PIR), empt, RISING);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    detachInterrupt(digitalPinToInterrupt(PIR));
    
    digitalWrite(SPWR, HIGH);
    lcd.init();
    showInfo();

    lastMil = millis();
  }

  if (digitalRead(PIR) == HIGH)
  {
    showInfo();
    lastMil = millis();
  }
}

void showInfo()
{
  lcd.display();
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
  lcd.noDisplay();
}

static void empt() {}
