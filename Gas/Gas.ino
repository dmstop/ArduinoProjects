#include "LowPower.h"

const int sensor = 2;
const int led = 9;
const int btn = 3;
const int sound = 8;

bool ledState = false;

volatile bool stopFlag = false;

void setup() {
  pinMode(sensor, INPUT);

  pinMode(led, OUTPUT);
  pinMode(sound, OUTPUT);
}

void loop() {
  attachInterrupt(digitalPinToInterrupt(sensor), empt, FALLING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(sensor));

  attachInterrupt(digitalPinToInterrupt(btn), stopV, RISING);

  while (!stopFlag)
  {
    ledState = !ledState;
    digitalWrite(led, ledState);

    makeSound();

    delay(200);
  }

  digitalWrite(led, LOW);
  
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
