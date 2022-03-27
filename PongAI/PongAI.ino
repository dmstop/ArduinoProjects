#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define ControlPin A3
#define ResetPin 2
#define Led1 3
#define Led2 7
#define Ctrl1 A3
#define Ctrl2 A2
#define SoundPin 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

const unsigned long RATE = 40;
const unsigned long AIRATE = 47;

const uint8_t PADDLE_HEIGHT = 8;

const uint8_t player1_x = 12;
uint8_t player1_y = 24;

const uint8_t player2_x = 115;
uint8_t player2_y = 24;

uint8_t ball_x = 64, ball_y = 32;
int8_t ball_dir_x = 1, ball_dir_y = 1;

uint8_t prevScore1 = 0, prevScore2 = 0;
uint8_t score1 = 0, score2 = 0;

const int notes[] = {262, 294, 330, 349};

unsigned long game_update;
unsigned long ai_update;

void setup() {
  pinMode(ControlPin, INPUT);
  pinMode(ResetPin, INPUT);
  pinMode(Ctrl1, INPUT);
  pinMode(Ctrl2, INPUT);

  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(SoundPin, OUTPUT);

  randomSeed(analogRead(0));

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.setTextSize(1);
  display.clearDisplay();
  display.display();

  drawCourt();
  game_update = millis();
  ai_update = game_update;
}

void loop() {
  if (digitalRead(ResetPin) == HIGH)
  {
    reset();
    return;
  }

  bool update = false;

  unsigned long currMil = millis();

  if (currMil > game_update)
  {
    //long res = currMil - game_update;
    //Serial.println(res);

    //ball
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;

    //walls check
    if (new_x - 2 == 0) {
      prevScore2 = score2;
      score2++;

      new_x = 64;
      new_y = 32;

      ball_dir_x = 1;

      int newDir = random(0, 2);
      if (newDir == 0)
      {
        ball_dir_y = -1;
      }
      else
      {
        ball_dir_y = 1;
      }

      digitalWrite(Led1, HIGH);
      tone(SoundPin, notes[0], 15);
      delay(100);
      digitalWrite(Led1, LOW);
    }
    else if (new_x + 2 == 127) {
      prevScore1 = score1;
      score1++;

      new_x = 64;
      new_y = 32;

      ball_dir_x = -1;

      int newDir = random(0, 2);
      if (newDir == 0)
      {
        ball_dir_y = -1;
      }
      else
      {
        ball_dir_y = 1;
      }

      digitalWrite(Led2, HIGH);
      tone(SoundPin, notes[0], 10);
      delay(100);
      digitalWrite(Led2, LOW);
    }

    if (new_y - 2 == 0 || new_y + 2 == 63) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    //player check
    if (new_x == player1_x + 1 && new_y >= player1_y - 1 && new_y <= player1_y + PADDLE_HEIGHT + 1) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }
    if (new_x == player2_x - 1 && new_y >= player2_y - 1 && new_y <= player2_y + PADDLE_HEIGHT + 1) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    display.fillCircle(ball_x, ball_y, 2, BLACK);
    display.fillCircle(new_x, new_y, 2, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    //pads
    display.drawLine(player1_x, player1_y, player1_x, player1_y + PADDLE_HEIGHT, BLACK);
    player1_y = map(analogRead(Ctrl1), 0, 1024, 1, SCREEN_HEIGHT - PADDLE_HEIGHT - 1);
    display.drawLine(player1_x, player1_y, player1_x, player1_y + PADDLE_HEIGHT, WHITE);

    //wins
    if (score1 == 10)
    {
      display.setTextSize(5);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.write(27);
      display.print("win");
      display.display();

      tone(SoundPin, notes[0], 50);
      delay(50);
      tone(SoundPin, notes[1], 50);
      delay(50);
      tone(SoundPin, notes[2], 50);
      delay(50);
      tone(SoundPin, notes[3], 50);
      delay(50);
      noTone(SoundPin);

      digitalWrite(Led1, HIGH);
      delay(1000);
      digitalWrite(Led1, LOW);
      delay(1000);
      digitalWrite(Led1, HIGH);
      delay(1000);
      digitalWrite(Led1, LOW);
      delay(1000);
      digitalWrite(Led1, HIGH);
      delay(1000);
      digitalWrite(Led1, LOW);

      reset();
      return;
    }
    if (score2 == 10)
    {
      display.setTextSize(5);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("win");
      display.write(26);
      display.display();
      tone(SoundPin, notes[0], 50);
      delay(50);
      tone(SoundPin, notes[1], 50);
      delay(50);
      tone(SoundPin, notes[2], 50);
      delay(50);
      tone(SoundPin, notes[3], 50);
      delay(50);
      noTone(SoundPin);

      digitalWrite(Led2, HIGH);
      delay(1000);
      digitalWrite(Led2, LOW);
      delay(1000);
      digitalWrite(Led2, HIGH);
      delay(1000);
      digitalWrite(Led2, LOW);
      delay(1000);
      digitalWrite(Led2, HIGH);
      delay(1000);
      digitalWrite(Led2, LOW);

      reset();
      return;
    }

    //update
    game_update += RATE;
    update = true;
  }

  if (currMil > ai_update)
  {
    if (ball_dir_x > 0 && player2_y + PADDLE_HEIGHT / 2 < ball_y && player2_y + PADDLE_HEIGHT <= SCREEN_HEIGHT - 3)
    {
      display.drawLine(player2_x, player2_y, player2_x, player2_y + PADDLE_HEIGHT, BLACK);
      player2_y++;
      display.drawLine(player2_x, player2_y, player2_x, player2_y + PADDLE_HEIGHT, WHITE);
    }
    else if (ball_dir_x > 0 && player2_y + PADDLE_HEIGHT / 2 > ball_y && player2_y >= 2)
    {
      display.drawLine(player2_x, player2_y, player2_x, player2_y + PADDLE_HEIGHT, BLACK);
      player2_y--;
      display.drawLine(player2_x, player2_y, player2_x, player2_y + PADDLE_HEIGHT, WHITE);
    }
    else
    {
      display.drawLine(player2_x, player2_y, player2_x, player2_y + PADDLE_HEIGHT, WHITE);
    }


    ai_update += AIRATE;
    update = true;
  }


  if (update)
  {
    //center
    display.drawLine(64, 4, 64, 12, SSD1306_WHITE);
    display.drawLine(64, 20, 64, 28, SSD1306_WHITE);
    display.drawLine(64, 36, 64, 44, SSD1306_WHITE);
    display.drawLine(64, 52, 64, 60, SSD1306_WHITE);

    //score
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(52, 5);
    display.print(prevScore1);
    display.setCursor(72, 5);
    display.print(prevScore2);

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(52, 5);
    display.print(score1);
    display.setCursor(72, 5);
    display.print(score2);

    display.display();
  }
}

void reset()
{
  display.setTextSize(0);
  display.clearDisplay();
  ball_x = 64;
  ball_y = 32;
  ball_dir_x = 1;
  ball_dir_y = 1;
  prevScore1 = 0;
  prevScore2 = 0;
  score1 = 0;
  score2 = 0;
  drawCourt();
  game_update = millis();
}

void drawCourt()
{
  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
  display.display();
}
