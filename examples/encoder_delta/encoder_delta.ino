/*
 * This example helps demonstrates getEncoderDelta().
 * 
 *   position = the current encoder position
 *      delta = the change in position since last read
 *      
 * With this sketch running, open the Serial Monitor. It may also help
 * to turn on "Show timestamp". Then play with turning the encoder between 
 * print outs to see how the values change.
 * 
 * If the encoder does ~not~ move between reads, then:
 *   - position will be the same value
 *   - delta will be zero
 *   
 * NOTE: reading position or delta resets delta to zero
 */
 
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define SEESAW_ADDR          0x36

Adafruit_seesaw ss;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Looking for seesaw!");
  
  if (! ss.begin(SEESAW_ADDR)) {
    Serial.println("Couldn't find seesaw on default address");
    while(1) delay(10);
  }
  Serial.println("seesaw started");

  uint16_t pid;
  uint8_t year, mon, day;

  ss.getProdDatecode(&pid, &year, &mon, &day);
  Serial.print("PID: "); Serial.println(pid);
  Serial.print("Firmware:");
  Serial.print(2000+year); Serial.print("/");
  Serial.print(mon); Serial.print("/");
  Serial.println(day);
}

void loop() {
  int32_t position;
  int32_t delta;

  // get delta and position
  delta = ss.getEncoderDelta();        // also resets delta to zero
  position = ss.getEncoderPosition();  // this also resets delta to zero
                                       // so getEncoderDelta() is called first

  // print info
  Serial.print("position: "); Serial.println(position);
  Serial.print("   delta: "); Serial.println(delta);

  // read once every two seconds
  // this is intentionally slow to allow user to easily turn encoder between reads
  delay(2000);
}
