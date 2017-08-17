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
        SEESAW_ADC_INTCLR = 0x06,
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

/*=========================================================================*/

#define SEESAW_HW_ID_CODE			0x55

/*- DAP Types -------------------------------------------------------------------*/
#define DAP_FREQ            16000000 // Hz

enum
{
  ID_DAP_INFO               = 0x00,
  ID_DAP_LED                = 0x01,
  ID_DAP_CONNECT            = 0x02,
  ID_DAP_DISCONNECT         = 0x03,
  ID_DAP_TRANSFER_CONFIGURE = 0x04,
  ID_DAP_TRANSFER           = 0x05,
  ID_DAP_TRANSFER_BLOCK     = 0x06,
  ID_DAP_TRANSFER_ABORT     = 0x07,
  ID_DAP_WRITE_ABORT        = 0x08,
  ID_DAP_DELAY              = 0x09,
  ID_DAP_RESET_TARGET       = 0x0a,
  ID_DAP_SWJ_PINS           = 0x10,
  ID_DAP_SWJ_CLOCK          = 0x11,
  ID_DAP_SWJ_SEQUENCE       = 0x12,
  ID_DAP_SWD_CONFIGURE      = 0x13,
  ID_DAP_JTAG_SEQUENCE      = 0x14,
  ID_DAP_JTAG_CONFIGURE     = 0x15,
  ID_DAP_JTAG_IDCODE        = 0x16,
};

enum
{
  DAP_INFO_VENDOR           = 0x01,
  DAP_INFO_PRODUCT          = 0x02,
  DAP_INFO_SER_NUM          = 0x03,
  DAP_INFO_FW_VER           = 0x04,
  DAP_INFO_DEVICE_VENDOR    = 0x05,
  DAP_INFO_DEVICE_NAME      = 0x06,
  DAP_INFO_CAPABILITIES     = 0xf0,
  DAP_INFO_PACKET_COUNT     = 0xfe,
  DAP_INFO_PACKET_SIZE      = 0xff,
};

enum
{
  DAP_TRANSFER_APnDP        = 1 << 0,
  DAP_TRANSFER_RnW          = 1 << 1,
  DAP_TRANSFER_A2           = 1 << 2,
  DAP_TRANSFER_A3           = 1 << 3,
  DAP_TRANSFER_MATCH_VALUE  = 1 << 4,
  DAP_TRANSFER_MATCH_MASK   = 1 << 5,
};

enum
{
  DAP_TRANSFER_INVALID      = 0,
  DAP_TRANSFER_OK           = 1 << 0,
  DAP_TRANSFER_WAIT         = 1 << 1,
  DAP_TRANSFER_FAULT        = 1 << 2,
  DAP_TRANSFER_ERROR        = 1 << 3,
  DAP_TRANSFER_MISMATCH     = 1 << 4,
  DAP_TRANSFER_NO_TARGET    = 7,
};

enum
{
  DAP_PORT_SWD   = 1 << 0,
  DAP_PORT_JTAG  = 1 << 1,
};

enum
{
  DAP_SWJ_SWCLK_TCK = 1 << 0,
  DAP_SWJ_SWDIO_TMS = 1 << 1,
  DAP_SWJ_TDI       = 1 << 2,
  DAP_SWJ_TDO       = 1 << 3,
  DAP_SWJ_nTRST     = 1 << 5,
  DAP_SWJ_nRESET    = 1 << 7,
};

enum
{
  DAP_OK    = 0x00,
  DAP_ERROR = 0xff,
};

enum
{
  SWD_DP_R_IDCODE    = 0x00,
  SWD_DP_W_ABORT     = 0x00,
  SWD_DP_R_CTRL_STAT = 0x04,
  SWD_DP_W_CTRL_STAT = 0x04, // When CTRLSEL == 0
  SWD_DP_W_WCR       = 0x04, // When CTRLSEL == 1
  SWD_DP_R_RESEND    = 0x08,
  SWD_DP_W_SELECT    = 0x08,
  SWD_DP_R_RDBUFF    = 0x0c,
};

