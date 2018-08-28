#ifndef _RGB_TRELLIS_H
#define _RGB_TRELLIS_H

#include "Adafruit_seesaw.h"
#include "seesaw_neopixel.h"

#define RGB_TRELLIS_ADDR 0x2E

#define RGB_TRELLIS_NEOPIX_PIN 3

#define RGB_TRELLIS_NUM_ROWS 4
#define RGB_TRELLIS_NUM_COLS 4
#define RGB_TRELLIS_NUM_KEYS (RGB_TRELLIS_NUM_ROWS * RGB_TRELLIS_NUM_COLS)

#define RGB_TRELLIS_MAX_CALLBACKS 32

#define RGB_TRELLIS_KEY(x) (((x)/4)*8 + ((x)%4))
#define RGB_TRELLIS_SEESAW_KEY(x) (((x)/8)*4 + ((x)%8))

#define RGB_TRELLIS_X(k) ((k)%4)
#define RGB_TRELLIS_Y(k) ((k)/4)

typedef void (*TrellisCallback)(keyEvent evt);

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with the seesaw keypad module
*/
/**************************************************************************/
class Adafruit_RGBTrellis : public Adafruit_seesaw {
public:
	Adafruit_RGBTrellis();
	~Adafruit_RGBTrellis() {};

    bool begin(uint8_t addr = RGB_TRELLIS_ADDR, int8_t flow=-1);

    void registerCallback(uint8_t key, TrellisCallback (*cb)(keyEvent));
    void unregisterCallback(uint8_t key);

    void activateKey(uint8_t key, uint8_t edge, bool enable=true);

    void read();

    seesaw_NeoPixel pixels; ///< the onboard neopixel matrix
    
protected:
    TrellisCallback (*_callbacks[RGB_TRELLIS_NUM_KEYS])(keyEvent); ///< the array of callback functions
};

#endif
