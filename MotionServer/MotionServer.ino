#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <nRF24L01.h>
#include <RF24.h>

struct RequestModel {
  char request[20];
  int param;
};

const int PIR = 6;
const int CatchLed = 2;
const int LedIn = 3;
const int LedOut = 4;

const int LogSize = 60;
const unsigned int DefPirInterval = 1500;
const unsigned long LongPirInterval = 60000;

const byte addresses[][6] = {"00001", "00002"};
const char reqMess[] = "GetLog";

ThreeWire myWire(A0, A1, A2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
RF24 radio(10, 9);

RequestModel model;

unsigned long pirInterval = DefPirInterval;
unsigned long pirPrevMil = 0;

char timeStr[20];

char Log[LogSize][20];
byte logIndex = 0;

void setup() {
  pinMode(PIR, INPUT);
  pinMode(CatchLed, OUTPUT);
  pinMode(LedIn, OUTPUT);
  pinMode(LedOut, OUTPUT);

  Serial.begin(9600);
  Rtc.Begin();
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  while (!radio.available())
  {
    unsigned long currMil = millis();

    if (currMil - pirPrevMil >= pirInterval)
    {
      pirPrevMil = currMil;
      pirInterval = checkPir() ? LongPirInterval : DefPirInterval;
    }
  }

  digitalWrite(LedIn, HIGH);
  radio.read(&model, sizeof(model));
  digitalWrite(LedIn, LOW);

  if (strcmp(model.request, reqMess) == 0)
  {
    sendLog(model.param);
    radio.startListening();
  }
}

void sendLog(int logI)
{
  if(logI >= logIndex)
  {
    return; 
  }
  
  radio.stopListening();

  digitalWrite(LedOut, HIGH);
  radio.write(&Log[logI], sizeof(Log[logI]));
  digitalWrite(LedOut, LOW);
}

bool checkPir()
{
  if (digitalRead(PIR) == HIGH)
  {
    digitalWrite(CatchLed, HIGH);

    updateTimeStr();

    if (logIndex < LogSize)
    {
      for (int i = 0; i < 20; i++)
      {
        Log[logIndex][i] = timeStr[i];
      }

      logIndex++;
    }
    else
    {
      for (int i = 0; i < LogSize - 1; i++)
      {
        for (int j = 0; j < 20; j++)
        {
          Log[i][j] = Log[i + 1][j];
        }
      }

      for (int i = 0; i < 20; i++)
      {
        Log[LogSize - 1][i] = timeStr[i];
      }
    }

    delay(10);
    digitalWrite(CatchLed, LOW);

    return true;
  }

  return false;
}

void updateTimeStr()
{
  RtcDateTime dt = Rtc.GetDateTime();

  snprintf_P(timeStr,
             sizeof(timeStr) / sizeof(timeStr[0]),
             PSTR("%02u/%02u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
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
