#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>

const int ledIn = 4;
const int ledOut = 6;
const int btnUpd = 3;
const int btnUp = 7;
const int btnDown = 8;

struct RequestModel {
  char request[20] = "GetLog";
  int param = 0;
};

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte addresses[][6] = {"00001", "00002"};

const char logMsg[] = "GetLog";

const int LogSize = 60;
char Log[LogSize][20];

RequestModel model;

RF24 radio(10, 9);

int btn_prevUpd = LOW;
int btn_prevU = LOW;
int btn_prevD = LOW;

int currI = 0;
int lastLogI = 0;

void setup()
{
  pinMode(ledIn, OUTPUT);
  pinMode(ledOut, OUTPUT);

  pinMode(btnUpd, INPUT);
  pinMode(btnUp, INPUT);
  pinMode(btnDown, INPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print(F("DETECTOR"));

  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_LOW);

  updateLog();
  writeLogToScreen(currI);
  writeLogToSerial();
}

void loop()
{
  if (readBtn(btnUpd, btn_prevUpd))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Detection...");
    
    updateLog();
    currI = 0;
    writeLogToScreen(currI);
    writeLogToSerial();
  }

  if (readBtn(btnUp, btn_prevU) && currI > 0)
  {
    writeLogToScreen(--currI);
  }

  if (readBtn(btnDown, btn_prevD) && currI < lastLogI)
  {
    writeLogToScreen(++currI);
  }
}

void writeLogToScreen(int index)
{
  lcd.setCursor(0, 0);

  if (lastLogI <= -1)
  {
    lcd.print(F("  No records!"));
    return;
  }

  lcd.print("  ");
  lcd.print(index + 1);
  lcd.print(" out of ");
  lcd.print(lastLogI + 1);
  lcd.print(":   ");

  lcd.setCursor(1, 1);
  lcd.print(Log[index]);
}

void updateLog()
{
  for (int i = 0; i < LogSize; i++)
  {
    model.param = i;

    digitalWrite(ledOut, HIGH);
    radio.stopListening();
    radio.write(&model, sizeof(model));
    digitalWrite(ledOut, LOW);

    radio.startListening();

    bool gotInfo = false;

    digitalWrite(ledIn, HIGH);
    unsigned long millFrom = millis();
    while (millis() - millFrom < 1000)
    {
      gotInfo = false;

      if (radio.available()) {
        radio.read(&Log[i], sizeof(Log[i]));
        gotInfo = true;
        break;
      }
    }
    digitalWrite(ledIn, LOW);

    if (!gotInfo)
    {
      lastLogI = i - 1;
      break;
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

void writeLogToSerial()
{
  for (int i = 0; i < LogSize; i++)
  {
    if (Log[i][0] != 0)
    {
      Serial.println(Log[i]);
    }
  }
  Serial.println(F("------------"));
}
