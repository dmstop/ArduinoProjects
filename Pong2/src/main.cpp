#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

uint8_t ball_x = 0, ball_y = 32;

unsigned long prevMil = 0;

void setup()
{
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();

  prevMil = millis();
}

void loop()
{
  unsigned long currMil = millis();
  unsigned long diff = currMil - prevMil;
  prevMil = currMil;

  if (ball_x == SCREEN_WIDTH + 2)
  {
    ball_x = 0;
  }

  display.fillCircle(ball_x, ball_y, 2, BLACK);
  display.fillCircle(++ball_x, ball_y, 2, WHITE);

  display.display();

  Serial.println(1000 / diff);
}