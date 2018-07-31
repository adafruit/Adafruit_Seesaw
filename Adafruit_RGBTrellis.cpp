#include "Adafruit_RGBTrellis.h"

Adafruit_RGBTrellis::Adafruit_RGBTrellis() : pixels(RGB_TRELLIS_NUM_KEYS, RGB_TRELLIS_NEOPIX_PIN, NEO_GRB + NEO_KHZ800)
{
    for(int i=0; i<RGB_TRELLIS_NUM_KEYS; i++){
        _callbacks[i] = NULL;
    }
}

bool Adafruit_RGBTrellis::begin(uint8_t addr, int8_t flow)
{
    bool ret = Adafruit_seesaw::begin(addr, flow);
    if(!ret) return ret;

    ret = pixels.begin(addr, flow);
    enableKeypadInterrupt();

    return ret;
}

void Adafruit_RGBTrellis::registerCallback(uint8_t key, TrellisCallback (*cb)(keyEvent))
{
    _callbacks[key] = cb;
}

void Adafruit_RGBTrellis::unregisterCallback(uint8_t key)
{
    _callbacks[key] = NULL;
}

void Adafruit_RGBTrellis::activateKey(uint8_t key, uint8_t edge, bool enable)
{
    setKeypadEvent(RGB_TRELLIS_KEY(key), edge, enable);
}

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