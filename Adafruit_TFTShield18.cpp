#include "Adafruit_TFTShield18.h"

bool Adafruit_TFTShield18::begin(uint8_t addr, int8_t flow){
    if (! Adafruit_seesaw::begin(addr, flow)) {
        return false;
    }

    pinMode(TFTSHIELD_RESET_PIN, OUTPUT);
    pinModeBulk(TFTSHIELD_BUTTON_ALL, INPUT_PULLUP);
    return true;
}

void Adafruit_TFTShield18::setBacklight(uint16_t value){
    uint8_t cmd[] = {0x00, (uint8_t)(value >> 8), (uint8_t)value};
    this->write(SEESAW_TIMER_BASE, SEESAW_TIMER_PWM, cmd, 3);
}

void Adafruit_TFTShield18::setBacklightFreq(uint16_t freq){
    uint8_t cmd[] = {0x0, (uint8_t)(freq >> 8), (uint8_t)freq};
	this->write(SEESAW_TIMER_BASE, SEESAW_TIMER_FREQ, cmd, 3);
}

void Adafruit_TFTShield18::tftReset(bool rst){
   digitalWrite(TFTSHIELD_RESET_PIN, rst); 
}

uint32_t Adafruit_TFTShield18::readButtons(){
    return digitalReadBulk(TFTSHIELD_BUTTON_ALL);
}