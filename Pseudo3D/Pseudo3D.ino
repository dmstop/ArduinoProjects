#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PI 3.1415926535897932384626433832795

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

const char w1 = 219;

const char mazeMap[8][13] = {
  "############",
  "#..........#",
  "#..........#",
  "#..........#",
  "#......##..#",
  "#..........#",
  "############"
};

const float depth = 8;
const float fov = PI / 3;

float pX = 3;
float pY = 3;
float pA = 0;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  display.clearDisplay();
  display.display();

  view();
}

void view()
{
  display.clearDisplay();

  for (int x = 0; x < SCREEN_WIDTH; x++)
  {
    float rayAngle = (pA - fov / 2) + x / (float) SCREEN_WIDTH;

    float rX = cos(rayAngle);
    float rY = sin(rayAngle);

    float DtW = 0;
    bool hit = false;

    while (hit == false && DtW < depth)
    {
      DtW += 0.1;

      int tX = pX + rX * DtW;
      int tY = pY + rY * DtW;

      if (tX < 0 || tX >= depth + pX || tY < 0 || tY >= depth + pY)
      {
        hit = true;
        DtW = depth;
      }
      else
      {
        char tC = mazeMap[tX][tY];

        if (tC == '#')
        {
          hit = true;
          DtW = DtW * cos(rayAngle - pA);
        }
      }
    }

    int cl = SCREEN_HEIGHT / 2.0 - SCREEN_HEIGHT * fov / DtW;

    display.drawFastVLine(x, cl + 1, SCREEN_HEIGHT - cl * 2, SSD1306_WHITE);
  }

  display.display();
}

void loop() {

  view();
  pA += 0.1;
}
