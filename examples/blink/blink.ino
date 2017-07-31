/*
 * This example shows how to blink a pin on a seesaw.
 * pin 13 is attached to the LED on the samd11 xplained board
 */

#include "seesaw.h"

Adafruit_seesaw ss;

void setup() {
  Serial.begin(9600);
  
  if(!ss.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("seesaw started");

  ss.pinMode(13, OUTPUT);
}

void loop() {
  ss.digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  ss.digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
}
