#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define btnPin 10
#define controlPin A0

uint8_t btn_prev = LOW;
int numLines = 20;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

LiquidCrystal_I2C lcd(0x27,16,2);  

void setup() {
  pinMode(btnPin, INPUT);
  pinMode(controlPin, INPUT);
  
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 

  lcd.init();
  lcd.clear();         
  lcd.backlight();
  
  delay(2000);
  display.clearDisplay();
  display.display();

  updateNumLines();

  drawPic(numLines);
}

void loop() {
  updateNumLines();
  
  bool click = readButton();

  if(!click)
  {
    return;
  }
  
  bool finished = drawPic(numLines);

  while(!finished)
  {
    finished = drawPic(numLines);
  }
}

bool drawPic(int objNum) 
{
  display.clearDisplay();
  for(int i = 0; i<objNum;i++){
    updateNumLines();
    bool read = readButton();

    if(read)
    {
      return false;
    }

    int objType = random(0, 7);

    switch (objType)
    {
      case 0:
      {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);
    
        int x2 = random(0, SCREEN_WIDTH);
        int y2 = random(0, SCREEN_HEIGHT);
    
        display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
        break;
      } 
      case 1:
      {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);

        int width = random(0, SCREEN_HEIGHT);
        int height = random(0, SCREEN_HEIGHT);
        
        display.drawRect(x1,y1, width, height, SSD1306_WHITE);
        break;
      }
      case 2:
      {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);

        int width = random(0, SCREEN_HEIGHT);
        int height = random(0, SCREEN_HEIGHT);
        
        display.fillRect(x1,y1, width, height, SSD1306_INVERSE);
        break;
      }
      case 3:
      {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);

        int rad = random(0, SCREEN_HEIGHT);
        
        display.drawCircle(x1,y1, rad, SSD1306_WHITE);
        break;
      }
      case 4:
      {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);

        int rad = random(0, SCREEN_HEIGHT/2);
        
        display.fillCircle(x1,y1, rad, SSD1306_INVERSE);
        break;
      }
      case 5:
      {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);

        int x2 = random(0, SCREEN_WIDTH);
        int y2 = random(0, SCREEN_HEIGHT);

        int x3 = random(0, SCREEN_WIDTH);
        int y3 = random(0, SCREEN_HEIGHT);

        display.drawTriangle(
        x1, y1,
        x2, y2,
        x3, y3,
        SSD1306_WHITE);
        break;
      }

      case 6:
        {
        int x1 = random(0, SCREEN_WIDTH);
        int y1 = random(0, SCREEN_HEIGHT);

        int x2 = random(0, SCREEN_WIDTH);
        int y2 = random(0, SCREEN_HEIGHT);

        int x3 = random(0, SCREEN_WIDTH);
        int y3 = random(0, SCREEN_HEIGHT);

        display.fillTriangle(
        x1, y1,
        x2, y2,
        x3, y3,
        SSD1306_INVERSE);
        break;
      }
        
      default:        
        break;
    }
    
    
    display.display();
    delay(1);
  }  

  return true;
}

bool readButton()
{
  uint8_t btn = digitalRead(btnPin);

  if(btn == HIGH)
  {
    if(btn_prev == LOW)
    {
      btn_prev = HIGH;
      updateNumLines();
      return true;
    }
  }
  else
  {
    btn_prev = LOW;      
  }
  
  return false;
}  

void updateNumLines()
{
  int newNum = map(analogRead(controlPin), 0, 1000, 1, 20);
  if(newNum != numLines)
  {
    numLines = newNum;
    lcdPrint();
  }  
}

void lcdPrint()
{
  lcd.setCursor(0,0);  
  lcd.print("Intensity:");

  lcd.setCursor(2,1);
  lcd.print("  ");
  lcd.setCursor(2,1);  
  lcd.print(numLines);
}
