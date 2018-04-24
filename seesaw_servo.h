#ifndef _SEESAW_SERVO_H
#define _SEESAW_SERVO_H

#include "Adafruit_seesaw.h"

class seesaw_Servo {

 public:
  seesaw_Servo(Adafruit_seesaw *ss) {
  	_ss = ss;
  	_attached = false;
  }

  ~seesaw_Servo() {}
  bool begin(uint8_t addr = SEESAW_ADDRESS, int8_t flow=-1);

  uint8_t attach(int pin);           // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
  uint8_t attach(int pin, int min, int max); // as above but also sets min and max values for writes. 
  void detach() { _attached = false; }
  void write(int value);             // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds 
  void writeMicroseconds(int value); // Write pulse width in microseconds 
  int read();                        // returns current pulse width as an angle between 0 and 180 degrees
  int readMicroseconds() { return _sval/3.2768; }            // returns current pulse width in microseconds for this servo (was read_us() in first release)
  bool attached() { return _attached; }                   // return true if this servo is attached, otherwise false 

private:
   Adafruit_seesaw *_ss;
   bool _attached;
   uint16_t _sval;
   uint8_t _pin;
   int8_t min;                       // minimum is this value times 4 added to MIN_PULSE_WIDTH    
   int8_t max;  
};

#endif