#include <Servo.h>

Servo serv;

const int piezo = A0;
const int switchPin = 12;
const int yellowLed = 7;
const int greenLed = 5;
const int redLed = 3;

const int quietKnock = 180;

const int knocksNeeded = 4;

const int knocksDelays[knocksNeeded] = {0, 200, 200, 500};

bool locked = false;

bool isFirstKnock = true;

unsigned long lastUpdate;

int correctKnocks = 0;

void setup() {
  serv.attach(11);
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(switchPin, INPUT);

  Serial.begin(9600);

  digitalWrite(greenLed, HIGH);

  serv.write(0);
}

void loop() {

  if (locked == false)
  {
    int switchVal = digitalRead(switchPin);

    if (switchVal == HIGH) {
      locked = true;
      isFirstKnock = true;
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, HIGH);
      serv.write(90);
      delay(1000);

      lastUpdate = millis();
    }
  }
  else
  {
    unsigned long currMil = millis();
    int t = currMil - lastUpdate;

    if (t >= 1000)
    {
      isFirstKnock = true;
    }

    bool kn = checkForKnock();

    if (kn == true)
    {
      if (isFirstKnock == true)
      {
        correctKnocks = 1;

        isFirstKnock = false;

        Serial.println("first");
      }
      else
      {
        if (t >= knocksDelays[correctKnocks] - 150 && t <=  knocksDelays[correctKnocks] + 150)
        {
          correctKnocks++;
          Serial.println(t);
        }
        else
        {
          correctKnocks = 0;
          isFirstKnock = true;
        }
      }

      delay(100);
      lastUpdate = millis();
    }

    if (correctKnocks >= knocksNeeded)
    {
      locked = false;
      digitalWrite(greenLed, HIGH);
      digitalWrite(redLed, LOW);
      correctKnocks = 0;
      serv.write(0);
      delay(1000);
    }
  }
}

bool checkForKnock() {
  int value = analogRead(piezo);

  if (value > quietKnock) {
    digitalWrite(yellowLed, HIGH);
    delay(50);
    digitalWrite(yellowLed, LOW);
    return true;
  }
  else {
    return false;
  }
}
