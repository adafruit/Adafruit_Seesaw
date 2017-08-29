/*
 * Copyright (c) 2013-2017, Alex Taradov <alex@taradov.com>, Adafruit <info@adafruit.com>
 * All rights reserved.
 *
 * This is mostly a re-mix of Alex Taradovs excellent Free-DAP code - we just put both halves into one library and wrapped it up in Arduino compatibility
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*- Includes ----------------------------------------------------------------*/
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "seesaw.h"

/*- Definitions -------------------------------------------------------------*/
#define FLASH_START            0
#define FLASH_ROW_SIZE         256
#define FLASH_PAGE_SIZE        64

#define DHCSR                  0xe000edf0
#define DEMCR                  0xe000edfc
#define AIRCR                  0xe000ed0c

#define DSU_CTRL_STATUS        0x41002100
#define DSU_DID                0x41002118
#define DSU_ADDR               0x41002104
#define DSU_DATA               0x4100210C
#define DSU_LENGTH             0x41002108

#define DSU_CTRL_CRC           0x00000004
#define DSU_STATUSA_DONE       0x00000100
#define DSU_STATUSA_BERR       0x00000400

#define NVMCTRL_CTRLA          0x41004000
#define NVMCTRL_CTRLB          0x41004004
#define NVMCTRL_PARAM          0x41004008
#define NVMCTRL_INTFLAG        0x41004014
#define NVMCTRL_STATUS         0x41004018
#define NVMCTRL_ADDR           0x4100401c

#define NVMCTRL_CMD_ER         0xa502
#define NVMCTRL_CMD_WP         0xa504
#define NVMCTRL_CMD_EAR        0xa505
#define NVMCTRL_CMD_WAP        0xa506
#define NVMCTRL_CMD_WL         0xa50f
#define NVMCTRL_CMD_UR         0xa541
#define NVMCTRL_CMD_PBC        0xa544
#define NVMCTRL_CMD_SSB        0xa545

#define USER_ROW_ADDR          0x00804000
#define USER_ROW_SIZE          256

#ifdef INCLUDE_DAP
/*- Variables ---------------------------------------------------------------*/
device_t Adafruit_DAP_SAM::devices[] =
{
  { 0x10040107, "SAM D09C13A",           8*1024,  128 },
  { 0x10020100, "SAM D10D14AM",         16*1024,  256 },
  { 0x10030100, "SAM D11D14A",          16*1024,  256 },
  { 0x10030000, "SAM D11D14AM",         16*1024,  256 },
  { 0x10030003, "SAM D11D14AS",         16*1024,  256 },
  { 0x10030006, "SAM D11C14A",          16*1024,  256 },
  { 0x10030106, "SAM D11C14A (Rev B)",  16*1024,  256 },
  { 0x1000120d, "SAM D20E15A",          32*1024,  512 },
  { 0x1000140a, "SAM D20E18A",         256*1024, 4096 },
  { 0x10001100, "SAM D20J18A",         256*1024, 4096 },
  { 0x10001200, "SAM D20J18A (Rev C)", 256*1024, 4096 },
  { 0x10010100, "SAM D21J18A",         256*1024, 4096 },
  { 0x10010200, "SAM D21J18A (Rev C)", 256*1024, 4096 },
  { 0x10010300, "SAM D21J18A (Rev D)", 256*1024, 4096 },
  { 0x1001020d, "SAM D21E15A (Rev C)",  32*1024,  512 },
  { 0x1001030a, "SAM D21E18A",         256*1024, 4096 },
  { 0x10010205, "SAM D21G18A",         256*1024, 4096 },
  { 0x10010305, "SAM D21G18A (Rev D)", 256*1024, 4096 },
  { 0x10010019, "SAM R21G18 ES",       256*1024, 4096 },
  { 0x10010119, "SAM R21G18",          256*1024, 4096 },
  { 0x10010219, "SAM R21G18A (Rev C)", 256*1024, 4096 },
  { 0x10010319, "SAM R21G18A (Rev D)", 256*1024, 4096 },
  { 0x11010100, "SAM C21J18A ES",      256*1024, 4096 },
  { 0x10810219, "SAM L21E18B",         256*1024, 4096 },
  { 0x10810000, "SAM L21J18A",         256*1024, 4096 },
  { 0x1081010f, "SAM L21J18B (Rev B)", 256*1024, 4096 },
  { 0x1081020f, "SAM L21J18B (Rev C)", 256*1024, 4096 },
  { 0x1081021e, "SAM R30G18A",         256*1024, 4096 },
  { 0x1081021f, "SAM R30E18A",         256*1024, 4096 },
  { 0 },
};

