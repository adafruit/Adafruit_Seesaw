#ifndef LIB_SEESAW_H
#define LIB_SEESAW_H

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define SEESAW_ADDRESS                (0x49)
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    enum
    {
        SEESAW_STATUS_BASE = 0x00,
        SEESAW_GPIO_BASE = 0x01,

        SEESAW_TIMER_BASE = 0x08,
        SEESAW_ADC_BASE = 0x09,
    };

    enum
    {
        SEESAW_GPIO_PINMODE_SINGLE = 0x00,
        SEESAW_GPIO_PIN_SINGLE = 0x01,
        SEESAW_GPIO_DIRSET_BULK = 0x02,
        SEESAW_GPIO_DIRCLR_BULK = 0x03,
        SEESAW_GPIO_BULK = 0x04,
        SEESAW_GPIO_BULK_SET = 0x05,
        SEESAW_GPIO_BULK_CLR = 0x06,
        SEESAW_GPIO_BULK_TOGGLE = 0x07,
    };

    enum
    {
        SEESAW_STATUS_VERSION = 0x02,
        SEESAW_STATUS_SWRST = 0x7F,
    };

    enum
    {
        SEESAW_TIMER_STATUS = 0x00,
        SEESAW_TIMER_PWM = 0x01,
    };
	
    enum
    {
        SEESAW_ADC_STATUS = 0x00,
        SEESAW_ADC_INTEN = 0x02,
        SEESAW_ADC_INTENCLR = 0x03,
        SEESAW_ADC_WINMODE = 0x04,
        SEESAW_ADC_WINTHRESH = 0x05,
        SEESAW_ADC_INTCLR = 0x06,
        SEESAW_ADC_CHANNEL_OFFSET = 0x07
    };

/*=========================================================================*/

#define SEESAW_HW_ID_CODE			0x55

class Adafruit_seesaw {
	public:
		//constructors
		Adafruit_seesaw(void) {};
		~Adafruit_seesaw(void) {};
		
		bool begin(uint8_t addr = SEESAW_ADDRESS);
		void SWReset();

        void pinMode(uint8_t pin, uint8_t mode);
        void pinModeBulk(uint32_t pins, uint8_t mode);
        void analogWrite(uint8_t pin, uint8_t value);
        void digitalWrite(uint8_t pin, uint8_t value);
        void digitalWriteBulk(uint32_t pins, uint8_t value);

        bool digitalRead(uint8_t pin);
        uint32_t digitalReadBulk(uint32_t pins);

        uint16_t analogRead(uint8_t pin);
        void analogReadBulk(uint16_t *buf, uint8_t num);

	private:
		uint8_t _i2caddr;
		
		void      write8(byte regHigh, byte regLow, byte value);
        uint8_t   read8(byte regHigh, byte regLow);
		
		void read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t delay = 250);
		void write(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
		void _i2c_init();
		
/*=========================================================================
	REGISTER BITFIELDS
    -----------------------------------------------------------------------*/

		/*
		// The status register
        struct status {

            uint8_t ERROR: 1;

            void set(uint8_t data){
            	ERROR = data & 0x01;
            }
        };
        status _status;
        */

/*=========================================================================*/
};

#endif