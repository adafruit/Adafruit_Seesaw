#include "Adafruit_RGBTrellis.h"

/**************************************************************************/
/*! 
    @brief  Class constructor
*/
/**************************************************************************/
Adafruit_RGBTrellis::Adafruit_RGBTrellis() : pixels(RGB_TRELLIS_NUM_KEYS, RGB_TRELLIS_NEOPIX_PIN, NEO_GRB + NEO_KHZ800)
{
    for(int i=0; i<RGB_TRELLIS_NUM_KEYS; i++){
        _callbacks[i] = NULL;
    }
}

/**************************************************************************/
/*! 
    @brief  Begin communication with the RGB trellis.
    @param  addr optional i2c address where the device can be found. Defaults to RGB_TRELLIS_ADDR
    @param  flow optional flow control pin
    @returns true on success, false on error.
*/
/**************************************************************************/
bool Adafruit_RGBTrellis::begin(uint8_t addr, int8_t flow)
{
    bool ret = Adafruit_seesaw::begin(addr, flow);
    if(!ret) return ret;

    ret = pixels.begin(addr, flow);
    enableKeypadInterrupt();

    return ret;
}

/**************************************************************************/
/*! 
    @brief  register a callback function on the passed key.
    @param  key the key number to register the callback on
    @param  cb the callback function that should be called when an event on that key happens
*/
/**************************************************************************/
void Adafruit_RGBTrellis::registerCallback(uint8_t key, TrellisCallback (*cb)(keyEvent))
{
    _callbacks[key] = cb;
}

/**************************************************************************/
/*! 
    @brief  unregister a callback on a given key
    @param  key the key number the callback is currently mapped to.
*/
/**************************************************************************/
void Adafruit_RGBTrellis::unregisterCallback(uint8_t key)
{
    _callbacks[key] = NULL;
}

/**************************************************************************/
/*! 
    @brief  activate or deactivate a given key event
    @param  key the key number to map the event to
    @param  edge the edge sensitivity of the event
    @param  enable pass true to enable the passed event, false to disable it.
*/
/**************************************************************************/
void Adafruit_RGBTrellis::activateKey(uint8_t key, uint8_t edge, bool enable)
{
    setKeypadEvent(RGB_TRELLIS_KEY(key), edge, enable);
}

/**************************************************************************/
/*! 
    @brief  read all events currently stored in the seesaw fifo and call any callbacks.
*/
/**************************************************************************/
void Adafruit_RGBTrellis::read()
{
    uint8_t count = getKeypadCount();
    delayMicroseconds(500);
    if(count > 0){
        keyEvent e[count];
        readKeypad(e, count);
        for(int i=0; i<count; i++){
            //call any callbacks associated with the key
            e[i].bit.NUM = RGB_TRELLIS_SEESAW_KEY(e[i].bit.NUM);
            if(e[i].bit.NUM < RGB_TRELLIS_NUM_KEYS && _callbacks[e[i].bit.NUM] != NULL)
                _callbacks[e[i].bit.NUM](e[i]);
        }
    }
}