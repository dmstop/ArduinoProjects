#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BTN 3
#define JOYX A0
#define JOYY A1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

enum Direction {
  NONE = 'N',
  UP = 'U',
  DOWN = 'D',
  RIGHT = 'R',
  LEFT = 'L'
};

const int mapW = 16;
const int mapH = 16;

const char map1[] = {
  "################"
  "#              #"
  "#          #####"
  "###        #####"
  "#              #"
  "#              #"
  "#         ######"
  "#              #"
  "#              #"
  "#              #"
  "#          #   #"
  "#####      #   #"
  "#          #   #"
  "#   #      #   #"
  "#   #      #####"
  "################"
};

const int sWidth = 32;

const float depth = 8;
const float fov = PI / 3;

void startGame()
{
  float pX = 2;
  float pY = 2;
  float pA = 0;

  long mFrom = millis();

  while (true)
  {
    long mTo = millis();
    int elapsed = mTo - mFrom;
    mFrom = mTo;

    float multi = float(elapsed) / 1000;

    if (readTurn() == LEFT)
    {
      pA -= multi;
    }
    if (readTurn() == RIGHT)
    {
      pA += multi;
    }

    if (readMovement() == UP)
    {
      pX += cos(pA) * multi;
      pY += sin(pA) * multi;

      if (map1[mapW * (int)pY + (int)pX] == '#')
      {
        pX -= cos(pA) * multi;
        pY -= sin(pA) * multi;
      }
    }

    if (readMovement() == DOWN)
    {
      pX -= cos(pA) * multi;
      pY -= sin(pA) * multi;

      if (map1[mapW * (int)pY + (int)pX] == '#')
      {
        pX += cos(pA) * multi;
        pY += sin(pA) * multi;
      }
    }

    if (digitalRead(BTN) == HIGH)
    {
      break;
    }

    display.clearDisplay();

    for (int x = 0; x < sWidth; x++)
    {
      float rayAngle = (pA - fov / 2) + x / (float) sWidth;

      float rX = cos(rayAngle);
      float rY = sin(rayAngle);

      float DtW = 0;
      bool hit = false;

      while (hit == false && DtW < depth)
      {
        DtW += 0.15;

        int tX = pX + rX * DtW;
        int tY = pY + rY * DtW;

        if (tX < 0 || tX >= depth + pX || tY < 0 || tY >= depth + pY)
        {
          hit = true;
          DtW = depth;
        }
        else
        {
          char tC = map1[mapW * tY + tX];

          if (tC == '#')
          {
            hit = true;
            DtW = DtW * cos(rayAngle - pA);
          }
        }
      }
      int cl = SCREEN_HEIGHT / 2.0 - SCREEN_HEIGHT * fov / DtW;

      if (DtW < 3)
      {
        display.fillRect(x * SCREEN_WIDTH / sWidth, cl + 1, SCREEN_WIDTH / sWidth, SCREEN_HEIGHT - cl * 2, SSD1306_WHITE);
      }
      else
      {
        if (DtW < depth) {
          display.drawRect(x * SCREEN_WIDTH / sWidth, cl + 1, SCREEN_WIDTH / sWidth, SCREEN_HEIGHT - cl * 2, SSD1306_WHITE);
        }
      }
    }

    display.display();
    Serial.println(1000 / elapsed);
  }
}

Direction readMovement() {
  int mY = map(analogRead(JOYX), 0, 1023, 10, -10);

  if (mY > 9 ) {
    return UP;
  }
  else if (mY < -9 ) {
    return DOWN;
  }
  return NONE;
}

Direction readTurn() {
  int mX = map(analogRead(JOYY), 0, 1023, 10, -10);

  if (mX > 9 ) {
    return RIGHT;
  }
  else if (mX < -9 ) {
    return LEFT;
  }
  return NONE;
}

void setup() {
  pinMode(BTN, INPUT);
  pinMode(JOYX, INPUT);
  pinMode(JOYY, INPUT);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();
}

void loop() {
  startGame();
}
