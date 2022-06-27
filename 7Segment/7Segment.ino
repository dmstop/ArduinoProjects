const int dataPin = 4;
const int latchPin = 7;
const int clockPin = 8;

const int btnP = 13;
const int sound = 10;
const int controlPin = A1;

const int interval = 1000;

uint8_t btn_prev = LOW;

unsigned long countUpdate;

void setup()
{
  Serial.begin(9600);

  pinMode(controlPin, INPUT);
  pinMode(btnP, INPUT);

  pinMode(sound, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop() {

  int v = map(analogRead(controlPin), 0 , 1023, 0 , 10);
  displayNum(v, false);

  if (readButton())
  {
    startCounter(v);
  }
}

void startCounter(int t)
{
  countUpdate = millis() + interval;

  while (!readButton())
  {
    unsigned long currM = millis();

    if (currM > countUpdate || t == 0)
    {
      t--;
      displayNum(t, false);

      if (t <= 0)
      {
        for (int i = 0; i < 10; i++)
        {
          displayNum(0, true);
          tone(sound, 150, 20);
          delay(100);
          displayNum(0, false);
          noTone(sound);
          delay(100);
        }

        break;
      }

      countUpdate += interval;
    }
  }
}

bool readButton()
{
  uint8_t btn = digitalRead(btnP);

  if (btn == HIGH)
  {
    if (btn_prev == LOW)
    {
      btn_prev = HIGH;
      return true;
    }
  }
  else
  {
    btn_prev = LOW;
  }

  return false;
}

void displayNum(byte num, bool dot)
{
  byte bits = numToBits(num);
  if (dot) {
    bits = bits | B00000001;
  }

  updateDisplay(bits);
}

void updateDisplay(byte eightBits) {
  eightBits = eightBits ^ B11111111;
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, eightBits);
  digitalWrite(latchPin, HIGH);
}

byte numToBits(int someNumber) {
  switch (someNumber) {
    case 20:
      return B00000000;
      break;
    case 0:
      return B11111100;
      break;
    case 1:
      return B01100000;
      break;
    case 2:
      return B11011010;
      break;
    case 3:
      return B11110010;
      break;
    case 4:
      return B01100110;
      break;
    case 5:
      return B10110110;
      break;
    case 6:
      return B10111110;
      break;
    case 7:
      return B11100000;
      break;
    case 8:
      return B11111110;
      break;
    case 9:
      return B11110110;
      break;
    case 10:
      return B11101110; // Hexidecimal A
      break;
    case 11:
      return B00111110; // Hexidecimal B
      break;
    case 12:
      return B10011100; // Hexidecimal C or use for Centigrade
      break;
    case 13:
      return B01111010; // Hexidecimal D
      break;
    case 14:
      return B10011110; // Hexidecimal E
      break;
    case 15:
      return B10001110; // Hexidecimal F or use for Fahrenheit
      break;
    default:
      return B10010010; // Error condition, displays three vertical bars
      break;
  }
}
