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
        SEESAW_SERCOM0_BASE = 0x02,

        SEESAW_TIMER_BASE = 0x08,
        SEESAW_ADC_BASE = 0x09,
        SEESAW_DAC_BASE = 0x0A,
        SEESAW_INTERRUPT_BASE = 0x0B,
        SEESAW_DAP_BASE = 0x0C,
        SEESAW_EEPROM_BASE = 0x0D,
        SEESAW_NEOPIXEL_BASE = 0x0E,
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
        SEESAW_GPIO_INTENSET = 0x08,
        SEESAW_GPIO_INTENCLR = 0x09,
        SEESAW_GPIO_INTFLAG = 0x0A,
        SEESAW_GPIO_PULLENSET = 0x0B,
        SEESAW_GPIO_PULLENCLR = 0x0C,
    };

    enum
    {
        SEESAW_STATUS_HW_ID = 0x01,
        SEESAW_STATUS_VERSION = 0x02,
        SEESAW_STATUS_OPTIONS = 0x03,
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
        SEESAW_ADC_CHANNEL_OFFSET = 0x07,
    };
    enum
    {
        SEESAW_SERCOM_STATUS = 0x00,
        SEESAW_SERCOM_INTEN = 0x02,
        SEESAW_SERCOM_INTENCLR = 0x03,
        SEESAW_SERCOM_BAUD = 0x04,
        SEESAW_SERCOM_DATA = 0x05,
    };
    enum
    {
        SEESAW_NEOPIXEL_STATUS = 0x00,
        SEESAW_NEOPIXEL_PIN = 0x01,
        SEESAW_NEOPIXEL_SPEED = 0x02,
        SEESAW_NEOPIXEL_BUF_LENGTH = 0x03,
        SEESAW_NEOPIXEL_BUF = 0x04,
        SEESAW_NEOPIXEL_SHOW = 0x05,
    };

/*=========================================================================*/

#define SEESAW_HW_ID_CODE			0x55
#define SEESAW_EEPROM_I2C_ADDR 0x3F

class Adafruit_seesaw : public Print {
	public:
		//constructors
		Adafruit_seesaw(void) {};
		~Adafruit_seesaw(void) {};
		
		bool begin(uint8_t addr = SEESAW_ADDRESS);
        uint32_t getOptions();
        uint32_t getVersion();
		void SWReset();

        void pinMode(uint8_t pin, uint8_t mode);
        void pinModeBulk(uint32_t pins, uint8_t mode);
        void analogWrite(uint8_t pin, uint8_t value);
        void digitalWrite(uint8_t pin, uint8_t value);
        void digitalWriteBulk(uint32_t pins, uint8_t value);

        bool digitalRead(uint8_t pin);
        uint32_t digitalReadBulk(uint32_t pins);

        void setGPIOInterrupts(uint32_t pins, bool enabled);

        uint16_t analogRead(uint8_t pin);
        void analogReadBulk(uint16_t *buf, uint8_t num);

        void enableSercomDataRdyInterrupt(uint8_t sercom = 0);
        void disableSercomDataRdyInterrupt(uint8_t sercom = 0);

        char readSercomData(uint8_t sercom = 0);

        void EEPROMWrite8(uint8_t addr, uint8_t val);
        void EEPROMWrite(uint8_t addr, uint8_t *buf, uint8_t size);
        uint8_t EEPROMRead8(uint8_t addr);

        void setI2CAddr(uint8_t addr);
        uint8_t getI2CAddr();

        virtual size_t write(uint8_t);
        virtual size_t write(const char *str);

	protected:
		uint8_t _i2caddr;

		void      write8(byte regHigh, byte regLow, byte value);
        uint8_t   read8(byte regHigh, byte regLow);
		
		void read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t delay = 125);
		void write(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
    void writeEmpty(uint8_t regHigh, uint8_t regLow);
		void _i2c_init();

/*=========================================================================
	REGISTER BITFIELDS
    -----------------------------------------------------------------------*/

		
		// The status register
        struct sercom_status {

            uint8_t ERROR: 1;
            uint8_t DATA_RDY : 1;

            void set(uint8_t data){
            	ERROR = data & 0x01;
                ERROR = data & 0x02;
            }
        };
        sercom_status _sercom_status;

        struct sercom_inten {

            uint8_t DATA_RDY : 1;

            uint8_t get(){
                return DATA_RDY;
            }
        };
        sercom_inten _sercom_inten;

/*=========================================================================*/
};

#endif