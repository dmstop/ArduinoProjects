#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LEDIN 5
#define LEDOUT 6
#define BTN 3

#define PERIOD 10000

const byte addresses[][6] = {"00001", "00002"};

const char conMessage[] = "CheckCon";
const char tempMessage[] = "GetTemp";

LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(9, 10);

uint8_t btn_prev = LOW;
bool is_connected = false;

struct DataModel {
  int hum = 0;
  float temp = 0;
};

DataModel model;

unsigned long next_update;

void setup() {
  Serial.begin(9600);

  pinMode(LEDIN, OUTPUT);
  pinMode(LEDOUT, OUTPUT);
  pinMode(BTN, INPUT);

  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_MIN);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  checkCon();
  delay(1000);
  getData();

  next_update = millis() + PERIOD;
}

void loop() {
  unsigned long currMil = millis();

  if (readButton() || currMil > next_update)
  {
    getData();
    next_update = currMil + PERIOD;
  }
}

void checkCon()
{
  while (!is_connected)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Connecting..."));

    radio.stopListening();
    digitalWrite(LEDOUT, HIGH);
    radio.write(&conMessage, sizeof(conMessage));
    digitalWrite(LEDOUT, LOW);

    char returnMessage[32] = "";

    radio.startListening();
    unsigned long millFrom = millis();
    while (millis() - millFrom < 2000)
    {
      if (radio.available()) {
        digitalWrite(LEDIN, HIGH);
        radio.read(&returnMessage, sizeof(returnMessage));
        digitalWrite(LEDIN, LOW);
        is_connected = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("Connected!"));
        break;
      }
      else
      {
        is_connected = false;
      }
    }

    if (is_connected && strcmp(returnMessage, "Welcome") == 0)
    {
      digitalWrite(LEDOUT, HIGH);
      digitalWrite(LEDIN, HIGH);
      delay(200);
      digitalWrite(LEDOUT, LOW);
      digitalWrite(LEDIN, LOW);
      delay(200);
      digitalWrite(LEDOUT, HIGH);
      digitalWrite(LEDIN, HIGH);
      delay(200);
      digitalWrite(LEDOUT, LOW);
      digitalWrite(LEDIN, LOW);
      delay(200);
      digitalWrite(LEDOUT, HIGH);
      digitalWrite(LEDIN, HIGH);
      delay(200);
      digitalWrite(LEDOUT, LOW);
      digitalWrite(LEDIN, LOW);
    }
    else
    {
      is_connected = false;
    }
  }
}

void getData()
{
  radio.stopListening();
  digitalWrite(LEDOUT, HIGH);
  radio.write(&tempMessage, sizeof(tempMessage));
  digitalWrite(LEDOUT, LOW);

  radio.startListening();
  unsigned long millFrom = millis();
  while (millis() - millFrom < 2000)
  {
    if (radio.available()) {
      digitalWrite(LEDIN, HIGH);
      radio.read(&model, sizeof(DataModel));
      digitalWrite(LEDIN, LOW);
      is_connected = true;
      break;
    }
    else
    {
      is_connected = false;
    }
  }

  if (is_connected)
  {
    lcd.setCursor(0, 0);
    lcd.print(F("Temp: "));
    lcd.print(model.temp);
    lcd.print(F("c   "));

    lcd.setCursor(0, 1);
    lcd.print(F("Hum: "));
    lcd.print(model.hum);
    lcd.print(F("%   "));
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Disconnect!"));
    delay(2000);
    checkCon();    
  }


  delay(100);
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
