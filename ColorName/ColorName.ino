const int S0 = 6;
const int S1 = 7;
const int S2 = 8;
const int S3 = 3;
const int Out = 4;

const int ledR = 9;
const int ledG = 11;
const int ledB = 10;

int redF = 0;
int greenF = 0;
int blueF = 0;

int redC = 0;
int greenC = 0;
int blueC = 0;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(Out, INPUT);

  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(9600);
}

void loop() {
  //red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  redF = pulseIn(Out, LOW);
  redC = constrain(map(redF, 58, 200, 255, 0), 0, 255);
  delay(50);

  //green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  greenF = pulseIn(Out, LOW);
  greenC = constrain(map(greenF, 75, 230, 255, 0), 0, 255);
  delay(50);

  //blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  blueF = pulseIn(Out, LOW);
  blueC = constrain(map(blueF, 70, 200, 255, 0), 0, 255);
  delay(50);

  analogWrite(ledR, map(redC, 0, 255, 0, 50));
  analogWrite(ledG, greenC);
  analogWrite(ledB, map(blueC, 0, 255, 0, 60));

  /*Serial.print("R-");
  Serial.print(redC);
  Serial.print(" G-");
  Serial.print(greenC);
  Serial.print(" B-");
  Serial.print(blueC);

  if (redC > greenC && redC > blueC) {
    Serial.print(" - RED!");
  }
  if (greenC > redC && greenC > blueC) {
    Serial.print(" - GREEN!");
  }
  if (blueC > redC && blueC > greenC) {
    Serial.print(" - BLUE!");
  }
  Serial.println("\n------------");

  delay(500);*/

}
