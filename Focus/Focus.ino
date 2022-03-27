#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET     4 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int red_light_pin= 9;
const int green_light_pin = 10;
const int blue_light_pin = 11;

const int yesPin = 5;
const int noPin = 2;

const int buzzPin = 6;

void setup() {
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  pinMode(buzzPin, OUTPUT);

  pinMode(yesPin, INPUT);
  pinMode(noPin, INPUT);

  randomSeed(analogRead(A0));
  
  Serial.begin(9600);
 
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  delay(1000);  
  display.clearDisplay();
  display.display();
  delay(1000);
}

struct Card
{  
  char val;
  int16_t mast;
};

const char CardValues[9]= { '6', '7', '8', '9', 't', 'J', 'Q', 'K', 'A' };
const int16_t CardMasts[] = { 3, 4, 5, 6 }; //chirva,bubna,trefa,pika

Card cards[21];

void loop() {
  display.clearDisplay();  
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.display();
  
  RGB_color(200, 200, 0);

  tone(buzzPin, 200, 200);
  delay(200);
  tone(buzzPin, 500, 400);
  
  digitalWrite(buzzPin, LOW);
  
  display.clearDisplay();  
  display.setTextColor(SSD1306_WHITE);
  display.cp437(true);
  display.setTextSize(1);
  display.setCursor(0, 0);  

  initCards();

  for(int i =0; i < 21;i++)
  {
    if(i % 5 == 0)
    {
      display.println();
    }
    display.write(cards[i].val);
    display.write(cards[i].mast);
    display.print(' '); 
  }
  display.display();
  
  while(digitalRead(yesPin)== LOW && digitalRead(noPin)== LOW )
  {
    delay(100);
  }
  delay(500);

  for(int i = 0; i < 3; i++)
  {
    if(i == 0)
    {
      RGB_color(255, 0, 0);      
    }
    else if(i == 1)
    {
      RGB_color(0, 0, 255);
    }
    else if(i == 2)
    {
      RGB_color(0, 255, 0);
    }    
    
    Card f[7];
    Card s[7];
    Card t[7];

    int pointer = 0;
    for (int j = 0; j < 21; j += 3)
    {
        f[pointer] = cards[j];
        s[pointer] = cards[j + 1];
        t[pointer] = cards[j + 2];

        pointer++;
    }

    tone(buzzPin, 300, 200);

    display.setTextSize(2);
    
    display.clearDisplay();    
    display.setCursor(0, 0); 
    
    for(int j = 0; j < 7; j++)
    {
      if(j > 1 && j % 3 == 0)
      {
        display.println();
      }
      display.print(f[j].val);
      display.write(f[j].mast);
      display.print(' '); 
    }
    display.display();

    bool decision;
    while(true)
    {
      if(digitalRead(yesPin)== HIGH)
      {
        decision = true;
        break;
      }
      if(digitalRead(noPin)== HIGH)
      {
        decision = false;
        break;
      }

      delay(100);
    }
    delay(500);

    if(decision == true)
    {
      for(int j = 0;j<7;j++)
      {
        cards[j]=s[j];
        cards[j+7]=f[j];
        cards[j+14]=t[j];
      }
      continue;
    }

    tone(buzzPin, 300, 200);

    display.clearDisplay();    
    display.setCursor(0, 0); 
    
    for(int j = 0; j < 7; j++)
    {
      if(j > 1 && j % 3 == 0)
      {
        display.println();
      }
      display.print(s[j].val);
      display.write(s[j].mast);
      display.print(' '); 
    }
    display.display();

    decision = false;
    while(true)
    {
      if(digitalRead(yesPin)== HIGH)
      {
        decision = true;
        break;
      }
      if(digitalRead(noPin)== HIGH)
      {
        decision = false;
        break;
      }

      delay(100);
    }
    delay(500);

    if(decision == true)
    {
      for(int j = 0;j<7;j++)
      {
        cards[j]=f[j];
        cards[j+7]=s[j];
        cards[j+14]=t[j];
      }
      continue;
    }
    
    for(int j = 0;j<7;j++)
    {
      cards[j]=f[j];
      cards[j+7]=t[j];
      cards[j+14]=s[j];
    }    
  }

  display.setTextSize(5);
  display.clearDisplay();    
  display.setCursor(0, 0); 

  display.print(cards[10].val);
  display.write(cards[10].mast);
  display.display();

  RGB_color(255, 255, 255);
  tone(buzzPin, 523, 200);
  delay(200);
  tone(buzzPin, 659, 200);
  delay(300);
  tone(buzzPin, 784, 200);

  delay(5000);  
}

void initCards()
{ 
  for(int i = 0;i<21;i++)
  {    
    bool isUniq;
    Card card;
    
    do
    {
      card.val = CardValues[random(0, 9)];
      card.mast = CardMasts[random(0, 4)];

      for(int j = 0;j<21;j++)
      {
        if(cards[j].val == card.val && cards[j].mast == card.mast)
        {
          isUniq = false;
          break;
        }
        else{
          isUniq = true;
        }
      }
    }
    while(!isUniq);

    cards[i] = card;    
  }  
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
