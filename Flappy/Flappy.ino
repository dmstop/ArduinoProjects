#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BIRD_HEIGHT   16
#define BIRD_WIDTH    16

#define BTN 3
#define LED 5
#define BUZZ 8

const int notes[] = {262, 294, 330, 349};

static const unsigned char bird_bmp[] PROGMEM = {
  0x00, 0x7e, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xf0, 0x00, 0xfc, 0x81, 0xe0, 0xc7, 0xf8,
  0xef, 0xe8, 0xff, 0xe0, 0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0d, 0x80, 0x08, 0x80, 0x0c, 0xc0
};

static const unsigned char bird2_bmp[] PROGMEM = {
  0x00, 0x7e, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xf0, 0x00, 0xfc, 0x81, 0xe0, 0xc7, 0xf8,
  0xef, 0xe8, 0xff, 0xe0, 0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0c, 0xc0, 0x08, 0x00, 0x0c, 0x00
};

static const unsigned char bird3_bmp[] PROGMEM = {
  0x00, 0x7e, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xf0, 0x00, 0xfc, 0x81, 0xe0, 0xc7, 0xf8,
  0xef, 0xe8, 0xff, 0xe0, 0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0d, 0x80, 0x04, 0x80, 0x00, 0xc0
};

static const unsigned char birdJump_bmp[] PROGMEM = {
  0x00, 0x7e, 0x00, 0xdf, 0x00, 0xdf, 0x00, 0xff, 0x00, 0xff, 0x00, 0xfc, 0x81, 0xe0, 0xc7, 0xf8,
  0xef, 0xe8, 0xff, 0xe0, 0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0d, 0x80, 0x08, 0x80, 0x0c, 0xc0
};

static const unsigned char sun_bmp[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x30, 0x00, 0x01, 0x32, 0x00, 0x11,
  0xb6, 0x20, 0x0c, 0x84, 0xc0, 0x06, 0x79, 0x80, 0x02, 0xfd, 0x00, 0x1d, 0xfe, 0xe0, 0x0d, 0xfe,
  0xc0, 0x03, 0xff, 0x00, 0x0b, 0xff, 0xc0, 0x7d, 0xfe, 0xf0, 0x01, 0xfe, 0x00, 0x06, 0xfd, 0x80,
  0x0c, 0x30, 0xc0, 0x01, 0x86, 0x00, 0x01, 0x32, 0x00, 0x03, 0x33, 0x00, 0x02, 0x01, 0x00, 0x00,
  0x00, 0x00
};

static const unsigned char cloud_bmp[] PROGMEM = {
  0x00, 0x07, 0xf0, 0x00, 0x00, 0x0c, 0x18, 0x00, 0x00, 0x18, 0x0c, 0x00, 0x00, 0x10, 0x0f, 0x00,
  0x00, 0x70, 0x0b, 0xe0, 0x0f, 0xc0, 0x00, 0x38, 0x18, 0x00, 0x00, 0x0c, 0x10, 0x00, 0x00, 0x06,
  0xd3, 0xff, 0xff, 0xfe
};

struct obstacle {
  int h = 0;
  int w = 8;
  float x = SCREEN_WIDTH - w / 2;
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

uint8_t btn_prev = LOW;

unsigned long lastMil;

int cloud_x = SCREEN_WIDTH - 1;
int cloud_y = 9;
int cloud_speed = 25;


int o_speed = 55;
float currY = SCREEN_HEIGHT - BIRD_HEIGHT - 1;
float vy = 0;
bool inAir = false;
bool lose = false;

byte animCounter = 0;
byte currAnim = 0;

int score = 0;

obstacle obs1;
obstacle obs2;

void setup() {
  pinMode(BTN, INPUT);
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

  obs2.x = SCREEN_WIDTH / 2;

  lastMil = millis();
}

void loop() {
  unsigned long currMil = millis();
  int diff = currMil - lastMil;
  lastMil = currMil;

  if (readButton() == true && inAir == false)
  {
    vy -= 8;
    inAir = true;
    tone(BUZZ, notes[3], 50);
  }

  vy += 19.6 * diff / 1000;
  currY += vy;

  if (currY >= SCREEN_HEIGHT - 1 - BIRD_HEIGHT)
  {
    currY = SCREEN_HEIGHT - 1 - BIRD_HEIGHT;
    vy = 0;
    inAir = false;
  }
  if (currY <= 0)
  {
    currY = 0;
    vy = 0;
  }

  obs1.x -= o_speed * diff / 1000;
  obs2.x -= o_speed * diff / 1000;

  if (obs1.x + obs1.w / 2 <= 0)
  {
    obs1.x = SCREEN_WIDTH + obs1.w / 2;
    obs1.h = (SCREEN_HEIGHT / 2 - 10) * (float)rand() / RAND_MAX;
    score++;
  }
  if (obs2.x + obs2.w / 2 <= 0)
  {
    obs2.x = SCREEN_WIDTH + obs2.w / 2;
    obs2.h = (SCREEN_HEIGHT / 2 - 10) * (float)rand() / RAND_MAX;
    score++;
  }

  if (obs1.x - obs1.w / 2 < 20 + BIRD_WIDTH - 3 && obs1.x + obs1.w / 2 > 22)
  {
    bool collision = currY + BIRD_HEIGHT > SCREEN_HEIGHT - obs1.h;

    if (collision == true)
    {
      lose = true;
    }
  }
  if (obs2.x - obs2.w / 2 < 20 + BIRD_WIDTH - 3 && obs2.x + obs2.w / 2 > 22)
  {
    bool collision = currY + BIRD_HEIGHT > SCREEN_HEIGHT - obs2.h;

    if (collision == true)
    {
      lose = true;
    }
  }

  cloud_x  -= cloud_speed * diff / 1000;
  if ((cloud_x + 31) < 0)
  {
    cloud_x = SCREEN_WIDTH - 1;
    cloud_y = random(5, 25);
    cloud_speed = random(25, 35);
  }

  display.clearDisplay();
  display.drawLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, SSD1306_WHITE);

