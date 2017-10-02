#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

void setup() {
  Serial.begin(9600);
  
  if(!ss.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("seesaw started");
}

void loop() {
  for(uint8_t i=0; i<255; i++){
    ss.analogWrite(0, i);
    delay(10);
  }
}
