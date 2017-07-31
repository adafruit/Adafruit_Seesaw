/*
 * This example shows how to blink multiple pins at once on a seesaw.
 * pin 13 is attached to the LED on the samd11 xplained board
 */

#include "seesaw.h"

Adafruit_seesaw ss;

//blink pins 11, 12, 13
uint32_t mask = ((uint32_t)0b111 << 11);

void setup() {
  Serial.begin(9600);
  
  if(!ss.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("seesaw started");

  ss.pinModeBulk(mask); //set pin modes
}

void loop() {
  ss.gpioSetBulk(mask);   //set pins
  delay(1000);                       // wait for a second
  ss.gpioClrBulk(mask);   //clear pins
  delay(1000);  
}
