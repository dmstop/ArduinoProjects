#include <NewPing.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PI 3.1415926535897932384626433832795

#define BTNPIN 3
#define LEDPIN 2

#define TURNRATE 100
#define TURNRATEGUN 1000

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SERVOPIN 10
#define SERVOPIN2 9

#define TRIGGER_PIN 5
#define ECHO_PIN 7
#define MAX_DISTANCE 60

const int dBtw = 11;
const int aBtw = 5;
const int critD = 40;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Servo servo;
Servo servo2;

unsigned long turnUpdate;
unsigned long gunUpdate;

uint8_t btn_prev = LOW;
bool isWorking = false;

int currentAngle = 20;
bool turnRight = true;

int lineX = 0;
int lineY = 1;
int lineDistance = 2;
short angleToLines[36][3];
int iterator = 0;

void setup() {

  pinMode(BTNPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  drawField();

  servo.attach(SERVOPIN);
  servo.write(currentAngle);

  servo2.attach(SERVOPIN2);
  servo2.write(90);
  delay(1000);

  turnUpdate = millis() + TURNRATE;
  gunUpdate = millis() + TURNRATEGUN;
}

void loop() {
  unsigned long currMil = millis();

  if (readButton() == true)
  {
    isWorking = !isWorking;
    turnUpdate = millis() + TURNRATE;
    gunUpdate = millis() + TURNRATEGUN;
  }

  if (currMil > turnUpdate && isWorking == true)
  {
    turnRadar();
    turnUpdate += TURNRATE;
  }

  if (currMil > gunUpdate && isWorking == true)
  {
    turnGun();
    gunUpdate += TURNRATEGUN;
  }
}

void turnGun()
{
  int radAngle = 0;
  int d = 60;

  for (int i = 0; i < 36; i++)
  {
    if (angleToLines[i][lineDistance] > 0 &&
        angleToLines[i][lineDistance] < d)
    {
      d = angleToLines[i][lineDistance];
      radAngle = 20 + i * 5;
    }
  }

  if (d >= critD)
  {
    return;
  }

  int angle = 180 - radAngle + aBtw;

  float b = sqrt(d * d + dBtw * dBtw - 2 * d * dBtw * cos(angle * PI / 180 ));
  int turnAngle = 180 - aBtw - acos((dBtw * dBtw + b * b - d * d) / (2 * dBtw * b)) * 180 / PI;

  servo2.write(turnAngle);
}

void turnRadar()
{
  servo.write(currentAngle);
  delay(50);

  int distance = sonar.ping_cm();
  if (distance == 0)
  {
    distance = MAX_DISTANCE;
  }

  int distanceOnScreen = map(distance, 0, MAX_DISTANCE, 0, display.height() - 3);

  float rayX = -cos((float) currentAngle / 360 * 2 * PI);
  float rayY = sin((float) currentAngle / 360 * 2 * PI);
  int screenX = display.width() / 2 + rayX * distanceOnScreen;
  int screenY = display.height() - 1 - rayY * distanceOnScreen;

  display.drawLine(display.width() / 2, display.height() - 1, angleToLines[iterator][lineX], angleToLines[iterator][lineY], SSD1306_BLACK);
  display.drawLine(display.width() / 2, display.height() - 1, screenX, screenY, SSD1306_WHITE);
  display.display();

  angleToLines[iterator][lineX] = screenX;
  angleToLines[iterator][lineY] = screenY;
  angleToLines[iterator][lineDistance] = distanceOnScreen;

  if (turnRight == true)
  {
    currentAngle += 5;
    iterator++;

    if (currentAngle > 160)
    {
      iterator -= 2;
      currentAngle = 155;
      turnRight = false;
    }
  }
  else
  {
    currentAngle -= 5;
    iterator--;

    if (currentAngle < 20)
    {
      iterator += 2;
      currentAngle = 25;
      turnRight = true;
    }
  }

  bool isLight = false;

  for (int i = 0; i < 36; i++)
  {
    if (angleToLines[i][lineDistance] < critD && angleToLines[i][lineDistance] > 0)
    {
      isLight = true;
      break;
    }
  }

  if (isLight == true)
  {
    digitalWrite(LEDPIN, HIGH);
  }
  else
  {
    digitalWrite(LEDPIN, LOW);
  }
}

void drawField()
{
  display.clearDisplay();
  display.drawCircle(display.width() / 2, display.height() - 1, display.height() - 1, SSD1306_WHITE);
  display.display();
}

bool readButton()
{
  uint8_t btn = digitalRead(BTNPIN);

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
