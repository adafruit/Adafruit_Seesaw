#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "Adafruit_miniTFTWing.h"

Adafruit_miniTFTWing ss;

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     5
#define TFT_RST    -1  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to -1!

#define TFT_DC     6

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup()   {
  //while (!Serial);
  Serial.begin(115200);

  if(!ss.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else{
    Serial.println("seesaw started");
    Serial.print("version: ");
    Serial.println(ss.getVersion(), HEX);

    ss.tftReset();
    ss.setBacklight(0x00);
  }

  Serial.println(F("OLED TIME"));

  tft.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  Serial.println("Initialized");
  tft.setRotation(1);

  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
}

void loop() {
  
  uint32_t buttons = ss.readButtons();
  //Serial.println(buttons, BIN);
  
  if(! (buttons & TFTWING_BUTTON_LEFT)){
    tft.fillTriangle(120, 30, 120, 50, 110, 40, ST7735_WHITE);
    Serial.println(F("LEFT"));
  }
  else {
    tft.fillTriangle(120, 30, 120, 50, 110, 40, ST7735_BLACK);
  }
  
  if(! (buttons & TFTWING_BUTTON_RIGHT)){
    tft.fillTriangle(150, 30, 150, 50, 160, 40, ST7735_WHITE);
    Serial.println(F("RIGHT"));
  }
  else {
    tft.fillTriangle(150, 30, 150, 50, 160, 40, ST7735_BLACK);
  }
  
  if(! (buttons & TFTWING_BUTTON_DOWN)){
    tft.fillTriangle(125, 26, 145, 26, 135, 16, ST7735_WHITE);
    Serial.println(F("DOWN"));
  }
  else {
    tft.fillTriangle(125, 26, 145, 26, 135, 16, ST7735_BLACK);
  }
  
  if(! (buttons & TFTWING_BUTTON_UP)){
    tft.fillTriangle(125, 53, 145, 53, 135, 63, ST7735_WHITE);
    Serial.println(F("UP"));
  }
  else {
    tft.fillTriangle(125, 53, 145, 53, 135, 63, ST7735_BLACK);
  }
  
  if (! (buttons & TFTWING_BUTTON_A)){
    tft.fillCircle(30, 18, 10, ST7735_GREEN);
    Serial.println("A");
  }
  else tft.fillCircle(30, 18, 10, ST7735_BLACK);
  
  if (! (buttons & TFTWING_BUTTON_B)) {
    tft.fillCircle(30, 57, 10, ST7735_YELLOW);
    Serial.println("B");
  }
  else tft.fillCircle(30, 57, 10, ST7735_BLACK);
  
  if (! (buttons & TFTWING_BUTTON_IN)) {
    tft.fillCircle(80, 40, 7, ST7735_WHITE);
    Serial.println(F("IN"));
  }
  else tft.fillCircle(80, 40, 7, ST7735_BLACK);
    
  delay(10);
}
