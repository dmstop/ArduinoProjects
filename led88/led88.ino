
#include <LedControl.h>

const int joyX = A4;
const int joyY = A5;

const int DIN = 2;
const int CS = 3;
const int CLK = 6;
LedControl lc = LedControl(DIN, CLK, CS, 0);

void setup() {
  Serial.begin(9600);

  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);

  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
}

int prevLedX;
int prevLedY;

void loop() {
  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);
  int xMap = map(yValue, 0, 1021, 7, 0);
  int yMap = map(xValue, 0, 1021, 7, 0);

  lc.setLed(0, prevLedX, prevLedY, false);
  lc.setLed(0, xMap, yMap, true);

  prevLedX = xMap;
  prevLedY = yMap;
}
