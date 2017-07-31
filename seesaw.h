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
    };

    enum
    {
        SEESAW_GPIO_PINMODE_SINGLE = 0x00,
        SEESAW_GPIO_PIN_SINGLE = 0x01,
        SEESAW_GPIO_PINMODE_BULK = 0x02,
        SEESAW_GPIO_BULK = 0x03,
        SEESAW_GPIO_BULK_SET = 0x04,
        SEESAW_GPIO_BULK_CLR = 0x05,
        SEESAW_GPIO_BULK_TOGGLE = 0x06,
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
        void analogWrite(uint8_t pin, uint8_t value);
        void digitalWrite(uint8_t pin, uint8_t value);

        void pinModeBulk(uint32_t pins);
        void gpioSetBulk(uint32_t pins);
        void gpioClrBulk(uint32_t pins);

	private:
		uint8_t _i2caddr;
		
		void      write8(byte regHigh, byte regLow, byte value);
        uint8_t   read8(byte regHigh, byte regLow);
		
		void read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
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