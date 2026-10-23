/*
 * This example shows how to blink a pin on a seesaw.
 * Attach the positive (longer lead) of the LED to pin 15 on the seesaw, and
 * the negative lead of the LED to ground through a 1k ohm resistor.
 */

#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

#define BLINK_PIN 15

void setup() {
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // waits for Serial Monitor on native USB boards
  delay(250);

  Serial.print(F("Adafruit seesaw blink test!\n\r"));
  
  if(!ss.begin()){
    Serial.print(F("seesaw not found!\n\r"));
    while(1) delay(10);
  }
  
  uint16_t pid;
  uint8_t year, mon, day;

  ss.getProdDatecode(&pid, &year, &mon, &day);
  Serial.print(F("seesaw found PID: "));
  Serial.print(pid);
  Serial.print(F(" datecode: "));
  Serial.print(2000 + year);
  Serial.print("/");
  Serial.print(mon);
  Serial.print("/");
  Serial.print(day);
  Serial.print(F("\n\r"));

  Serial.print(F("seesaw started OK!\n\r"));

  ss.pinMode(BLINK_PIN, OUTPUT);
}

void loop() {
  ss.digitalWrite(BLINK_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  ss.digitalWrite(BLINK_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);  
}
