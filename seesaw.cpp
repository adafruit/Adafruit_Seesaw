#include "seesaw.h"

#ifdef INCLUDE_DAP //DAP support disabled by default
static uint8_t hid_buffer[256];
#define REPORT_SIZE 64
#endif //INCLUDE_DAP

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
	uint8_t cmd[] = {pin, mode};
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PINMODE_SINGLE, cmd, 2);
}

void Adafruit_seesaw::digitalWrite(uint8_t pin, uint8_t value)
{
	uint8_t cmd[] = {pin, value};
	this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PIN_SINGLE, cmd, 2);
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

#ifdef INCLUDE_DAP

int Adafruit_seesaw::dbg_dap_cmd(uint8_t *data, int size, int rsize)
{
	//TODO: leaving off here, we can only write 64 bytes at a time
  char cmd = data[0];
  int res;

  memset(hid_buffer, 0xff, REPORT_SIZE + 1);

  this->write(SEESAW_DAP_BASE, 0x00, data, size);
  delay(1);

  this->read(SEESAW_DAP_BASE, 0x00, hid_buffer, REPORT_SIZE + 1);
  delay(1);

  check(hid_buffer[0] == cmd, "invalid response received");

  memcpy(data, &hid_buffer[1], size);

  return 0;
}

void Adafruit_seesaw::check(bool cond, char *fmt)
{
  if (!cond)
  {
    //va_list args;

    //dbg_close();

   	perror_exit(fmt);
  }
}

void Adafruit_seesaw::perror_exit(char *text)
{
  //dbg_close();
  Serial.println(text);
  while(1);
}

