#include "Adafruit_RGBTrellis.h"

Adafruit_RGBTrellis trellis_1, trellis_2;

//define a callback for key presses
TrellisCallback blink_1(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    trellis_1.pixels.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, trellis_1.pixels.numPixels(), 0, 255))); //on rising
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
  // or is the pad released?
    trellis_1.pixels.setPixelColor(evt.bit.NUM, 0); //off falling
  }

  // Turn on/off the neopixels!
  trellis_1.pixels.show();

  return 0;
}

//define a callback for key presses
TrellisCallback blink_2(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    trellis_2.pixels.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, trellis_2.pixels.numPixels(), 0, 255))); //on rising
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
  // or is the pad released?
    trellis_2.pixels.setPixelColor(evt.bit.NUM, 0); //off falling
  }

  // Turn on/off the neopixels!
  trellis_2.pixels.show();

  return 0;
}

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  
  if (!trellis_1.begin(0x2E) || !trellis_2.begin(0x2F)) {
    Serial.println("Could not start trellis, check wiring?");
    while(1);
  } else {
    Serial.println("NeoPixel Trellis started");
  }

  //activate all keys and set callbacks
  for(int i=0; i<RGB_TRELLIS_NUM_KEYS; i++){
    trellis_1.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis_1.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis_1.registerCallback(i, blink_1);
    trellis_2.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis_2.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis_2.registerCallback(i, blink_2);
  }

  //do a little animation to show we're on
  for (uint16_t i=0; i<trellis_1.pixels.numPixels(); i++) {
    trellis_1.pixels.setPixelColor(i, Wheel(map(i, 0, trellis_1.pixels.numPixels(), 0, 255)));
    trellis_1.pixels.show();
    delay(50);
  }
  for (uint16_t i=0; i<trellis_2.pixels.numPixels(); i++) {
    trellis_2.pixels.setPixelColor(i, Wheel(map(i, 0, trellis_2.pixels.numPixels(), 0, 255)));
    trellis_2.pixels.show();
    delay(50);
  }
  for (uint16_t i=0; i<trellis_1.pixels.numPixels(); i++) {
    trellis_1.pixels.setPixelColor(i, 0x000000);
    trellis_1.pixels.show();
    delay(50);
  }
  for (uint16_t i=0; i<trellis_2.pixels.numPixels(); i++) {
    trellis_2.pixels.setPixelColor(i, 0x000000);
    trellis_2.pixels.show();
    delay(50);
  }
}

void loop() {
  trellis_1.read();  // interrupt management does all the work! :)
  trellis_2.read();  // interrupt management does all the work! :)
  
  delay(20); //the trellis has a resolution of around 60hz
}


/******************************************/

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return trellis_1.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return trellis_1.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return trellis_1.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}
