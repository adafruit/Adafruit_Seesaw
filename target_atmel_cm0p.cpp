/*
 * Copyright (c) 2013-2017, Alex Taradov <alex@taradov.com>
 * All rights reserved.
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

/*- Variables ---------------------------------------------------------------*/
device_t dap_m0p::devices[] =
{
  { 0x10040100, "SAM D09D14A",           8*1024,  128 },
  { 0x10010205, "SAM D21G18A",         256*1024, 4096 },
  { 0x10010305, "SAM D21G18A (Rev D)", 256*1024, 4096 },
  /*
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
  */
  { 0 },
};

static options_t options;

//-----------------------------------------------------------------------------
void dap_m0p::select(options_t *target_options)
{
  uint32_t dsu_did;

  // Stop the core
  dap_write_word(DHCSR, 0xa05f0003);
  dap_write_word(DEMCR, 0x00000001);
  dap_write_word(AIRCR, 0x05fa0004);

  dsu_did = dap_read_word(DSU_DID);

  for (device_t *device = devices; device->dsu_did > 0; device++)
  {
    if (device->dsu_did == dsu_did)
    {
      Serial.print("Target: ");
      Serial.println(device->name);

      target_device = *device;
      options = *target_options;

      //check_options(&options, device->flash_size, FLASH_ROW_SIZE);

      return;
    }
  }

  //error_exit("unknown target device (DSU_DID = 0x%08x)", dsu_did);
  perror_exit("unknown target device");
}

//-----------------------------------------------------------------------------
void dap_m0p::deselect(void)
{
  dap_write_word(DEMCR, 0x00000000);
  dap_write_word(AIRCR, 0x05fa0004);

  //free_options(&options);
}

//-----------------------------------------------------------------------------
void dap_m0p::erase(void)
{
  dap_write_word(DSU_CTRL_STATUS, 0x00001f00); // Clear flags
  dap_write_word(DSU_CTRL_STATUS, 0x00000010); // Chip erase
  delay(100);
  while (0 == (dap_read_word(DSU_CTRL_STATUS) & 0x00000100));
}

//-----------------------------------------------------------------------------
void dap_m0p::lock(void)
{
  dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_SSB); // Set Security Bit
}

//-----------------------------------------------------------------------------
void dap_m0p::program(void)
{
  uint32_t addr = FLASH_START + options.offset;
  uint32_t offs = 0;
  uint32_t number_of_rows;
  uint8_t *buf = options.file_data;
  uint32_t size = options.file_size;

  if (dap_read_word(DSU_CTRL_STATUS) & 0x00010000)
    perror_exit("device is locked, perform a chip erase before programming");

  number_of_rows = (size + FLASH_ROW_SIZE - 1) / FLASH_ROW_SIZE;

  dap_write_word(NVMCTRL_CTRLB, 0); // Enable automatic write

  for (uint32_t row = 0; row < number_of_rows; row++)
  {
    dap_write_word(NVMCTRL_ADDR, addr >> 1);

    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_UR); // Unlock Region
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));

    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_ER); // Erase Row
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));

    dap_write_block(addr, &buf[offs], FLASH_ROW_SIZE);

    addr += FLASH_ROW_SIZE;
    offs += FLASH_ROW_SIZE;

    Serial.print(".");
  }
}

//-----------------------------------------------------------------------------
uint32_t dap_m0p::program_start(void)
{

  if (dap_read_word(DSU_CTRL_STATUS) & 0x00010000)
    perror_exit("device is locked, perform a chip erase before programming");

  dap_write_word(NVMCTRL_CTRLB, 0); // Enable automatic write

  return FLASH_START + options.offset;
}

uint32_t dap_m0p::program(uint32_t addr, uint8_t *buf)
{
    dap_write_word(NVMCTRL_ADDR, addr >> 1);

    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_UR); // Unlock Region
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));

    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_ER); // Erase Row
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));
    dap_write_block(addr, buf, FLASH_ROW_SIZE);

    addr += FLASH_ROW_SIZE;

    Serial.print(".");
    return addr;
}


