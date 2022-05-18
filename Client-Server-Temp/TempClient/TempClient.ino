#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define LEDIN 5
#define LEDOUT 6
#define BTN 3

#define PERIOD 10000

const byte addresses[][6] = {"00001", "00002"};

const char conMessage[] = "CheckCon";
const char tempMessage[] = "GetTemp";

ThreeWire myWire(A0, A1, A2);
RtcDS1302<ThreeWire> Rtc(myWire);

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

  Rtc.Begin();

  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_LOW);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  showTime();

  checkCon();
  next_update = millis() + PERIOD;
}

void loop() {
  unsigned long currMil = millis();

  if (readButton() || currMil > next_update)
  {
    getData();
    next_update = currMil + PERIOD;
  }

  showTime();
}

void showTime()
{
  RtcDateTime dt = Rtc.GetDateTime();

  char datestring[20];

  snprintf_P(datestring,
             sizeof(datestring) / sizeof(datestring[0]),
             PSTR("%02u/%02u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );

  lcd.setCursor(1, 1);

  lcd.print(datestring);
}

void checkCon()
{
  while (!is_connected)
  {
    lcd.setCursor(0, 0);
    lcd.print(F(" Connecting...     "));

    radio.stopListening();
    digitalWrite(LEDOUT, HIGH);
    radio.write(&conMessage, sizeof(conMessage));
    digitalWrite(LEDOUT, LOW);

    char returnMessage[32] = "";

    radio.startListening();
    unsigned long millFrom = millis();
    while (millis() - millFrom < 2000)
    {
      showTime();

      if (radio.available()) {
        digitalWrite(LEDIN, HIGH);
        radio.read(&returnMessage, sizeof(returnMessage));
        digitalWrite(LEDIN, LOW);
        is_connected = true;
        lcd.setCursor(0, 0);
        lcd.print(F(" Connected!     "));
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
      delay(1000);
      getData();
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
    lcd.print(F(" T:"));
    lcd.print(model.temp);
    lcd.print(F("c "));
    lcd.print(F("H:"));
    lcd.print(model.hum);
    lcd.print(F("%   "));
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print(F(" Disconnect!    "));
    delay(1000);
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
