#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"

#define LEDIN 5
#define LEDOUT 6
#define DHTPIN 3
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

RF24 radio(10, 9);

const byte addresses[][6] = {"00001", "00002"};

const char conMessage[] = "GOT";

struct DataModel {
  int hum = 0;
  float temp = 0;
};

DataModel model;

void setup() {
  pinMode(LEDIN, OUTPUT);
  pinMode(LEDOUT, OUTPUT);

  dht.begin();
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  radio.startListening();

  while (!radio.available());

  char request[32] = "";
  digitalWrite(LEDIN, HIGH);
  radio.read(&request, sizeof(request));
  digitalWrite(LEDIN, LOW);

  char * p = strchr(request, ' ');
  if (p)
    *p = 0;

  if (strcmp(request, "GetTemp") == 0)
  {
    sendTemp();
  }

  if (strcmp(request, "CheckCon") == 0)
  {
    checkCon();
  }

  delay(100);
}

void sendTemp()
{
  model.hum = dht.readHumidity();
  model.temp = dht.readTemperature();

  radio.stopListening();
  digitalWrite(LEDOUT, HIGH);
  radio.write(&model, sizeof(DataModel));
  digitalWrite(LEDOUT, LOW);
}

void checkCon()
{ 
  radio.stopListening();
  digitalWrite(LEDOUT, HIGH);
  radio.write(&conMessage, sizeof(conMessage));
  digitalWrite(LEDOUT, LOW);

  digitalWrite(LEDOUT, HIGH);
  digitalWrite(LEDIN, HIGH);
  delay(200 );
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