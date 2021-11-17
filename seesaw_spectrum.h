#ifndef _SEESAW_SPECTRUM_H
#define _SEESAW_SPECTRUM_H

#include "Adafruit_seesaw.h"

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for seesaw audio spectrum
             interface
*/
/**************************************************************************/
class seesaw_Audio_Spectrum : public Adafruit_seesaw {
public:
  /**************************************************************************/
  /*!
    @brief  seesaw_Audio_Spectrum class constructor.
    @param  ss  Corresponding seesaw object this is working through.
  */
  /**************************************************************************/
  seesaw_Audio_Spectrum(TwoWire *Wi=NULL) : Adafruit_seesaw(Wi) {}

  ~seesaw_Audio_Spectrum() {}

  bool begin(uint8_t addr = SEESAW_ADDRESS, int8_t flow = -1) {
    if (Adafruit_seesaw::begin(addr, flow)) {
      memset(bins, 0, sizeof bins);
      return true;
    }
    return false;
  }

  void getData(void); // Pull latest audio spectrum data from device
  void setRate(uint8_t index); // Set audio sampling rage 0-31
  uint8_t getRate(void); // Query audio sampling rate 0-31

  /**************************************************************************/
  /*!
    @brief   Get value of individual spectrum bin, as determined during
             most recent get_data() call.
    @param   idx  Spectrum bin index (0-63) to query.
    @return  Level: 0 (silent) to 255 (loudest) for bin.
  */
  /**************************************************************************/
  uint8_t getLevel(uint8_t idx) const { return bins[idx < 64 ? idx : 63]; }

  /**************************************************************************/
  /*!
    @brief   Get pointer to spectrum bin buffer directly. Use with caution!
    @return  uint8_t base pointer to 64 spectrum bins.
  */
  /**************************************************************************/
  uint8_t *getBuffer(void) const { return (uint8_t *)bins; }

private:
  uint8_t bins[64]; // Audio spectrum "bins"
};

#endif // end _SEESAW_SPECTRUM_H
