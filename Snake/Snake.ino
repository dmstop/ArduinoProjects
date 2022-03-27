#include <LedControl.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

const unsigned int ANIMRATE = 15;

const int irPin = 11;

const int speedControl = A2;

const int resetBtn = 12;

const int soundPin = 9;

const int joyX = A0;
const int joyY = A1;

const int DIN = 2;
const int CS = 3;
const int CLK = 6;

const int notes[] = {
  147, 165, 196, 220, 262, 294, 330, 392
};

LedControl lc = LedControl(DIN, CLK, CS, 0);

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long game_update;
unsigned long anim_update;

bool animOn = true;

enum Direction { UP = 1, DOWN = 2, RIGHT = 3, LEFT = 4 };

int foodX;
int foodY;

int prevHx;
int prevHy;
int headX;
int headY;

int pbx = 0;
int pby = 1;
int bx = 2;
int by = 3;

int body[65][4];

int bodyLength;

Direction oldDir;
Direction currentDir;

int score;

int getRate()
{
  return map(analogRead(speedControl), 0, 1023, 100, 1200);
}

void showSnake()
{
  lc.setLed(0, body[bodyLength - 1][pbx], body[bodyLength - 1][pby], false);

  lc.setLed(0, headX, headY, true);
  for (int i = 0; i < bodyLength; i++)
  {
    lc.setLed(0, body[i][bx], body[i][by], true);
  }
}

void moveSnake()
{
  prevHx = headX;
  prevHy = headY;

  if (currentDir == RIGHT)
  {
    headX++;
  }
  else if (currentDir == LEFT)
  {
    headX--;
  }
  else if (currentDir == UP)
  {
    headY++;
  }
  else if (currentDir == DOWN)
  {
    headY--;
  }

  if (headX == 8)
  {
    headX = 0;
  }
  if (headY == 8)
  {
    headY = 0;
  }
  if (headX == -1)
  {
    headX = 7;
  }
  if (headY == -1)
  {
    headY = 7;
  }

  for (int i = 0; i < bodyLength; i++)
  {
    body[i][pbx] = body[i][bx];
    body[i][pby] = body[i][by];

    if (i == 0)
    {
      body[i][bx] = prevHx;
      body[i][by] = prevHy;
    }
    else
    {
      body[i][bx] = body[i - 1][pbx];
      body[i][by] = body[i - 1][pby];
    }
  }

  if (headX == foodX && headY == foodY)
  {
    body[bodyLength][bx] = body[bodyLength - 1][bx];
    body[bodyLength][by] = body[bodyLength - 1][by];
    body[bodyLength][pbx] = body[bodyLength - 1][pbx];
    body[bodyLength][pby] = body[bodyLength - 1][pby];

    bodyLength++;
    genFood();

    tone(soundPin, notes[5], 10);

    score++;
    showScore();
  }
}

void readMovement()
{
  int mX = map(analogRead(joyY), 0, 1023, 10, -10);
  int mY = map(analogRead(joyX), 0, 1023, 10, -10);

  if (abs(mX) > abs(mY))
  {
    if (mX > 8 && currentDir != LEFT)
    {
      currentDir = RIGHT;
    }
    else if (mX < -8 && currentDir != RIGHT)
    {
      currentDir = LEFT;
    }
  }
  else
  {
    if (mY > 8 && currentDir != DOWN)
    {
      currentDir = UP;
    }
    else if (mY < -8 && currentDir != UP)
    {
      currentDir = DOWN;
    }
  }
}

void readMovementIR()
{
  if (IrReceiver.decode()) {
    int cmd = IrReceiver.decodedIRData.command;

    if (cmd == 8 && currentDir != RIGHT)
    {
      currentDir = LEFT;
    }
    else if (cmd == 90 && currentDir != LEFT)
    {
      currentDir = RIGHT;
    }
    else if (cmd == 24 && currentDir != DOWN)
    {
      currentDir = UP;
    }
    else if (cmd == 82 && currentDir != UP)
    {
      currentDir = DOWN;
    }
    else if (cmd == 22)
    {
      restart();
    }

    IrReceiver.resume();
  }
}

bool checkGameOver()
{
  bool gameOver = false;

  for (int i = 0; i < bodyLength; i++)
  {
    if (body[i][bx] == headX && body[i][by] == headY)
    {
      gameOver = true;
      break;
    }
  }

  if (gameOver)
  {
    lcd.setCursor(3, 0);
    lcd.print("Game over!");
    lcd.setCursor(4, 1);
    lcd.print("Score: ");
    lcd.print(score);
    lcd.print("   ");

    tone(soundPin, notes[5], 100);
    delay(100);
    tone(soundPin, notes[4], 100);
    delay(100);
    tone(soundPin, notes[3], 100);

    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        lc.setLed(0, j, i, true);
        delay(15);
      }
    }

    return true;
  }
  else
  {
    return false;
  }
}

void genFood()
{
  do
  {
    foodX = random(0, 8);
    foodY = random(0, 8);
  }
  while (verFood() == false);

  lc.setLed(0, foodX, foodY, true);
}

bool verFood()
{
  if (headX == foodX && headY == foodY)
  {
    return false;
  }

  for (int i = 0; i < bodyLength; i++)
  {
    if (body[i][bx] == foodX && body[i][by] == foodY)
    {
      return false;
    }
  }

  return true;
}

void showScore()
{
  lcd.setCursor(4, 0);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.print("   ");
}

void restart()
{
  lcd.clear();
  lc.clearDisplay(0);

  headX = 4;
  headY = 3;
  prevHx = headX;
  prevHy = headY;

  bodyLength = 3;

  currentDir = RIGHT;

  score = 0;

  for (int i = 0; i < bodyLength; i++)
  {
    body[i][bx] = headX - i - 1;
    body[i][by] = headY;

    body[i][pbx] = body[i][bx];
    body[i][pby] = body[i][by];
  }

  showSnake();

  genFood();

  showScore();

  game_update = millis() + getRate();
  anim_update = millis() + ANIMRATE;
}

uint8_t btn_prev = LOW;
bool readResBtn()
{
  uint8_t btn = digitalRead(resetBtn);

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

void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(A3));

  IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);

  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);

  pinMode(soundPin, OUTPUT);

  pinMode(speedControl, INPUT);
  pinMode(resetBtn, INPUT);
  pinMode(irPin, INPUT);

  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);

  lc.shutdown(0, false);
  lc.setIntensity(0, 4);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  restart();
}

void loop() {
  unsigned long currMil = millis();

  if (currMil > game_update)
  {
    moveSnake();
    showSnake();

    oldDir = currentDir;
    game_update += getRate();

    if (checkGameOver())
    {
      delay(5000);
      restart();
    }
  }

  if (currMil > anim_update)
  {
    if (animOn == true)
    {
      lc.setLed(0, headX, headY, true);
      lc.setLed(0, foodX, foodY, false);
    }
    else
    {
      lc.setLed(0, headX, headY, false);
      lc.setLed(0, foodX, foodY, true);
    }

    animOn = !animOn;
    anim_update += ANIMRATE;
  }

  if (readResBtn() == true)
  {
    restart();
  }

  if (oldDir == currentDir)
  {
    readMovement();
    readMovementIR();
  }
}
