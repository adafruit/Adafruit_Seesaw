#include "Adafruit_seesaw.h"

bool Adafruit_seesaw::begin(uint8_t addr)
{
	_i2caddr = addr;
	
	_i2c_init();

	SWReset();
	delay(500);

	uint8_t c = this->read8(SEESAW_STATUS_BASE, SEESAW_STATUS_HW_ID);

	if(c != SEESAW_HW_ID_CODE) return false;
	
	return true;
}


void Adafruit_seesaw::SWReset()
{
	this->write8(SEESAW_STATUS_BASE, SEESAW_STATUS_SWRST, 0xFF);
}

uint32_t Adafruit_seesaw::getOptions()
{
	uint8_t buf[4];
	this->read(SEESAW_STATUS_BASE, SEESAW_STATUS_OPTIONS, buf, 4);
	uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return ret;
}

uint32_t Adafruit_seesaw::getVersion()
{
	uint8_t buf[4];
	this->read(SEESAW_STATUS_BASE, SEESAW_STATUS_VERSION, buf, 4);
	uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return ret;
}


void Adafruit_seesaw::pinMode(uint8_t pin, uint8_t mode)
{
	pinModeBulk(1ul << pin, mode);
}

void Adafruit_seesaw::digitalWrite(uint8_t pin, uint8_t value)
{
	digitalWriteBulk(1ul << pin, value);
}

bool Adafruit_seesaw::digitalRead(uint8_t pin)
{
	return digitalReadBulk((1ul << pin)) != 0;
}

uint32_t Adafruit_seesaw::digitalReadBulk(uint32_t pins)
{
	uint8_t buf[4];
	this->read(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 4);
	uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return ret & pins;
}

void Adafruit_seesaw::setGPIOInterrupts(uint32_t pins, bool enabled)
{
	uint8_t cmd[] = { (pins >> 24), (pins >> 16), (pins >> 8), pins };
	if(enabled)
		this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_INTENSET, cmd, 4);
	else
		this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_INTENCLR, cmd, 4);
}

uint16_t Adafruit_seesaw::analogRead(uint8_t pin)
{
	uint8_t buf[2];
	this->read(SEESAW_ADC_BASE, SEESAW_ADC_CHANNEL_OFFSET + pin, buf, 2, 500);
	uint16_t ret = ((uint16_t)buf[0] << 8) | buf[1];
  delay(1);
	return ret;
}

//TODO: not sure if this is how this is gonna work yet
void Adafruit_seesaw::analogReadBulk(uint16_t *buf, uint8_t num)
{
	uint8_t rawbuf[num * 2];
	this->read(SEESAW_ADC_BASE, SEESAW_ADC_CHANNEL_OFFSET, rawbuf, num * 2);
	for(int i=0; i<num; i++){
		buf[i] = ((uint16_t)rawbuf[i * 2] << 8) | buf[i * 2 + 1];
	}
}

void Adafruit_seesaw::pinModeBulk(uint32_t pins, uint8_t mode)
{
	uint8_t cmd[] = { (pins >> 24), (pins >> 16), (pins >> 8), pins };
	switch (mode){
		case OUTPUT:
			this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRSET_BULK, cmd, 4);
			break;
		case INPUT:
			this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
			break;
		case INPUT_PULLUP:
			this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
			this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
			this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);
			break;
	}
		
}

void Adafruit_seesaw::digitalWriteBulk(uint32_t pins, uint8_t value)
{
	uint8_t cmd[] = { (pins >> 24), (pins >> 16), (pins >> 8), pins };
	if(value)
		this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);
	else
		this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 4);
}

void Adafruit_seesaw::analogWrite(uint8_t pin, uint8_t value)
{
	uint8_t cmd[] = {pin, value};
	this->write(SEESAW_TIMER_BASE, SEESAW_TIMER_PWM, cmd, 2);
}

void Adafruit_seesaw::enableSercomDataRdyInterrupt(uint8_t sercom)
{
	_sercom_inten.DATA_RDY = 1;
	this->write8(SEESAW_SERCOM0_BASE + sercom, SEESAW_SERCOM_INTEN, _sercom_inten.get());
}

void Adafruit_seesaw::disableSercomDataRdyInterrupt(uint8_t sercom)
{
	_sercom_inten.DATA_RDY = 0;
	this->write8(SEESAW_SERCOM0_BASE + sercom, SEESAW_SERCOM_INTEN, _sercom_inten.get());
}

char Adafruit_seesaw::readSercomData(uint8_t sercom)
{
	return this->read8(SEESAW_SERCOM0_BASE + sercom, SEESAW_SERCOM_DATA);
}

void Adafruit_seesaw::setI2CAddr(uint8_t addr)
{
  this->EEPROMWrite8(SEESAW_EEPROM_I2C_ADDR, addr);
  delay(250);
  this->begin(addr); //restart w/ the new addr
}

uint8_t Adafruit_seesaw::getI2CAddr()
{
  return this->read8(SEESAW_EEPROM_BASE, SEESAW_EEPROM_I2C_ADDR);
}

void Adafruit_seesaw::EEPROMWrite8(uint8_t addr, uint8_t val)
{
  this->EEPROMWrite(addr, &val, 1);
}

void Adafruit_seesaw::EEPROMWrite(uint8_t addr, uint8_t *buf, uint8_t size)
{
  this->write(SEESAW_EEPROM_BASE, addr, buf, size);
}

uint8_t Adafruit_seesaw::EEPROMRead8(uint8_t addr)
{
  return this->read8(SEESAW_EEPROM_BASE, addr);
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

void Adafruit_seesaw::read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t delay)
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
		delayMicroseconds(delay);

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

//print wrapper
size_t Adafruit_seesaw::write(uint8_t character) {
	//TODO: add support for multiple sercoms
	this->write8(SEESAW_SERCOM0_BASE, SEESAW_SERCOM_DATA, character);
	delay(1); //TODO: this can be optimized... it's only needed for longer writes
}

size_t Adafruit_seesaw::write(const char *str) {
	uint8_t buf[32];
	uint8_t len = 0;
	while(*str){
		buf[len] = *str;
		str++;
		len++;
	}
	this->write(SEESAW_SERCOM0_BASE, SEESAW_SERCOM_DATA, buf, len);
	return len;
}

void Adafruit_seesaw::writeEmpty(uint8_t regHigh, uint8_t regLow)
{
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)regHigh);
    Wire.write((uint8_t)regLow);
    Wire.endTransmission();
}
