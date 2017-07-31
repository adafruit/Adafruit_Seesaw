#include "seesaw.h"

bool Adafruit_seesaw::begin(uint8_t addr)
{
	_i2caddr = addr;
	
	_i2c_init();

	SWReset();
	delay(500);

	uint8_t c = this->read8(SEESAW_STATUS_BASE, SEESAW_STATUS_VERSION);

	if(c != SEESAW_HW_ID_CODE) return false;
	
	return true;
}


void Adafruit_seesaw::SWReset()
{
	this->write8(SEESAW_STATUS_BASE, SEESAW_STATUS_SWRST, 0xFF);
}

void Adafruit_seesaw::pinMode(uint8_t pin, uint8_t mode)
{
	uint8_t cmd[] = {pin, mode};
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PINMODE_SINGLE, cmd, 2);
}

void Adafruit_seesaw::digitalWrite(uint8_t pin, uint8_t value)
{
	uint8_t cmd[] = {pin, value};
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PIN_SINGLE, cmd, 2);
}

void Adafruit_seesaw::pinModeBulk(uint32_t pins)
{
	uint8_t cmd[] = { (pins >> 24), (pins >> 16), (pins >> 8), pins };
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PINMODE_BULK, cmd, 4);
}

void Adafruit_seesaw::gpioSetBulk(uint32_t pins)
{
	uint8_t cmd[] = { (pins >> 24), (pins >> 16), (pins >> 8), pins };
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);
}

void Adafruit_seesaw::gpioClrBulk(uint32_t pins)
{
	uint8_t cmd[] = { (pins >> 24), (pins >> 16), (pins >> 8), pins };
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 4);
}

void Adafruit_seesaw::analogWrite(uint8_t pin, uint8_t value)
{
	uint8_t cmd[] = {pin, value};
	this->write(SEESAW_TIMER_BASE, SEESAW_TIMER_PWM, cmd, 2);
}

void Adafruit_seesaw::write8(byte regHigh, byte regLow, byte value)
{
	this->write(regHigh, regLow, &value, 1);
}

uint8_t Adafruit_seesaw::read8(byte regHigh, byte regLow)
{
	uint8_t ret;
	this->read(regHigh, regLow, &ret, 1);
	
	return ret;
}

void Adafruit_seesaw::_i2c_init()
{
	Wire.begin();
}

void Adafruit_seesaw::read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num)
{
	uint8_t value;
	uint8_t pos = 0;
	
	//on arduino we need to read in 32 byte chunks
	while(pos < num){
		
		uint8_t read_now = min(32, num - pos);
		Wire.beginTransmission((uint8_t)_i2caddr);
		Wire.write((uint8_t)regHigh);
		Wire.write((uint8_t)regLow);
		Wire.endTransmission();

		//TODO: tune this
		delayMicroseconds(250);

		Wire.requestFrom((uint8_t)_i2caddr, read_now);
		
		for(int i=0; i<read_now; i++){
			buf[pos] = Wire.read();
			pos++;
		}
	}
}

void Adafruit_seesaw::write(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num)
{
	Wire.beginTransmission((uint8_t)_i2caddr);
	Wire.write((uint8_t)regHigh);
	Wire.write((uint8_t)regLow);
	Wire.write((uint8_t *)buf, num);
	Wire.endTransmission();
}