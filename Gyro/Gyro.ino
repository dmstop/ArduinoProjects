#include <LedControl.h>
#include <Wire.h>
#include <MPU6050.h>

const int DIN = 2;
const int CS = 3;
const int CLK = 6;

MPU6050 mpu;

LedControl lc = LedControl(DIN, CLK, CS, 0);

int prevX, prevY;

void setup() {
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
}

void loop() {
  Vector normAccel = mpu.readNormalizeAccel();

  int x = map(normAccel.XAxis, -5, 5, 7, 0);
  x = x > 7 ? 7 : x < 0 ? 0 : x;

  int y = map(normAccel.YAxis, -5, 5, 0, 7);
  y = y > 7 ? 7 : y < 0 ? 0 : y;

  if(x == prevX && y == prevY)
  {
    return;    
  }  

  lc.setLed(0, prevY, prevX, false);  
  lc.setLed(0, y, x, true);

  prevX = x;
  prevY = y;

  delay(10);
  
}
