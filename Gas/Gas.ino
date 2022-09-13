#include "LowPower.h"

const int sensor = 2;
const int led = 9;
const int btn = 3;
const int sound = 8;

const int soundDuration = 20000;

bool ledState = false;

volatile bool stopFlag = false;

unsigned long lastMil;

void setup() {
  pinMode(sensor, INPUT);
  pinMode(btn, INPUT);

  pinMode(led, OUTPUT);
  pinMode(sound, OUTPUT);
}

void loop() {
  attachInterrupt(digitalPinToInterrupt(sensor), empt, FALLING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(sensor));

  attachInterrupt(digitalPinToInterrupt(btn), stopV, RISING);

  lastMil = millis();

  while (!stopFlag)
  {
    ledState = !ledState;
    digitalWrite(led, ledState);

    makeSound();

    delay(200);

    if (millis() - lastMil > soundDuration)
    {
      stopFlag = true;
    }
  }

  digitalWrite(led, LOW);

  detachInterrupt(digitalPinToInterrupt(btn));
  ledState = false;
  stopFlag = false;
}

void makeSound()
{
  for (int i = 1500; i < 2600; i += 10)
  {
    tone(sound, i, 5);
    delay(5);
  }

  noTone(sound);
  delay(10);
}

static void stopV()
{
  stopFlag = true;
  detachInterrupt(digitalPinToInterrupt(btn));
}

static void empt() {}
