#ifndef _CRICKIT_TERSTER_H
#define _CRICKIT_TERSTER_H

#include "Adafruit_seesaw.h"

#define CRICKIT_SIGNAL1 2
#define CRICKIT_SIGNAL2 3
#define CRICKIT_SIGNAL3 40
#define CRICKIT_SIGNAL4 41
#define CRICKIT_SIGNAL5 11
#define CRICKIT_SIGNAL6 10
#define CRICKIT_SIGNAL7 9
#define CRICKIT_SIGNAL8 8

#define CRICKIT_SERVO4 14
#define CRICKIT_SERVO3 15
#define CRICKIT_SERVO2 16
#define CRICKIT_SERVO1 17

#define CRICKIT_MOTOR_A1 22
#define CRICKIT_MOTOR_A2 23
#define CRICKIT_MOTOR_B1 19
#define CRICKIT_MOTOR_B2 18
#define CRICKIT_DRIVE1 13
#define CRICKIT_DRIVE2 12
#define CRICKIT_DRIVE3 43
#define CRICKIT_DRIVE4 42

#define CRICKIT_TOUCH1 0
#define CRICKIT_TOUCH2 1
#define CRICKIT_TOUCH3 2
#define CRICKIT_TOUCH4 3

#define CRICKIT_DUTY_CYCLE_OFF 0
#define CRICKIT_DUTY_CYCLE_MAX 65535

// ESP32 core has macros that interfere with the compilation
#ifdef ARDUINO_ARCH_ESP32
#pragma push_macro("pinMode")
#undef pinMode
#pragma push_macro("analogWrite")
#undef analogWrite
#pragma push_macro("analogRead")
#undef analogRead
#pragma push_macro("digitalRead")
#undef digitalRead
#pragma push_macro("digitalWrite")
#undef digitalWrite
#pragma push_macro("touchRead")
#undef touchRead
#endif

/**************************************************************************/
/*!
   @brief  Class that stores state and functions for interacting with Crickit
   variant of seesaw helper IC
*/
/**************************************************************************/
class Adafruit_Crickit : public Adafruit_seesaw {
public:
  Adafruit_Crickit(){};
  ~Adafruit_Crickit(){};

  void analogWrite(uint8_t pin, uint16_t value, uint8_t width = 8);
  uint16_t analogRead(uint8_t pin);
  void setPWMFreq(uint8_t pin, uint16_t freq);
};

#endif

// ESP32 core macros reinstate
#ifdef ARDUINO_ARCH_ESP32
#pragma pop_macro("touchRead")
#pragma pop_macro("digitalWrite")
#pragma pop_macro("digitalRead")
#pragma pop_macro("analogRead")
#pragma pop_macro("analogWrite")
#pragma pop_macro("pinMode")
#endif
