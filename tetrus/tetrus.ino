#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SPDC A6

#define BTNL A2
#define BTNR 6
#define BTNC 7
#define BTND 3

#define LED 2
#define BUZZ 9

#define GAMERATE 1000

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

const byte tetra[7][17] = {
  { 0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0
  },
  { 0, 0, 1, 0,
    0, 1, 1, 0,
    0, 1, 0, 0,
    0, 0, 0, 0
  },
  { 0, 1, 0, 0,
    0, 1, 1, 0,
    0, 0, 1, 0,
    0, 0, 0, 0
  },
  { 0, 0, 0, 0,
    0, 1, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0
  },
  { 0, 0, 0, 0,
    0, 1, 1, 0,
    0, 1, 0, 0,
    0, 1, 0, 0
  },
  { 0, 0, 0, 0,
    0, 1, 1, 0,
    0, 1, 1, 0,
    0, 0, 0, 0
  },
  { 0, 0, 1, 0,
    0, 1, 1, 0,
    0, 0, 1, 0,
    0, 0, 0, 0
  }
};

const byte boardWidth = 10;
const byte boardHeight = 20;

int btn_prevL = LOW;
int btn_prevR = LOW;
int btn_prevC = LOW;

byte board[200];

unsigned long game_update;

int lines[5];
int linesCount = 0;

bool redraw = false;
bool lock = false;
bool gameOver = false;

int currentPiece = 0;
int currentRotation = 0;
int currentX = boardWidth / 2 - 1;
int currentY = -3;

void setup() {
  pinMode(BTNL, INPUT);
  pinMode(BTNR, INPUT);
  pinMode(BTNC, INPUT);
  pinMode(BTND, INPUT);
  pinMode(SPDC, INPUT);

  pinMode(LED, OUTPUT);
  pinMode(BUZZ, OUTPUT);

  randomSeed(analogRead(A0));

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();

  restart();
}

void loop() {

  while (!gameOver)
  {
    unsigned long currMil = millis();

    readControls();

    if (currMil > game_update)
    {
      if (doesFit(currentPiece, currentRotation, currentX, currentY + 1))
      {
        currentY++;
      }
      else
      {
        lock = true;
      }

      redraw = true;
      game_update += GAMERATE;
    }

    if (lock)
    {
      //lock
      for (int x = 0; x < 4; x++)
      {
        for (int y = 0; y < 4; y++)
        {
          if (tetra[currentPiece][rotate(x, y, currentRotation)] == 1)
          {
            board[(currentY + y) * boardWidth + currentX + x] = 1;
          }
        }
      }

      //check lines
      for (int y = 0; y < boardHeight; y++)
      {
        int countP = 0;

        for (int x = 0; x < boardWidth; x++)
        {
          if (board[y * boardWidth + x] == 1)
          {
            countP++;
          }
        }

        if (countP >= 8)
        {
          lines[linesCount] = y;
          linesCount++;
        }
      }

      //next piece
      currentPiece = random(0, 7);
      currentX = boardWidth / 2 - 1;
      currentY = -3;
      currentRotation = 0;

      //game over check
      if (!doesFit(currentPiece, currentRotation, currentX, currentY))
      {
        gameOver = true;
      }

      lock = false;
    }

    if (redraw)
    {
      display.clearDisplay();
      drawBoard();
      drawPiece();
      display.display();

      redraw = false;
    }
  }

  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);

  restart();
}

void restart()
{
  initBoard();
  currentPiece = 0;
  currentRotation = 0;
  currentX = boardWidth / 2 - 1;
  currentY = -3;
  gameOver = false;
  game_update = millis() + GAMERATE;
}

void readControls()
{
  if (readBtn(BTNL, btn_prevL) && doesFit(currentPiece, currentRotation, currentX - 1, currentY))
  {
    currentX--;
    redraw = true;
  }
  if (readBtn(BTNR, btn_prevR) && doesFit(currentPiece, currentRotation, currentX + 1, currentY))
  {
    currentX++;
    redraw = true;
  }
  if (readBtn(BTNC, btn_prevC) && doesFit(currentPiece, currentRotation + 1, currentX, currentY))
  {
    currentRotation++;
    if (currentRotation >= 4)
    {
      currentRotation = 0;
    }
    redraw = true;
  }
  if (digitalRead(BTND) == HIGH)
  {
    if (doesFit(currentPiece, currentRotation, currentX, currentY + 1))
    {
      currentY++;
      redraw = true;
    }
    else
    {
      lock = true;
    }
  }
}

bool doesFit(int nTetro, int nRotation, int posX, int posY)
{
  for (int px = 0; px < 4; px++)
  {
    for (int py = 0; py < 4; py++)
    {
      int pi = rotate(px, py, nRotation);
      int fi = (posY + py) * boardWidth + posX + px;

      if (posX + px >= 0 && posX + px < boardWidth &&
          posY + py >= 0 && posY + px < boardHeight &&
          tetra[nTetro][pi] == 1 && board[fi] != 0)
      {
        return false;
      }
    }
  }

  return true;
}

int rotate(int px, int py, int r)
{
  switch (r)
  {
    case 0: return py * 4 + px;
    case 1: return 12 + py - (px * 4);
    case 2: return 15 - py * 4 - px;
    case 3: return 3 - py + px * 4;
  }
  return 0;
}

void drawPiece()
{
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (tetra[currentPiece][rotate(x, y, currentRotation)] == 1)
      {
        display.drawRect(8 + (y + currentY) * 6, SCREEN_HEIGHT - 8 - 6 * (x + currentX), 6, 6, SSD1306_WHITE);
      }
    }
  }
}

void drawBoard()
{
  for (int x = 0; x < boardWidth; x++)
  {
    for (int y = 0; y < boardHeight; y++)
    {
      byte cell = board[y * boardWidth + x];

      if (cell == 9 || cell == 1)
      {
        display.fillRect(8 + y * 6, SCREEN_HEIGHT - 8 - 6 * x, 6, 6, SSD1306_WHITE);
      }
    }
  }
}

void initBoard()
{
  for (int x = 0; x < boardWidth; x++)
  {
    for (int y = 0; y < boardHeight; y++)
    {
      board[y * boardWidth + x] = (x == 0 || x == boardWidth - 1 || y == boardHeight - 1) ? 9 : 0;
    }
  }
}

bool readBtn(int btnP, int &btn_prev)
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
