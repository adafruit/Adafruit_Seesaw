#include "Adafruit_NeoTrellis.h"

/**************************************************************************/
/*! 
    @brief  Class constructor
*/
/**************************************************************************/
Adafruit_NeoTrellis::Adafruit_NeoTrellis(uint8_t addr) : pixels(NEO_TRELLIS_NUM_KEYS, NEO_TRELLIS_NEOPIX_PIN, NEO_GRB + NEO_KHZ800)
{
    for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
        _callbacks[i] = NULL;
    }
    this->_addr = addr;
}

/**************************************************************************/
/*! 
    @brief  Begin communication with the RGB trellis.
    @param  addr optional i2c address where the device can be found. Defaults to NEO_TRELLIS_ADDR
    @param  flow optional flow control pin
    @returns true on success, false on error.
*/
/**************************************************************************/
bool Adafruit_NeoTrellis::begin(uint8_t addr, int8_t flow)
{
    _addr = addr;
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
void Adafruit_NeoTrellis::registerCallback(uint8_t key, TrellisCallback (*cb)(keyEvent))
{
    _callbacks[key] = cb;
}

/**************************************************************************/
/*! 
    @brief  unregister a callback on a given key
    @param  key the key number the callback is currently mapped to.
*/
/**************************************************************************/
void Adafruit_NeoTrellis::unregisterCallback(uint8_t key)
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
void Adafruit_NeoTrellis::activateKey(uint8_t key, uint8_t edge, bool enable)
{
    setKeypadEvent(NEO_TRELLIS_KEY(key), edge, enable);
}

/**************************************************************************/
/*! 
    @brief  read all events currently stored in the seesaw fifo and call any callbacks.
*/
/**************************************************************************/
void Adafruit_NeoTrellis::read()
{
    uint8_t count = getKeypadCount();
    delayMicroseconds(500);
    if(count > 0){
        keyEventRaw e[count];
        readKeypad(e, count);
        for(int i=0; i<count; i++){
            //call any callbacks associated with the key
            e[i].bit.NUM = NEO_TRELLIS_SEESAW_KEY(e[i].bit.NUM);
            if(e[i].bit.NUM < NEO_TRELLIS_NUM_KEYS && _callbacks[e[i].bit.NUM] != NULL){
                keyEvent evt = { e[i].bit.EDGE, e[i].bit.NUM };
                _callbacks[e[i].bit.NUM](evt);
            }
        }
    }
}


Adafruit_MultiTrellis::Adafruit_MultiTrellis(Adafruit_NeoTrellis *trelli, uint8_t rows, uint8_t cols)
{
    this->_rows = rows;
    this->_cols = cols;
    this->_trelli = trelli;
}

bool Adafruit_MultiTrellis::begin()
{
    Adafruit_NeoTrellis *t;
    for(int n=0; n<_rows; n++){
        for(int m=0; m<_cols; m++){
            t = (_trelli+n*_cols) + m;
            if(!t->begin(t->_addr))
                return false;
        }
    }

    return true;
}

void Adafruit_MultiTrellis::registerCallback(uint8_t x, uint8_t y, TrellisCallback (*cb)(keyEvent))
{
    Adafruit_NeoTrellis *t = (_trelli+y/NEO_TRELLIS_NUM_ROWS*_cols) + x/NEO_TRELLIS_NUM_COLS;
    int xkey = NEO_TRELLIS_X(x);
    int ykey = NEO_TRELLIS_Y(y%NEO_TRELLIS_NUM_ROWS*NEO_TRELLIS_NUM_COLS);

    t->registerCallback(NEO_TRELLIS_XY(xkey, ykey), cb);
}

void Adafruit_MultiTrellis::registerCallback(uint16_t num, TrellisCallback (*cb)(keyEvent)){
    uint8_t x = num % (NEO_TRELLIS_NUM_COLS*_cols);
    uint8_t y = num / (NEO_TRELLIS_NUM_COLS*_cols);

    registerCallback(x, y, cb);
}

void Adafruit_MultiTrellis::unregisterCallback(uint8_t x, uint8_t y)
{
    Adafruit_NeoTrellis *t = (_trelli+y/NEO_TRELLIS_NUM_ROWS*_cols) + x/NEO_TRELLIS_NUM_COLS;
    int xkey = NEO_TRELLIS_X(x);
    int ykey = NEO_TRELLIS_Y(y%NEO_TRELLIS_NUM_ROWS*NEO_TRELLIS_NUM_COLS);

    t->unregisterCallback(NEO_TRELLIS_XY(xkey, ykey));
}

void Adafruit_MultiTrellis::unregisterCallback(uint16_t num){
    uint8_t x = num % (NEO_TRELLIS_NUM_COLS*_cols);
    uint8_t y = num / (NEO_TRELLIS_NUM_COLS*_cols);

    unregisterCallback(x, y);
}

void Adafruit_MultiTrellis::activateKey(uint8_t x, uint8_t y, uint8_t edge, bool enable)
{
    Adafruit_NeoTrellis *t = (_trelli+y/NEO_TRELLIS_NUM_ROWS*_cols) + x/NEO_TRELLIS_NUM_COLS;
    int xkey = NEO_TRELLIS_X(x);
    int ykey = NEO_TRELLIS_Y(y%NEO_TRELLIS_NUM_ROWS*NEO_TRELLIS_NUM_COLS);

    t->activateKey(NEO_TRELLIS_XY(xkey, ykey), edge, enable);
}

void Adafruit_MultiTrellis::activateKey(uint16_t num, uint8_t edge, bool enable)
{
    uint8_t x = num % (NEO_TRELLIS_NUM_COLS*_cols);
    uint8_t y = num / (NEO_TRELLIS_NUM_COLS*_cols);

    activateKey(x, y, edge, enable);
}

void Adafruit_MultiTrellis::setPixelColor(uint8_t x, uint8_t y, uint32_t color)
{
    Adafruit_NeoTrellis *t = (_trelli+y/NEO_TRELLIS_NUM_ROWS*_cols) + x/NEO_TRELLIS_NUM_COLS;
    int xkey = NEO_TRELLIS_X(x);
    int ykey = NEO_TRELLIS_Y(y%NEO_TRELLIS_NUM_ROWS*NEO_TRELLIS_NUM_COLS);

    t->pixels.setPixelColor(NEO_TRELLIS_XY(xkey, ykey), color);
}

void Adafruit_MultiTrellis::setPixelColor(uint16_t num, uint32_t color)
{
    uint8_t x = num % (NEO_TRELLIS_NUM_COLS*_cols);
    uint8_t y = num / (NEO_TRELLIS_NUM_COLS*_cols);

    setPixelColor(x, y, color);
}

void Adafruit_MultiTrellis::show()
{
    Adafruit_NeoTrellis *t;
    for(int n=0; n<_rows; n++){
        for(int m=0; m<_cols; m++){
            t = (_trelli+n*_cols) + m;
            t->pixels.show();
        }
    }
}

void Adafruit_MultiTrellis::read()
{
    Adafruit_NeoTrellis *t;
    for(int n=0; n<_rows; n++){
        for(int m=0; m<_cols; m++){
            t = (_trelli+n*_cols) + m;

            uint8_t count = t->getKeypadCount();
            delayMicroseconds(500);
            if(count > 0){
                keyEventRaw e[count];
                t->readKeypad(e, count);
                for(int i=0; i<count; i++){
                    //call any callbacks associated with the key
                    e[i].bit.NUM = NEO_TRELLIS_SEESAW_KEY(e[i].bit.NUM);
                    if(e[i].bit.NUM < NEO_TRELLIS_NUM_KEYS && t->_callbacks[e[i].bit.NUM] != NULL){
                        //update the event with the multitrellis number
                        keyEvent evt = { e[i].bit.EDGE, e[i].bit.NUM };
                        int x = NEO_TRELLIS_X(e[i].bit.NUM);
                        int y = NEO_TRELLIS_Y(e[i].bit.NUM);

                        x = x + m*NEO_TRELLIS_NUM_COLS;
                        y = y + n*NEO_TRELLIS_NUM_ROWS;

                        evt.bit.NUM = y*NEO_TRELLIS_NUM_COLS*_cols + x;

                        t->_callbacks[e[i].bit.NUM](evt);
                    }
                }
            }
        }
    }
}