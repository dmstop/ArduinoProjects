const int S0 = 6;
const int S1 = 7;
const int S2 = 8;
const int S3 = 3;
const int Out = 4;

int redF = 0;
int greenF = 0;
int blueF = 0;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(Out, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(9600);
}

void loop() {
  //red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  redF = pulseIn(Out, LOW);

  Serial.print("R = ");
  Serial.print(redF);
  delay(100);

  //green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  greenF = pulseIn(Out, LOW);

  Serial.print(" G = ");
  Serial.print(greenF);
  delay(100);

  //blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  blueF = pulseIn(Out, LOW);

  Serial.print(" B = ");
  Serial.println(blueF);
  delay(1000);


}