enum
{
  SWD_AP_CSW  = 0x00 | DAP_TRANSFER_APnDP,
  SWD_AP_TAR  = 0x04 | DAP_TRANSFER_APnDP,
  SWD_AP_DRW  = 0x0c | DAP_TRANSFER_APnDP,

  SWD_AP_DB0  = 0x00 | DAP_TRANSFER_APnDP, // 0x10
  SWD_AP_DB1  = 0x04 | DAP_TRANSFER_APnDP, // 0x14
  SWD_AP_DB2  = 0x08 | DAP_TRANSFER_APnDP, // 0x18
  SWD_AP_DB3  = 0x0c | DAP_TRANSFER_APnDP, // 0x1c

  SWD_AP_CFG  = 0x04 | DAP_TRANSFER_APnDP, // 0xf4
  SWD_AP_BASE = 0x08 | DAP_TRANSFER_APnDP, // 0xf8
  SWD_AP_IDR  = 0x0c | DAP_TRANSFER_APnDP, // 0xfc
};

/*- Definitions -------------------------------------------------------------*/
enum
{
  FUSE_READ   = (1 << 0),
  FUSE_WRITE  = (1 << 1),
  FUSE_VERIFY = (1 << 2),
};

enum
{
  M0_PLUS,
};

typedef struct
{
  bool         fuse_read;
  bool         fuse_write;
  bool         fuse_verify;
  int          fuse_start;
  int          fuse_end;
  uint32_t     fuse_value;
  char         *fuse_name;
  char         *name;
  int32_t      offset;
  int32_t      size;

  // For target use only
  int          file_size;
  uint8_t      *file_data;

  int          fuse_size;
  uint8_t      *fuse_data;
} options_t;

typedef struct
{
  uint32_t  dsu_did;
  char      *name;
  uint32_t  flash_size;
  uint32_t  n_pages;
} device_t;

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

        virtual size_t write(uint8_t);
        virtual size_t write(const char *str);

        void dap_led(int index, int state);
        void dap_connect(void);
        void dap_disconnect(void);
        void dap_swj_clock(uint32_t clock);
        void dap_transfer_configure(uint8_t idle, uint16_t count, uint16_t retry);
        void dap_swd_configure(uint8_t cfg);
        void dap_get_debugger_info(void);
        void dap_reset_target(void);
        void dap_reset_target_hw(void);
        uint32_t dap_read_reg(uint8_t reg);
        void dap_write_reg(uint8_t reg, uint32_t data);
        uint32_t dap_read_word(uint32_t addr);
        void dap_write_word(uint32_t addr, uint32_t data);
        void dap_read_block(uint32_t addr, uint8_t *data, int size);
        void dap_write_block(uint32_t addr, uint8_t *data, int size);
        void dap_reset_link(void);
        uint32_t dap_read_idcode(void);
        void dap_target_prepare(void);
        void dap_set_target(uint8_t target);

	protected:
		uint8_t _i2caddr;
        int dbg_dap_cmd(uint8_t *data, int size, int rsize);
        void perror_exit(char *text);
        void check(bool cond, char *fmt);

		void      write8(byte regHigh, byte regLow, byte value);
        uint8_t   read8(byte regHigh, byte regLow);
		
		void read(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num, uint16_t delay = 125);
		void write(uint8_t regHigh, uint8_t regLow, uint8_t *buf, uint8_t num);
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

class dap_m0p : public Adafruit_seesaw {
public:
    dap_m0p(void) {};
    ~dap_m0p(void) {};

    static device_t devices[];
    device_t target_device;
    options_t options;

    void select(options_t *target_options);
    void deselect(void);
    void erase(void);
    void lock(void);
    void program(void);
    uint32_t program(uint32_t addr, uint8_t *buf);
    void verify(void);
    void read(void);
    void fuse(void);

    uint32_t program_start(void);
};

#endif