//-----------------------------------------------------------------------------
bool Adafruit_DAP_SAM::select(uint32_t *found_id)
{
  uint32_t dsu_did;

  // Stop the core
  dap_write_word(DHCSR, 0xa05f0003);
  dap_write_word(DEMCR, 0x00000001);
  dap_write_word(AIRCR, 0x05fa0004);

  dsu_did = dap_read_word(DSU_DID);
  *found_id = dsu_did;

  for (device_t *device = devices; device->dsu_did > 0; device++)
  {
    if (device->dsu_did == dsu_did)
    {
      target_device = *device;

      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
void Adafruit_DAP_SAM::deselect(void)
{
  dap_write_word(DEMCR, 0x00000000);
  dap_write_word(AIRCR, 0x05fa0004);
}

//-----------------------------------------------------------------------------
void Adafruit_DAP_SAM::erase(void)
{
  dap_write_word(DSU_CTRL_STATUS, 0x00001f00); // Clear flags
  dap_write_word(DSU_CTRL_STATUS, 0x00000010); // Chip erase
  delay(100);
  while (0 == (dap_read_word(DSU_CTRL_STATUS) & 0x00000100));
}

//-----------------------------------------------------------------------------
void Adafruit_DAP_SAM::lock(void)
{
  dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_SSB); // Set Security Bit
}

//-----------------------------------------------------------------------------
uint32_t Adafruit_DAP_SAM::program_start(uint32_t offset)
{

  if (dap_read_word(DSU_CTRL_STATUS) & 0x00010000)
    perror_exit("device is locked, perform a chip erase before programming");

  dap_write_word(NVMCTRL_CTRLB, 0); // Enable automatic write

  dap_setup_clock(0);

  return FLASH_START + offset;
}

void Adafruit_DAP_SAM::programBlock(uint32_t addr, const uint8_t *buf)
{
    /* DM: this is actually unnecessary after a chip erase
    dap_write_word(NVMCTRL_ADDR, addr >> 1);

    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_UR); // Unlock Region
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));

    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_ER); // Erase Row
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));
    */
    dap_write_block(addr, buf, FLASH_ROW_SIZE);
}

//-----------------------------------------------------------------------------
void Adafruit_DAP_SAM::readBlock(uint32_t addr, uint8_t *buf)
{
  if (dap_read_word(DSU_CTRL_STATUS) & 0x00010000)
    perror_exit("device is locked, unable to read");

  dap_read_block(addr, buf, FLASH_ROW_SIZE);
}

/*
uint32_t Adafruit_DAP_SAM::verifyBlock(uint32_t addr)
{
   dap_write_word(DSU_DATA, 0xFFFFFFFF);
   dap_write_word(DSU_ADDR, (addr << 2));
   dap_write_word(DSU_LENGTH, FLASH_ROW_SIZE);

   dap_write_word(DSU_CTRL_STATUS, 0x00001f00); // Clear flags
   dap_write_word(DSU_CTRL_STATUS, DSU_CTRL_CRC); //start CRC

   uint32_t status = 0;
   while(0 == (status & DSU_STATUSA_DONE) ){
      status = dap_read_word(DSU_CTRL_STATUS);
      if( (status & DSU_STATUSA_BERR) > 0){
       Serial.println(status, BIN);
       perror_exit("bus read error during verify!");
     }
   }
   return dap_read_word(DSU_DATA);
}
*/


void Adafruit_DAP_SAM::verify(uint32_t length, uint32_t crc)
{
   /* to verify CRC, compare (dap_read_word(DSU_DATA) ^ 0xFFFFFFFF) to output of crc32 program on linux */
   dap_write_word(DSU_DATA, 0xFFFFFFFF);
   dap_write_word(DSU_ADDR, 0);
   dap_write_word(DSU_LENGTH, length);

   dap_write_word(DSU_CTRL_STATUS, 0x00001f00); // Clear flags
   dap_write_word(DSU_CTRL_STATUS, DSU_CTRL_CRC); //start CRC

   uint32_t status = 0;
   while(0 == (status & DSU_STATUSA_DONE) ){
      status = dap_read_word(DSU_CTRL_STATUS);
      if( (status & DSU_STATUSA_BERR) > 0){
       Serial.println(status, BIN);
       perror_exit("bus read error during verify!");
     }
   }
   if(dap_read_word(DSU_DATA)  != crc) perror_exit("verify failed!");
}

void Adafruit_DAP_SAM::fuseRead(){
  uint8_t buf[USER_ROW_SIZE];
  dap_read_block(USER_ROW_ADDR, buf, USER_ROW_SIZE);

  uint64_t fuses = ((uint64_t)buf[7] << 56) | 
          ((uint64_t)buf[6] << 48) |
          ((uint64_t)buf[5] << 40) |
          ((uint64_t)buf[4] << 32) |
          ((uint64_t)buf[3] << 24) |
          ((uint64_t)buf[2] << 16) |
          ((uint64_t)buf[1] << 8) |
          (uint64_t)buf[0];

  _USER_ROW.set(fuses);
}

void Adafruit_DAP_SAM::fuseWrite()
{
  uint64_t fuses = _USER_ROW.get();
  uint8_t buf[USER_ROW_SIZE] = {(uint8_t)fuses,
      (uint8_t)(fuses >> 8),
      (uint8_t)(fuses >> 16),
      (uint8_t)(fuses >> 24),
      (uint8_t)(fuses >> 32),
      (uint8_t)(fuses >> 40),
      (uint8_t)(fuses >> 48),
      (uint8_t)(fuses >> 56)
    };

  dap_write_word(NVMCTRL_CTRLB, 0);
  dap_write_word(NVMCTRL_ADDR, USER_ROW_ADDR >> 1);
  dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_EAR);
  while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));

  dap_write_block(USER_ROW_ADDR, buf, USER_ROW_SIZE);
}
#endif //INCLUDE_DAP