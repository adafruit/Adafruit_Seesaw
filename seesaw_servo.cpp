#include "seesaw_servo.h"

#define MIN_PULSE 3277
#define MAX_PULSE 6554

bool seesaw_Servo::begin(uint8_t addr, int8_t flow)
{
	return _ss->begin(addr, flow);
}

uint8_t seesaw_Servo::attach(int pin)
{
	_pin = pin;
	//set frequency to 50 hz
	_ss->setPWMFreq(_pin, 50);
	_attached = true;
	min = 0;
	max = 0;
}

uint8_t seesaw_Servo::attach(int pin, int min, int max)
{
	attach(pin);
	min = min;
	max = max;
}

void seesaw_Servo::write(int value)
{
	if(value < 200){
		//angle
		uint16_t val = map(value, 0, 180, MIN_PULSE, MAX_PULSE);
		val = constrain(val, ((uint16_t)min*4 + MIN_PULSE), (MAX_PULSE - (uint16_t)max*4));
		_ss->analogWrite(_pin, val);
		_sval = val;
	}
	else
		writeMicroseconds(value);
	
}

int seesaw_Servo::read()
{
	return map(_sval, MIN_PULSE, MAX_PULSE, 0, 180);
}

void seesaw_Servo::writeMicroseconds(int value)
{
	uint16_t val = 3.2768 * value;
	_ss->analogWrite(_pin, val);
	_sval = val;
}   