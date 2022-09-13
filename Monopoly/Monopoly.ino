#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define LEDG A3
#define LEDR A2

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

const char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {A0, 2, 3, 4};
byte colPins[COLS] = {5, 6, 7, 8};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const byte AnnaKey[] = {0x53, 0x33, 0xFB, 0x12};
const byte DimaKey[] = {0x03, 0x6E, 0xC6, 0x02};

const char dName[] = "Dmitry";
const char aName[] = "Anna";

unsigned long balanceA = 0;
unsigned long balanceD = 0;

void setup() {
  pinMode(LEDG, OUTPUT);
  pinMode(LEDR, OUTPUT);

  Serial.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.clear();
  lcd.noBacklight();

  viewMainScreen();
}

void loop() {

  char aKey = keypad.getKey();
  if (aKey == 'A') {
    bank(1);
  }
  else if (aKey == 'B') {
    bank(2);
  }
  else if (aKey == 'C') {
    bank(3);
  }

  int player = checkCard();
  if (player >= 0)
  {
    viewPlayerInfo(player);
  }
}

void bank(byte operation)
{
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0, 0);
  if (operation == 1)
  {
    lcd.print(F("Pay sum:"));
  }
  else if (operation == 2)
  {
    lcd.print(F("Receive sum:"));
  }
  else if (operation == 3)
  {
    lcd.print(F("Send sum:"));
  }

  lcd.setCursor(0, 1);
  lcd.cursor();

  unsigned long sum = 0;

  while (true)
  {
    char n = keypad.getKey();

    if (n == '0' || n == '1' || n == '2' || n == '3' || n == '4' || n == '5' || n == '6' || n == '7' || n == '8' || n == '9')
    {
      byte num = n - '0';

      if (sum > 0)
      {
        sum = sum * 10 + num;
      }
      else
      {
        sum = num;
      }

      if (sum > 10000)
      {
        sum = 0;
      }

      lcd.setCursor(0, 1);
      lcd.print(F("      "));
      lcd.setCursor(0, 1);
      lcd.print(sum);
      lcd.print('$');
    }
    else if (n == '*')
    {
      break;
    }
    else if (sum > 0)
    {
      int player = checkCard();

      if (player >= 0)
      {
        lcd.noCursor();
        lcd.setCursor(0, 0);

        if (operation == 1)
        {
          if (player == 0)
          {
            if (balanceD < sum)
            {
              lcd.print(dName);
              lcd.print(" no money!");
              digitalWrite(LEDR, HIGH);
              delay(2000);
              digitalWrite(LEDR, LOW);
              break;
            }

            balanceD -= sum;
            lcd.print(dName);
          }
          else if (player == 1)
          {
            if (balanceA < sum)
            {
              lcd.print(aName);
              lcd.print(" no money!");
              digitalWrite(LEDR, HIGH);
              delay(2000);
              digitalWrite(LEDR, LOW);
              break;
            }

            lcd.print(aName);
            balanceA -= sum;
          }

          lcd.print(F(" -       "));
        }
        else if (operation == 2)
        {
          if (player == 0)
          {
            balanceD += sum;
            lcd.print(dName);
          }
          else if (player == 1)
          {
            lcd.print(aName);
            balanceA += sum;
          }

          lcd.print(F(" +       "));
        }
        else if (operation == 3)
        {
          if (player == 0)
          {
            if (balanceD < sum)
            {
              lcd.print(dName);
              lcd.print(" no money!");
              digitalWrite(LEDR, HIGH);
              delay(2000);
              digitalWrite(LEDR, LOW);
              break;
            }

            balanceD -= sum;
            balanceA += sum;
            lcd.print(dName);
            lcd.print(F(" to "));
            lcd.print(aName);
          }
          else if (player == 1)
          {
            if (balanceA < sum)
            {
              lcd.print(aName);
              lcd.print(" no money!");
              digitalWrite(LEDR, HIGH);
              delay(2000);
              digitalWrite(LEDR, LOW);
              break;
            }

            balanceD += sum;
            balanceA -= sum;
            lcd.print(aName);
            lcd.print(F(" to "));
            lcd.print(dName);
          }

          lcd.print(F("        "));
        }

        digitalWrite(LEDG, HIGH);
        delay(2000);
        digitalWrite(LEDG, LOW);
        break;
      }
    }
  }

  lcd.noCursor();
  viewMainScreen();
}

void viewMainScreen()
{
  lcd.noBacklight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("  A-P"));
  lcd.print(F(" B-R"));
  lcd.print(F(" C-S"));
  lcd.setCursor(0, 1);
  lcd.print(F("  *-Cncl"));
}

void viewPlayerInfo(int player)
{
  if (player < 0)
  {
    return;
  }

  lcd.clear();

  if (player == 0)
  {
    lcd.setCursor(5, 0);
    lcd.print(dName);
    lcd.print(F("     "));
    lcd.setCursor(0, 1);
    lcd.print(balanceD);
    lcd.print('$');
  }
  else if (player == 1)
  {
    lcd.clear();

    lcd.setCursor(5, 0);
    lcd.print(aName);
    lcd.print(F("     "));
    lcd.setCursor(0, 1);
    lcd.print(balanceA);
    lcd.print('$');
  }

  lcd.backlight();
  delay(2000);
  viewMainScreen();
}

int checkCard()
{
  if ( !mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial())
  {
    return -1;
  }

  byte key[4];
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    key[i] = mfrc522.uid.uidByte[i];
  }

  bool isDima = true;

  for (byte i = 0; i < 4; i++)
  {
    if (DimaKey[i] != key[i])
    {
      isDima = false;
      break;
    }
  }

  if (isDima)
  {
    return 0;
  }

  bool isAnna = true;

  for (byte i = 0; i < 4; i++)
  {
    if (AnnaKey[i] != key[i])
    {
      isAnna = false;
      break;
    }
  }

  if (isAnna)
  {
    return 1;
  }

  return -1;
}