  display.drawBitmap(
    90,
    5,
    sun_bmp, 20, 20, 1);

  display.drawBitmap(
    cloud_x,
    cloud_y,
    cloud_bmp, 31, 9, 1);

  if (inAir == true || lose == true)
  {
    display.drawBitmap(
      20,
      currY,
      birdJump_bmp, BIRD_WIDTH, BIRD_HEIGHT, 1);
  }
  else if (currAnim == 0)
  {
    display.drawBitmap(
      20,
      currY,
      bird_bmp, BIRD_WIDTH, BIRD_HEIGHT, 1);
  }
  else if (currAnim == 1)
  {
    display.drawBitmap(
      20,
      currY,
      bird2_bmp, BIRD_WIDTH, BIRD_HEIGHT, 1);
  }
  else if (currAnim == 2)
  {
    display.drawBitmap(
      20,
      currY,
      bird3_bmp, BIRD_WIDTH, BIRD_HEIGHT, 1);
  }

  display.fillRect(obs1.x - obs1.w / 2 + 2, SCREEN_HEIGHT - 1 - obs1.h, obs1.w - 4, obs1.h, SSD1306_WHITE);
  display.fillRect(obs1.x - obs1.w / 2, SCREEN_HEIGHT - 1 - obs1.h, obs1.w, 4, SSD1306_WHITE);
  display.fillRect(obs2.x - obs2.w / 2 + 2, SCREEN_HEIGHT - 1 - obs2.h, obs2.w - 4, obs2.h, SSD1306_WHITE);
  display.fillRect(obs2.x - obs2.w / 2, SCREEN_HEIGHT - 1 - obs2.h, obs2.w, 4, SSD1306_WHITE);

  display.setCursor(58, 5);
  display.print(score);

  display.display();

  animCounter++;
  if (animCounter > 1)
  {
    currAnim++;
    animCounter = 0;
  }
  if (currAnim > 2)
  {
    currAnim = 0;
  }

  if (score == 10 && o_speed != 65)
  {
    o_speed = 65;
    tone(BUZZ, notes[0], 50);
  }
  else if (score == 20 && o_speed != 75)
  {
    o_speed = 75;
    tone(BUZZ, notes[0], 50);
  }
  else if (score == 30 && o_speed != 85)
  {
    o_speed = 85;
    tone(BUZZ, notes[0], 50);
  }
  else if (score == 40 && o_speed != 95)
  {
    o_speed = 95;
    tone(BUZZ, notes[0], 50);
  }
  else if (score == 50 && o_speed != 105)
  {
    o_speed = 105;
    tone(BUZZ, notes[0], 50);
  }
  else if (score == 60 && o_speed != 115)
  {
    o_speed = 115;
    tone(BUZZ, notes[0], 50);
  }
  else if (score == 70 && o_speed != 125)
  {
    o_speed = 125;
    tone(BUZZ, notes[0], 50);
  }

  if (lose == true)
  {
    digitalWrite(LED, HIGH);
    tone(BUZZ, notes[2], 50);
    delay(50);
    tone(BUZZ, notes[1], 50);
    delay(50);
    tone(BUZZ, notes[0], 50);
    delay(1900);
    noTone(BUZZ);
    digitalWrite(LED, LOW);

    lose = false;
    cloud_x = SCREEN_WIDTH - 1;
    obs1.h = 0;
    obs2.h = 0;
    obs1.x = SCREEN_WIDTH - obs1.w / 2;
    obs2.x = SCREEN_WIDTH / 2;
    currY = SCREEN_HEIGHT - BIRD_HEIGHT - 1;
    vy = 0;
    score = 0;
    o_speed = 55;

    lastMil = millis();
  }

  Serial.println(1000 / diff);
}

bool readButton()
{
  uint8_t btn = digitalRead(BTN);

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