//-----------------------------------------------------------------------------
void dap_m0p::verify(void)
{
  uint32_t addr = FLASH_START + options.offset;
  uint32_t block_size;
  uint32_t offs = 0;
  uint8_t bufb[FLASH_ROW_SIZE];
  uint8_t *bufa = options.file_data;
  uint32_t size = options.file_size;

  if (dap_read_word(DSU_CTRL_STATUS) & 0x00010000)
    perror_exit("device is locked, unable to verify");

  //bufb = buf_alloc(FLASH_ROW_SIZE);

  while (size)
  {
    dap_read_block(addr, bufb, FLASH_ROW_SIZE);

    block_size = (size > FLASH_ROW_SIZE) ? FLASH_ROW_SIZE : size;

    for (int i = 0; i < (int)block_size; i++)
    {
      if (bufa[offs + i] != bufb[i])
      {
        /*
        verbose("\nat address 0x%x expected 0x%02x, read 0x%02x\n",
            addr + i, bufa[offs + i], bufb[i]);
        buf_free(bufb);
        */
        perror_exit("verification failed");
      }
    }

    addr += FLASH_ROW_SIZE;
    offs += FLASH_ROW_SIZE;
    size -= block_size;

    Serial.print(".");
  }

  //buf_free(bufb);
}

/*
//-----------------------------------------------------------------------------
void dap_m0p::read(void)
{
  uint32_t addr = FLASH_START + options.offset;
  uint32_t offs = 0;
  uint8_t *buf = options.file_data;
  uint32_t size = options.size;

  if (dap_read_word(DSU_CTRL_STATUS) & 0x00010000)
    perror_exit("device is locked, unable to read");

  while (size)
  {
    dap_read_block(addr, &buf[offs], FLASH_ROW_SIZE);

    addr += FLASH_ROW_SIZE;
    offs += FLASH_ROW_SIZE;
    size -= FLASH_ROW_SIZE;

    verbose(".");
  }

  save_file(options.name, buf, options.size);
}
*/


/*
//-----------------------------------------------------------------------------
void dap_m0p::fuse(void)
{
  uint8_t buf[USER_ROW_SIZE];
  bool read_all = (-1 == options.fuse_start);
  int size = (options.fuse_size < USER_ROW_SIZE) ?
      options.fuse_size : USER_ROW_SIZE;

  dap_read_block(USER_ROW_ADDR, buf, USER_ROW_SIZE);

  if (options.fuse_read)
  {
    if (options.fuse_name)
    {
      save_file(options.fuse_name, buf, USER_ROW_SIZE);
    }
    else if (read_all)
    {
      Serial.print("Fuses (user row): ");

      for (int i = 0; i < USER_ROW_SIZE; i++)
        Serial.print(buf[i], HEX);

      Serial.println();
    }
    else
    {
      uint32_t value = extract_value(buf, options.fuse_start,
          options.fuse_end);

      //message("Fuses: 0x%x (%d)\n", value, value);
    }
  }

  if (options.fuse_write)
  {
    if (options.fuse_name)
    {
      for (int i = 0; i < size; i++)
        buf[i] = options.fuse_data[i];
    }
    else
    {
      apply_value(buf, options.fuse_value, options.fuse_start,
          options.fuse_end);
    }

    dap_write_word(NVMCTRL_CTRLB, 0);
    dap_write_word(NVMCTRL_ADDR, USER_ROW_ADDR >> 1);
    dap_write_word(NVMCTRL_CTRLA, NVMCTRL_CMD_EAR);
    while (0 == (dap_read_word(NVMCTRL_INTFLAG) & 1));

    dap_write_block(USER_ROW_ADDR, buf, USER_ROW_SIZE);
  }

  if (options.fuse_verify)
  {
    dap_read_block(USER_ROW_ADDR, buf, USER_ROW_SIZE);

    if (options.fuse_name)
    {
      for (int i = 0; i < size; i++)
      {
        if (options.fuse_data[i] != buf[i])
        {
          message("fuse byte %d expected 0x%02x, got 0x%02x", i,
              options.fuse_data[i], buf[i]);
          perror_exit("fuse verification failed");
        }
      }
    }
    else if (read_all)
    {
      perror_exit("please specify fuse bit range for verification");
    }
    else
    {
      uint32_t value = extract_value(buf, options.fuse_start,
          options.fuse_end);

      if (options.fuse_value != value)
      {
        perror_exit("fuse verification failed: expected 0x%x (%u), got 0x%x (%u)",
            options.fuse_value, options.fuse_value, value, value);
      }
    }
  }
}
*/
