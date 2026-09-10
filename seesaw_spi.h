/*!
 * @file seesaw_spi.h
 * Experimental seesaw SPI-controller bridge for chunked I2C transfers.
 */
#ifndef SEESAW_SPI_H
#define SEESAW_SPI_H

#include "Adafruit_seesaw.h"

/** SPI controller registers. */
enum {
  SEESAW_SPI_STATUS = 0x00,   ///< Flags, error, RX length, completion sequence.
  SEESAW_SPI_CONFIG = 0x01,   ///< Mode, order, requested frequency (six bytes).
  SEESAW_SPI_TRANSFER = 0x02, ///< Flags and up to 29 transmit bytes.
  SEESAW_SPI_READ = 0x03,     ///< Consume the completed receive buffer.
  SEESAW_SPI_ABORT = 0x04,    ///< Abort transaction; optionally release pins.
  SEESAW_SPI_CLOCK = 0x05,    ///< Read actual SPI clock frequency in hertz.
};
/** SPI transfer controls. */
enum {
  SEESAW_SPI_BEGIN = 0x01,      ///< Assert chip-select before this chunk.
  SEESAW_SPI_END = 0x02,        ///< Release chip-select after this chunk.
  SEESAW_SPI_DISCARD_RX = 0x04, ///< Do not retain receive data.
};
/** SPI status flag bits. */
enum {
  SEESAW_SPI_BUSY = 0x01,     ///< Queued or executing work.
  SEESAW_SPI_READY = 0x02,    ///< Controller configured.
  SEESAW_SPI_SELECTED = 0x04, ///< Chip-select is low.
  SEESAW_SPI_RX_READY = 0x08, ///< Receive data available.
  SEESAW_SPI_ERROR = 0x80,    ///< Sticky device error.
};
/** SPI errors, including host-side transport failures. */
enum {
  SEESAW_SPI_OK = 0,         ///< No error.
  SEESAW_SPI_BAD_CONFIG = 1, ///< Unsupported configuration.
  SEESAW_SPI_BAD_STATE = 2,  ///< Disabled controller or invalid CS sequence.
  SEESAW_SPI_QUEUE_OVERFLOW = 3,   ///< Firmware command queue filled.
  SEESAW_SPI_UNREAD_DATA = 4,      ///< Previous receive data was not consumed.
  SEESAW_SPI_TRANSFER_FAILED = 5,  ///< Hardware SPI failure or timeout.
  SEESAW_SPI_INVALID_COMMAND = 6,  ///< Invalid register/flags/payload.
  SEESAW_SPI_PIN_CONFLICT = 7,     ///< Pins reserved by another feature.
  SEESAW_SPI_HOST_PROTOCOL = 0xFD, ///< Unexpected response length or sequence.
  SEESAW_SPI_HOST_TIMEOUT = 0xFE,  ///< Completion deadline expired.
  SEESAW_SPI_HOST_IO = 0xFF,       ///< I2C read/write failed.
};
const uint16_t SEESAW_SPI_TIMEOUT_MS =
    250; ///< Per-command completion deadline.
const uint8_t SEESAW_SPI_CHUNK_SIZE =
    29; ///< Payload fitting a classic Wire packet.

/** Chunked hardware SPI controller accessed through seesaw I2C registers.
 * Call inherited begin() before beginSPI(). C011 uses PA4 CS, PA5 SCK,
 * PA6 MISO, and PA7 MOSI; D/C, reset, and BUSY use separate GPIOs.
 */
class seesaw_SPI : public Adafruit_seesaw {
public:
  seesaw_SPI(TwoWire *wire = NULL);
  bool beginSPI(uint32_t frequency = 1000000, uint8_t mode = 0,
                bool lsbFirst = false);
  bool transfer(const uint8_t *tx, uint8_t *rx, size_t length,
                bool beginTransaction = true, bool endTransaction = true);
  bool abortSPI(bool releasePins = true);
  uint32_t getSPIClockFrequency();
  uint8_t getLastSPIError();

private:
  bool waitReady(uint8_t status[4]);
  bool transferFailed();
  uint8_t _lastSPIError =
      SEESAW_SPI_OK; ///< Last operation's device/host error.
};
#endif