/*- DAP Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_led(int index, int state)
{
  uint8_t buf[3];

  buf[0] = ID_DAP_LED;
  buf[1] = index;
  buf[2] = state;
  dbg_dap_cmd(buf, sizeof(buf), 3);

  check(DAP_OK == buf[0], "DAP_LED failed");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_connect(void)
{
  uint8_t buf[2];

  buf[0] = ID_DAP_CONNECT;
  buf[1] = DAP_PORT_SWD;
  dbg_dap_cmd(buf, sizeof(buf), 2);

  check(DAP_PORT_SWD == buf[0], "DAP_CONNECT failed");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_disconnect(void)
{
  uint8_t buf[1];

  buf[0] = ID_DAP_DISCONNECT;
  dbg_dap_cmd(buf, sizeof(buf), 1);
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_swj_clock(uint32_t clock)
{
  uint8_t buf[5];

  buf[0] = ID_DAP_SWJ_CLOCK;
  buf[1] = clock & 0xff;
  buf[2] = (clock >> 8) & 0xff;
  buf[3] = (clock >> 16) & 0xff;
  buf[4] = (clock >> 24) & 0xff;
  dbg_dap_cmd(buf, sizeof(buf), 5);

  check(DAP_OK == buf[0], "SWJ_CLOCK failed");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_transfer_configure(uint8_t idle, uint16_t count, uint16_t retry)
{
  uint8_t buf[6];

  buf[0] = ID_DAP_TRANSFER_CONFIGURE;
  buf[1] = idle;
  buf[2] = count & 0xff;
  buf[3] = (count >> 8) & 0xff;
  buf[4] = retry & 0xff;
  buf[5] = (retry >> 8) & 0xff;
  dbg_dap_cmd(buf, sizeof(buf), 6);

  check(DAP_OK == buf[0], "TRANSFER_CONFIGURE failed");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_swd_configure(uint8_t cfg)
{
  uint8_t buf[2];

  buf[0] = ID_DAP_SWD_CONFIGURE;
  buf[1] = cfg;
  dbg_dap_cmd(buf, sizeof(buf), 2);

  check(DAP_OK == buf[0], "SWD_CONFIGURE failed");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_get_debugger_info(void)
{
  uint8_t buf[100];
  char str[500] = "";

  buf[0] = ID_DAP_INFO;
  buf[1] = DAP_INFO_VENDOR;
  dbg_dap_cmd(buf, sizeof(buf), 2);
  strncat(str, (char *)&buf[1], buf[0]);
  strcat(str, " ");

  buf[0] = ID_DAP_INFO;
  buf[1] = DAP_INFO_PRODUCT;
  dbg_dap_cmd(buf, sizeof(buf), 2);
  strncat(str, (char *)&buf[1], buf[0]);
  strcat(str, " ");

  buf[0] = ID_DAP_INFO;
  buf[1] = DAP_INFO_SER_NUM;
  dbg_dap_cmd(buf, sizeof(buf), 2);
  strncat(str, (char *)&buf[1], buf[0]);
  strcat(str, " ");

  buf[0] = ID_DAP_INFO;
  buf[1] = DAP_INFO_FW_VER;
  dbg_dap_cmd(buf, sizeof(buf), 2);
  strncat(str, (char *)&buf[1], buf[0]);
  strcat(str, " ");

  buf[0] = ID_DAP_INFO;
  buf[1] = DAP_INFO_CAPABILITIES;
  dbg_dap_cmd(buf, sizeof(buf), 2);

  strcat(str, "(");

  if (buf[1] & DAP_PORT_SWD)
    strcat(str, "S");

  if (buf[1] & DAP_PORT_JTAG)
    strcat(str, "J");

  strcat(str, ")");

  //verbose("Debugger: %s\n", str);
  Serial.println(str);

  check(buf[1] & DAP_PORT_SWD, "SWD support required");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_reset_target(void)
{
  uint8_t buf[1];

  buf[0] = ID_DAP_RESET_TARGET;
  dbg_dap_cmd(buf, sizeof(buf), 1);

  check(DAP_OK == buf[0], "RESET_TARGET failed");
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_reset_target_hw(void)
{
  uint8_t buf[7];

  //-------------
  buf[0] = ID_DAP_SWJ_PINS;
  buf[1] = 0; // Value
  buf[2] = DAP_SWJ_nRESET | DAP_SWJ_SWCLK_TCK | DAP_SWJ_SWDIO_TMS; // Select
  buf[3] = 0; // Wait
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  dbg_dap_cmd(buf, sizeof(buf), 7);

  //-------------
  buf[0] = ID_DAP_SWJ_PINS;
  buf[1] = DAP_SWJ_nRESET; // Value
  buf[2] = DAP_SWJ_nRESET | DAP_SWJ_SWCLK_TCK | DAP_SWJ_SWDIO_TMS; // Select
  buf[3] = 0; // Wait
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  dbg_dap_cmd(buf, sizeof(buf), 7);
}

//-----------------------------------------------------------------------------
uint32_t Adafruit_seesaw::dap_read_reg(uint8_t reg)
{
  uint8_t buf[8];

  buf[0] = ID_DAP_TRANSFER;
  buf[1] = 0x00; // DAP index
  buf[2] = 0x01; // Request size
  buf[3] = reg | DAP_TRANSFER_RnW;
  dbg_dap_cmd(buf, sizeof(buf), 4);

  if (1 != buf[0] || DAP_TRANSFER_OK != buf[1])
  {
    //error_exit("invalid response while reading the register 0x%02x (count = %d, value = %d)",
    //    reg, buf[0], buf[1]);
    Serial.print("invalid response reading reg ");
  	Serial.print(reg, HEX);

  	Serial.print(" (count = ");
  	Serial.print(buf[0]);
  	Serial.print(", value = ");
  	Serial.print(buf[1]);
  	perror_exit(")");
  }

  return ((uint32_t)buf[5] << 24) | ((uint32_t)buf[4] << 16) |
         ((uint32_t)buf[3] << 8) | (uint32_t)buf[2];
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_write_reg(uint8_t reg, uint32_t data)
{
  uint8_t buf[8];

  buf[0] = ID_DAP_TRANSFER;
  buf[1] = 0x00; // DAP index
  buf[2] = 0x01; // Request size
  buf[3] = reg;
  buf[4] = data & 0xff;
  buf[5] = (data >> 8) & 0xff;
  buf[6] = (data >> 16) & 0xff;
  buf[7] = (data >> 24) & 0xff;
  dbg_dap_cmd(buf, sizeof(buf), 8);

  if (1 != buf[0] || DAP_TRANSFER_OK != buf[1])
  {
  	Serial.print("invalid response writing to reg ");
  	Serial.print(reg, HEX);

  	Serial.print(" (count = ");
  	Serial.print(buf[0]);
  	Serial.print(", value = ");
  	Serial.print(buf[1]);
  	perror_exit(")");
  }
}

//-----------------------------------------------------------------------------
uint32_t Adafruit_seesaw::dap_read_word(uint32_t addr)
{
  dap_write_reg(SWD_AP_TAR, addr);
  return dap_read_reg(SWD_AP_DRW);
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_write_word(uint32_t addr, uint32_t data)
{
  dap_write_reg(SWD_AP_TAR, addr);
  dap_write_reg(SWD_AP_DRW, data);
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_read_block(uint32_t addr, uint8_t *data, int size)
{
  int max_size = (32 - 5) & ~3;
  int offs = 0;

  while (size)
  {
    int align, sz;
    uint8_t buf[1024];

    align = 0x400 - (addr - (addr & ~0x3ff));
    sz = (size > max_size) ? max_size : size;
    sz = (sz > align) ? align : sz;

    dap_write_reg(SWD_AP_TAR, addr);

    buf[0] = ID_DAP_TRANSFER_BLOCK;
    buf[1] = 0x00; // DAP index
    buf[2] = (sz / 4) & 0xff;
    buf[3] = ((sz / 4) >> 8) & 0xff;
    buf[4] = SWD_AP_DRW | DAP_TRANSFER_RnW | DAP_TRANSFER_APnDP;
    dbg_dap_cmd(buf, sizeof(buf), 5);

    if (DAP_TRANSFER_OK != buf[2])
    {
      //error_exit("invalid response while reading the block at 0x%08x (value = %d)",
      //    addr, buf[2]);
    }

    memcpy(&data[offs], &buf[3], sz);

    size -= sz;
    addr += sz;
    offs += sz;
  }
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_write_block(uint32_t addr, uint8_t *data, int size)
{
  int max_size = (32 - 5) & ~3;
  int offs = 0;

  while (size)
  {
    int align, sz;
    uint8_t buf[1024];

    align = 0x400 - (addr - (addr & ~0x3ff));
    sz = (size > max_size) ? max_size : size;
    sz = (sz > align) ? align : sz;

    dap_write_reg(SWD_AP_TAR, addr);

    buf[0] = ID_DAP_TRANSFER_BLOCK;
    buf[1] = 0x00; // DAP index
    buf[2] = (sz / 4) & 0xff;
    buf[3] = ((sz / 4) >> 8) & 0xff;
    buf[4] = SWD_AP_DRW | DAP_TRANSFER_APnDP;
    memcpy(&buf[5], &data[offs], sz);
    dbg_dap_cmd(buf, 5 + sz, 0);

    if (DAP_TRANSFER_OK != buf[2])
    {
      //error_exit("invalid response while writing the block at 0x%08x (value = %d)",
      //    addr, buf[2]);
    }

    size -= sz;
    addr += sz;
    offs += sz;
  }
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_reset_link(void)
{
  uint8_t buf[128];

  //-------------
  buf[0] = ID_DAP_SWJ_SEQUENCE;
  buf[1] = (7 + 2 + 7 + 1) * 8;
  buf[2] = 0xff;
  buf[3] = 0xff;
  buf[4] = 0xff;
  buf[5] = 0xff;
  buf[6] = 0xff;
  buf[7] = 0xff;
  buf[8] = 0xff;
  buf[9] = 0x9e;
  buf[10] = 0xe7;
  buf[11] = 0xff;
  buf[12] = 0xff;
  buf[13] = 0xff;
  buf[14] = 0xff;
  buf[15] = 0xff;
  buf[16] = 0xff;
  buf[17] = 0xff;
  buf[18] = 0x00;

  dbg_dap_cmd(buf, sizeof(buf), 19);
  check(DAP_OK == buf[0], "SWJ_SEQUENCE failed");

  //-------------
  buf[0] = ID_DAP_TRANSFER;
  buf[1] = 0; // DAP index
  buf[2] = 1; // Request size
  buf[3] = SWD_DP_R_IDCODE | DAP_TRANSFER_RnW;
  dbg_dap_cmd(buf, sizeof(buf), 4);
}

//-----------------------------------------------------------------------------
uint32_t Adafruit_seesaw::dap_read_idcode(void)
{
  return dap_read_reg(SWD_DP_R_IDCODE);
}

//-----------------------------------------------------------------------------
void Adafruit_seesaw::dap_target_prepare(void)
{
  dap_write_reg(SWD_DP_W_ABORT, 0x00000016);
  dap_write_reg(SWD_DP_W_SELECT, 0x00000000);
  dap_write_reg(SWD_DP_W_CTRL_STAT, 0x50000f00);
  dap_write_reg(SWD_AP_CSW, 0x23000052);
}

#endif // INCLUDE_DAP
