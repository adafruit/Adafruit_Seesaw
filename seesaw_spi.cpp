/*!
 * @file seesaw_spi.cpp
 * Experimental SPI-controller bridge. No display framebuffer is allocated.
 */
#include "seesaw_spi.h"

/*! @brief Construct a bridge using an I2C bus.
 * @param wire I2C bus, or NULL for the default Wire instance.
 */
seesaw_SPI::seesaw_SPI(TwoWire *wire) : Adafruit_seesaw(wire) {}

/*! @brief Wait for queued SPI work to finish and capture its status.
 * @param status Four-byte device status response.
 * @return True on completion without a device or transport error.
 */
bool seesaw_SPI::waitReady(uint8_t status[4]) {
  uint32_t started = millis();
  do {
    if (!read(SEESAW_SPI_BASE, SEESAW_SPI_STATUS, status, 4)) {
      _lastSPIError = SEESAW_SPI_HOST_IO;
      return false;
    }
    if (!(status[0] & SEESAW_SPI_BUSY)) {
      _lastSPIError = status[1];
      return _lastSPIError == SEESAW_SPI_OK;
    }
    delay(1);
  } while (millis() - started < SEESAW_SPI_TIMEOUT_MS);
  _lastSPIError = SEESAW_SPI_HOST_TIMEOUT;
  return false;
}

/*! @brief Configure the controller after the inherited begin() succeeds.
 * @param frequency Maximum requested SPI clock, 187500 to 24000000 Hz on C011.
 * The actual clock is rounded down to a hardware divisor; query it separately.
 * @param mode SPI mode 0, 1, 2, or 3.
 * @param lsbFirst True for least-significant-bit first; false for MSB first.
 * @return True if the device accepted the configuration.
 */
bool seesaw_SPI::beginSPI(uint32_t frequency, uint8_t mode, bool lsbFirst) {
  if (frequency < 187500 || frequency > 24000000 || mode > 3) {
    _lastSPIError = SEESAW_SPI_BAD_CONFIG;
    return false;
  }
  if (!(getOptions() & (1UL << SEESAW_SPI_BASE))) {
    _lastSPIError = SEESAW_SPI_BAD_STATE;
    return false;
  }
  if (!abortSPI(true))
    return false;
  uint8_t config[] = {mode,
                      (uint8_t)lsbFirst,
                      (uint8_t)(frequency >> 24),
                      (uint8_t)(frequency >> 16),
                      (uint8_t)(frequency >> 8),
                      (uint8_t)frequency};
  if (!write(SEESAW_SPI_BASE, SEESAW_SPI_CONFIG, config, sizeof(config))) {
    _lastSPIError = SEESAW_SPI_HOST_IO;
    return false;
  }
  uint8_t status[4];
  if (!waitReady(status))
    return false;
  if (!(status[0] & SEESAW_SPI_READY)) {
    _lastSPIError = SEESAW_SPI_BAD_STATE;
    return false;
  }
  return true;
}

/*! @brief Best-effort CS/pin release after a failure, retaining its error code.
 * @return Always false for the calling operation.
 */
bool seesaw_SPI::transferFailed() {
  uint8_t error = _lastSPIError;
  abortSPI(true);
  _lastSPIError = error;
  return false;
}

/*! @brief Transfer an arbitrary-length buffer using bounded I2C chunks.
 * @param tx Transmit bytes, or NULL to send 0xFF while reading.
 * @param rx Receive destination, or NULL to discard received data.
 * @param length Number of bytes; zero can assert/release CS without clocking.
 * @param beginTransaction Assert CS before the first chunk. False continues
 * a transaction started by an earlier call.
 * @param endTransaction Release CS after the last chunk. False keeps CS low
 * so callers can change a separate D/C GPIO or send another buffer.
 * @return True if every chunk completed. Failure attempts to release CS/pins;
 * a disconnected I2C bus can prevent cleanup, so restore communication before
 * assuming the peripheral is idle. Inspect getLastSPIError() for the cause.
 */
bool seesaw_SPI::transfer(const uint8_t *tx, uint8_t *rx, size_t length,
                          bool beginTransaction, bool endTransaction) {
  size_t offset = 0;
  do {
    uint8_t status[4];
    if (!waitReady(status))
      return transferFailed();
    uint8_t sequence = status[3];
    uint8_t count =
        (uint8_t)min(length - offset, (size_t)SEESAW_SPI_CHUNK_SIZE);
    uint8_t packet[SEESAW_SPI_CHUNK_SIZE + 1];
    packet[0] = rx ? 0 : SEESAW_SPI_DISCARD_RX;
    if (offset == 0 && beginTransaction)
      packet[0] |= SEESAW_SPI_BEGIN;
    if (offset + count == length && endTransaction)
      packet[0] |= SEESAW_SPI_END;
    for (uint8_t i = 0; i < count; i++)
      packet[i + 1] = tx ? tx[offset + i] : 0xFF;
    if (!write(SEESAW_SPI_BASE, SEESAW_SPI_TRANSFER, packet, count + 1)) {
      _lastSPIError = SEESAW_SPI_HOST_IO;
      return transferFailed();
    }
    if (!waitReady(status))
      return transferFailed();
    if (status[3] != (uint8_t)(sequence + 1) || status[2] != (rx ? count : 0)) {
      _lastSPIError = SEESAW_SPI_HOST_PROTOCOL;
      return transferFailed();
    }
    if (rx && count &&
        !read(SEESAW_SPI_BASE, SEESAW_SPI_READ, rx + offset, count)) {
      _lastSPIError = SEESAW_SPI_HOST_IO;
      return transferFailed();
    }
    offset += count;
  } while (offset < length);
  return true;
}

/*! @brief Abort the current transaction and clear the device error/RX state.
 * @param releasePins True also disables SPI and returns its pins to inputs.
 * @return True when the abort completed successfully.
 */
bool seesaw_SPI::abortSPI(bool releasePins) {
  uint8_t value = releasePins;
  if (!write(SEESAW_SPI_BASE, SEESAW_SPI_ABORT, &value, 1)) {
    _lastSPIError = SEESAW_SPI_HOST_IO;
    return false;
  }
  uint8_t status[4];
  return waitReady(status);
}

/*! @brief Read the actual divided SPI clock from the peripheral.
 * @return Clock in hertz, or 0 when disabled or the read failed.
 */
uint32_t seesaw_SPI::getSPIClockFrequency() {
  uint8_t data[4];
  if (!read(SEESAW_SPI_BASE, SEESAW_SPI_CLOCK, data, sizeof(data))) {
    _lastSPIError = SEESAW_SPI_HOST_IO;
    return 0;
  }
  return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
         ((uint32_t)data[2] << 8) | data[3];
}

/*! @brief Report the last SPI operation's device or host-side error.
 * @return A SEESAW_SPI_* error code, or SEESAW_SPI_OK.
 */
uint8_t seesaw_SPI::getLastSPIError() { return _lastSPIError; }
