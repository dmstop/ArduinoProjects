#include <Arduino.h>
#include <Servo.h>

#define lPin A6
#define servoPin 9

const int cactus = 740;

Servo servo;

void setup()
{
  pinMode(lPin, INPUT);
  servo.attach(servoPin);
  servo.write(90);

  Serial.begin(9600);
}

void loop()
{
  if (analogRead(lPin) > cactus)
  {
    servo.write(150);
    delay(200);
    servo.write(90);
  }
}