#include <LiquidCrystal_I2C.h>
#include <LowPower.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensPin = 2;

void wake()
{  
}

void setup() {
  pinMode(sensPin, INPUT);
  lcd.init();
}

void loop() {

  attachInterrupt(digitalPinToInterrupt(sensPin), wake, RISING);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  detachInterrupt(0);

  lcd.display();
  lcd.backlight();
  lcd.clear();

  int i;
  for (i = 0; i < 101; i++) {
    lcd.setCursor(0, 0);
    lcd.print(i);
    delay(50);
  }

  delay(1000);

  lcd.noBacklight();
  lcd.noDisplay();

}
