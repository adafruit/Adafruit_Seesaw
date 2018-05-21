#include "adafruit_crickit.h"

Adafruit_Crickit crickit;

void setup() {
  Serial.begin(9600);
  
  if(!crickit.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("seesaw started");

  //set the PWM freq for all the servo pins
  crickit.setPWMFreq(CRICKIT_S1, 50);
}

void loop() {

  //set some PWMS
  crickit.analogWrite(CRICKIT_S1, 10000);
  crickit.analogWrite(CRICKIT_S2, 5000);
  crickit.analogWrite(CRICKIT_S3, 20000);
  crickit.analogWrite(CRICKIT_S4, 45000);

  // read an ADC
  Serial.print(crickit.analogRead(CRICKIT_A3));
  Serial.print(",");

  // read a captouch
  Serial.println(crickit.touchRead(CRICKIT_CT2));
  
  delay(1);  
}
