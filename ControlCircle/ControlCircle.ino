#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const int controlPin = A3;
const int resetPin = 5;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

void setup() {
  pinMode(controlPin, INPUT);
  pinMode(resetPin, INPUT);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  delay(1000);
  display.clearDisplay();
  display.display();

  fall();
}

void loop() {
}

#define XPOS 0
#define YPOS 1
#define SPEED 1

bool stop = false;

unsigned long prevMil = 0;

void fall()
{
  int data[3];

  data[XPOS] = display.width() / 2;
  data[YPOS] = 0;

  for (;;) {
    if (digitalRead(resetPin) == HIGH)
    {
      data[YPOS] = 0;
      Serial.println("Res");
    }

    int rad = map(analogRead(controlPin), 0, 1022, 0, 31);

    display.clearDisplay();
    display.drawLine(0, display.height() - 1, display.width() - 1, display.height() - 1, SSD1306_WHITE);
    display.drawCircle(data[XPOS], data[YPOS], rad, SSD1306_WHITE);
    display.display();

    int currentMil = millis();

    int fromlastFrame = currentMil - prevMil;

    if (!stop && fromlastFrame >= 100)
    {
      data[YPOS] += SPEED * fromlastFrame/100;

      prevMil = currentMil;
    }

    if (data[YPOS] + rad + 2 >= display.height())
    {
      stop = true;
    }
    else
    {
      stop = false;
    }    
  }
}
