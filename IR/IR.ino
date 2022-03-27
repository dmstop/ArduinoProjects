#include <IRremote.h>

const int RECV_PIN = 2;

void setup(){
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
}

void loop(){
  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.command);

      
      
      IrReceiver.resume(); // Enable receiving of the next value
  }
}
