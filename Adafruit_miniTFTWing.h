#ifndef _MINI_TFT_WING_H
#define _MINI_TFT_WING_H

#include "Adafruit_seesaw.h"

#define TFTWING_ADDR 0x5E
#define TFTWING_RESET_PIN 8

//TODO: define the rest of the buttons here

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with mini tft wing variant of seesaw helper IC
*/
/**************************************************************************/
class Adafruit_miniTFTWing : public Adafruit_seesaw {
public:
	Adafruit_miniTFTWing() {};
	~Adafruit_miniTFTWing() {};

    bool begin(uint8_t addr = TFTWING_ADDR, int8_t flow=-1);

	void setBacklight(uint16_t value);
	void setBacklightFreq(uint16_t freq);
    void tftReset(bool rst = true);
};

#endif