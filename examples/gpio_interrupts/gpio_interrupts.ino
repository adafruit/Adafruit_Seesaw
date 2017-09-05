/*
 * This example shows how set GPIO interrupts on a seesaw.
 */

#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

//pin A8
uint32_t mask = ((uint32_t)0b1 << 8);

void setup() {
  Serial.begin(9600);
  
  if(!ss.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("seesaw started");

  ss.setGPIOInterrupts(mask, 1);
}

void loop() {
  if(!digitalRead(3)){
    Serial.println(ss.digitalRead(9));
  }
}

