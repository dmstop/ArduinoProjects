#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LED 7
#define BTN 3

//UNO

RF24 radio(9, 10); // CE, CSN

const byte addresses[][6] = {"00001", "00002"};

int ledState = LOW;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);

  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);
  radio.stopListening();
  int btnVal = digitalRead(BTN);  
  radio.write(&btnVal, sizeof(btnVal));

  delay(5);
  radio.startListening();
  radio.read(&ledState, sizeof(ledState));
  digitalWrite(LED, ledState);
}
