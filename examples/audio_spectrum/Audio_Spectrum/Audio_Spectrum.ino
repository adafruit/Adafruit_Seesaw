/*
  Audio Spectrum.

  This example shows how to set the audio sampling rate and read
  audio spectrum data.
*/

#include <seesaw_spectrum.h>

seesaw_Audio_Spectrum ss;

// The setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // wait until serial port is opened
  Serial.println("A");
  
  if(!ss.begin()){
    Serial.println("seesaw not found!");
    while(1) delay(10);
  }
  Serial.println("B");

  ss.setRate(12); // Configure audio sampling rate
}

// The loop routine runs over and over again forever:
void loop() {
  ss.getData(); // Pull audio spectrum data from device
  Serial.println(ss.getLevel(10));
}
