#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define PIR 4
#define D22 11

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht22(D22, DHT22);

bool isOff = true;

void setup() {
  Serial.begin(9600);

  pinMode(PIR, INPUT);
  pinMode(D22, INPUT);

  dht22.begin();

  lcd.init();
  lcd.clear();
}

void loop() {
  int pirStat = digitalRead(PIR);
  if (pirStat == HIGH)
  {
    float h = dht22.readHumidity();
    float t = dht22.readTemperature();

    lcd.clear();

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

    lcd.display();
    lcd.backlight();

    isOff = false;

    delay(6000);
  }
  else
  {
    if (!isOff)
    {
      lcd.noBacklight();
      lcd.noDisplay(); 
      isOff = true;
    }
  }
}
