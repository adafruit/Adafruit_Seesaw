#include "seesaw.h"

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
  Serial.print(ss.analogRead(0));
  Serial.print(",");
  Serial.println(ss.analogRead(1));
  //delay(1000);  
}
