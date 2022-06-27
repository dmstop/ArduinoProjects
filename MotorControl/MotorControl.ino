#define ONBTN 12
#define DIRBTN 10
#define SPDC A3

#define POWCONTROL 6
#define DIRCONTROL1 2
#define DIRCONTROL2 4

void setup() {
  Serial.begin(9600);
  
  pinMode(ONBTN, INPUT);
  pinMode(DIRBTN, INPUT);
  pinMode(SPDC, INPUT);

  pinMode(POWCONTROL, OUTPUT);
  pinMode(DIRCONTROL1, OUTPUT);
  pinMode(DIRCONTROL2, OUTPUT);

}

void loop() {
  int onBtn = digitalRead(ONBTN);
  int dirBtn = digitalRead(DIRBTN);
  int motorSpeed = analogRead(SPDC);

  analogWrite(POWCONTROL, 200);

  digitalWrite(DIRCONTROL1, HIGH);
  digitalWrite(DIRCONTROL2, LOW);  

  delay(2000);

  digitalWrite(DIRCONTROL1, LOW);
  digitalWrite(DIRCONTROL2, HIGH);

  delay(2000);

}
