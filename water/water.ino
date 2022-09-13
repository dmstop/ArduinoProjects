const byte leds[] = {13, 12, 11, 6, 7, 8};
const byte sensor = A3;

const int maxSensVal = 640;

void setup() {
  for (int i = 0; i < 6; i ++)
  {
    pinMode(leds[i], OUTPUT);
  }

  pinMode(sensor, INPUT);

  Serial.begin(9600);
}

void loop() {

  float volt = analogRead(sensor) / 1024.0 * 5.0;
  float temp = (volt  - 0.5 ) * 100;
  
  Serial.println(analogRead(sensor));
  Serial.println(volt);
  Serial.println(temp);
  Serial.println("---------");

  
  /*byte ledLight = map(analogRead(sensor), 0, maxSensVal, 0, 6);

  for (int i = 0; i < ledLight; i++)
    {
    digitalWrite(leds[i], HIGH);
    }
    for (int i = ledLight; i < 6; i++)
    {
    digitalWrite(leds[i], LOW);
    }*/

  delay(500);
}
