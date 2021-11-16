#include "seesaw_spectrum.h"

/**************************************************************************/
/*!
  @brief  Pull latest audio spectrum data from device.
*/
/**************************************************************************/
void seesaw_Audio_Spectrum::getData(void) {
  read(SEESAW_SPECTRUM_BASE, SEESAW_SPECTRUM_RESULTS_LOWER, bins, 32, 0);
  read(SEESAW_SPECTRUM_BASE, SEESAW_SPECTRUM_RESULTS_UPPER, &bins[32], 32, 0);
}

/**************************************************************************/
/*!
  @brief  Set the audio sampling rate.
  @param  value  Sampling rate index, 0-31. Values outside this range
                 will be clipped.
*/
/**************************************************************************/
void seesaw_Audio_Spectrum::setRate(uint8_t index) {
  write8(SEESAW_SPECTRUM_BASE, SEESAW_SPECTRUM_RATE, index);
}

/**************************************************************************/
/*!
  @brief   Query the current audio sampling rate.
  @return  Sampling rate index, 0-31.
*/
/**************************************************************************/
uint8_t seesaw_Audio_Spectrum::getRate(void) const {
  return read8(SEESAW_SPECTRUM_BASE, SEESAW_SPECTRUM_RATE);
}
