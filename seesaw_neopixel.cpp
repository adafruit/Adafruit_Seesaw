/*-------------------------------------------------------------------------
  Arduino library to control a wide variety of WS2811- and WS2812-based RGB
  LED devices such as Adafruit FLORA RGB Smart Pixels and NeoPixel strips.
  Currently handles 400 and 800 KHz bitstreams on 8, 12 and 16 MHz ATmega
  MCUs, with LEDs wired for various color orders.  Handles most output pins
  (possible exception with upper PORT registers on the Arduino Mega).
  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
  contributions by PJRC, Michael Miller and other members of the open
  source community.
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!
  -------------------------------------------------------------------------
  This file is part of the Adafruit NeoPixel library.
  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.
  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "seesaw_neopixel.h"
#include "Adafruit_seesaw.h"

// Constructor when length, pin and type are known at compile-time:
seesaw_NeoPixel::seesaw_NeoPixel(uint16_t n, uint8_t p, neoPixelType t,
                                 TwoWire *Wi)
    : Adafruit_seesaw(Wi), begun(false), numLEDs(n), pin(p), brightness(0),
      pixels(NULL), endTime(0), type(t) {}

// via Michael Vogt/neophob: empty constructor is used when strand length
// isn't known at compile-time; situations where program config might be
// read from internal flash memory or an SD card, or arrive via serial
// command.  If using this constructor, MUST follow up with updateType(),
// updateLength(), etc. to establish the strand type, length and pin number!
seesaw_NeoPixel::seesaw_NeoPixel(TwoWire *Wi)
    : Adafruit_seesaw(Wi),
#ifdef NEO_KHZ400
      is800KHz(true),
#endif
      begun(false), numLEDs(0), numBytes(0), pin(-1), brightness(0),
      pixels(NULL), rOffset(1), gOffset(0), bOffset(2), wOffset(1), endTime(0) {
}

seesaw_NeoPixel::~seesaw_NeoPixel() {
  if (pixels)
    free(pixels);
}

bool seesaw_NeoPixel::begin(uint8_t addr, int8_t flow) {
  if (!Adafruit_seesaw::begin(addr, flow))
    return false;

  updateType(type);
  updateLength(numLEDs);
  setPin(pin);

  return true;
}

void seesaw_NeoPixel::updateLength(uint16_t n) {
  if (pixels)
    free(pixels); // Free existing data (if any)

  // Allocate new data -- note: ALL PIXELS ARE CLEARED
  numBytes = n * ((wOffset == rOffset) ? 3 : 4);
  if ((pixels = (uint8_t *)malloc(numBytes))) {
    memset(pixels, 0, numBytes);
    numLEDs = n;
  } else {
    numLEDs = numBytes = 0;
  }

  uint8_t buf[] = {(uint8_t)(numBytes >> 8), (uint8_t)(numBytes & 0xFF)};
  this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF_LENGTH, buf, 2);
}

void seesaw_NeoPixel::updateType(neoPixelType t) {
  boolean oldThreeBytesPerPixel = (wOffset == rOffset); // false if RGBW

  wOffset = (t >> 6) & 0b11; // See notes in header file
  rOffset = (t >> 4) & 0b11; // regarding R/G/B/W offsets
  gOffset = (t >> 2) & 0b11;
  bOffset = t & 0b11;
  is800KHz = (t < 256); // 400 KHz flag is 1<<8

  this->write8(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_SPEED, is800KHz);

  // If bytes-per-pixel has changed (and pixel data was previously
  // allocated), re-allocate to new size.  Will clear any data.
  if (pixels) {
    boolean newThreeBytesPerPixel = (wOffset == rOffset);
    if (newThreeBytesPerPixel != oldThreeBytesPerPixel)
      updateLength(numLEDs);
  }
}

void seesaw_NeoPixel::show(void) {

  if (!pixels)
    return;

  // Data latch = 300+ microsecond pause in the output stream.  Rather than
  // put a delay at the end of the function, the ending time is noted and
  // the function will simply hold off (if needed) on issuing the
  // subsequent round of data until the latch time has elapsed.  This
  // allows the mainline code to start generating the next frame of data
  // rather than stalling for the latch.
  while (!canShow())
    ;

  this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_SHOW, NULL, 0);

  endTime = micros(); // Save EOD time for latch on next call
}

// Set the output pin number
void seesaw_NeoPixel::setPin(uint8_t p) {
  this->write8(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_PIN, p);
  pin = p;
}

// Set pixel color from separate R,G,B components:
void seesaw_NeoPixel::setPixelColor(uint16_t n, uint8_t r, uint8_t g,
                                    uint8_t b) {

  if (n < numLEDs) {
    if (brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    uint8_t *p;
    if (wOffset == rOffset) { // Is an RGB-type strip
      p = &pixels[n * 3];     // 3 bytes per pixel
    } else {                  // Is a WRGB-type strip
      p = &pixels[n * 4];     // 4 bytes per pixel
      p[wOffset] = 0;         // But only R,G,B passed -- set W to 0
    }
    p[rOffset] = r; // R,G,B always stored
    p[gOffset] = g;
    p[bOffset] = b;

    uint8_t len = (wOffset == rOffset ? 3 : 4);
    uint16_t offset = n * len;

    uint8_t writeBuf[6];
    writeBuf[0] = (offset >> 8);
    writeBuf[1] = offset;
    memcpy(&writeBuf[2], p, len);

    this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF, writeBuf, len + 2);
  }
}

void seesaw_NeoPixel::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b,
                                    uint8_t w) {

  if (n < numLEDs) {
    if (brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
      w = (w * brightness) >> 8;
    }
    uint8_t *p;
    if (wOffset == rOffset) { // Is an RGB-type strip
      p = &pixels[n * 3];     // 3 bytes per pixel (ignore W)
    } else {                  // Is a WRGB-type strip
      p = &pixels[n * 4];     // 4 bytes per pixel
      p[wOffset] = w;         // Store W
    }
    p[rOffset] = r; // Store R,G,B
    p[gOffset] = g;
    p[bOffset] = b;

    uint8_t len = (wOffset == rOffset ? 3 : 4);
    uint16_t offset = n * len;

    uint8_t writeBuf[6];
    writeBuf[0] = (offset >> 8);
    writeBuf[1] = offset;
    memcpy(&writeBuf[2], p, len);

    this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF, writeBuf, len + 2);
  }
}

// Set pixel color from 'packed' 32-bit RGB color:
void seesaw_NeoPixel::setPixelColor(uint16_t n, uint32_t c) {
  if (n < numLEDs) {
    uint8_t *p, r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c;
    if (brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    if (wOffset == rOffset) {
      p = &pixels[n * 3];
    } else {
      p = &pixels[n * 4];
      uint8_t w = (uint8_t)(c >> 24);
      p[wOffset] = brightness ? ((w * brightness) >> 8) : w;
    }
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;

    uint8_t len = (wOffset == rOffset ? 3 : 4);
    uint16_t offset = n * len;

    uint8_t writeBuf[6];
    writeBuf[0] = (offset >> 8);
    writeBuf[1] = offset;
    memcpy(&writeBuf[2], p, len);

    this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF, writeBuf, len + 2);
  }
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t seesaw_NeoPixel::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Convert separate R,G,B,W into packed 32-bit WRGB color.
// Packed format is always WRGB, regardless of LED strand color order.
uint32_t seesaw_NeoPixel::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t seesaw_NeoPixel::getPixelColor(uint16_t n) const {
  if (n >= numLEDs)
    return 0; // Out of bounds, return no color.

  uint8_t *p;

  if (wOffset == rOffset) { // Is RGB-type device
    p = &pixels[n * 3];
    if (brightness) {
      // Stored color was decimated by setBrightness().  Returned value
      // attempts to scale back to an approximation of the original 24-bit
      // value used when setting the pixel color, but there will always be
      // some error -- those bits are simply gone.  Issue is most
      // pronounced at low brightness levels.
      return (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
             (((uint32_t)(p[gOffset] << 8) / brightness) << 8) |
             ((uint32_t)(p[bOffset] << 8) / brightness);
    } else {
      // No brightness adjustment has been made -- return 'raw' color
      return ((uint32_t)p[rOffset] << 16) | ((uint32_t)p[gOffset] << 8) |
             (uint32_t)p[bOffset];
    }
  } else { // Is RGBW-type device
    p = &pixels[n * 4];
    if (brightness) { // Return scaled color
      return (((uint32_t)(p[wOffset] << 8) / brightness) << 24) |
             (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
             (((uint32_t)(p[gOffset] << 8) / brightness) << 8) |
             ((uint32_t)(p[bOffset] << 8) / brightness);
    } else { // Return raw color
      return ((uint32_t)p[wOffset] << 24) | ((uint32_t)p[rOffset] << 16) |
             ((uint32_t)p[gOffset] << 8) | (uint32_t)p[bOffset];
    }
  }
}

// Returns pointer to pixels[] array.  Pixel data is stored in device-
// native format and is not translated here.  Application will need to be
// aware of specific pixel data format and handle colors appropriately.
uint8_t *seesaw_NeoPixel::getPixels(void) const { return pixels; }

uint16_t seesaw_NeoPixel::numPixels(void) const { return numLEDs; }

void seesaw_NeoPixel::clear() {
  // Clear local pixel buffer
  memset(pixels, 0, numBytes);

  // Now clear the pixels on the seesaw.
  // Max write buffer size is I2C buffer limit (32 bytes) minus sizes
  // for seesaw regHigh (1 byte) and regLow (1 byte).
  uint8_t writeBuf[30];
  memset(writeBuf, 0, 30);
  for (uint8_t offset = 0; offset < numBytes; offset += 32 - 4) {
    writeBuf[0] = (offset >> 8);
    writeBuf[1] = offset;
    this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF, writeBuf, 30);
  }
}

void seesaw_NeoPixel::setBrightness(uint8_t b) { brightness = b; }

// Set colors for a range of LEDs from a colors array, starting from firstLed up to length LEDs.
// Minimizes bytes written via I2C by using best method (bulk write vs. setPixelColor()) for given length and bitmask.
// If colorsLength < length, the index into the array will wrap around, allowing for a simple definition of repeating patterns. colorsLength must be > 0;
// bitMask can be used, to omit updating the color, i.e. the current LED color will be rewritten to the seesaw.
void seesaw_NeoPixel::setPixelRangeColors(uint16_t firstLed, uint16_t length, const uint32_t* colors, uint16_t colorsLength, const BitMask* bitMask) {
  bool isMaskInvalid = (bitMask != nullptr) && (bitMask->maxLength() < (1 + ((length - 1)/32)));
  if (firstLed + length > numLEDs || length == 0 || isMaskInvalid) {
    return;
  }

  uint8_t colorIndex = 0;
  uint8_t r, g, b, w;
  uint8_t stride = (wOffset == rOffset) ? 3 : 4;

  bool bulkWrite = isBulkWriteFastest(length, bitMask);

  for(uint8_t i = 0; i < length; ++i) {
    if (bitMask == nullptr || bitMask->readBit(i)) {
      if(bulkWrite) {
        r = (uint8_t)(colors[colorIndex] >> 16);
        g = (uint8_t)(colors[colorIndex] >> 8);
        b = (uint8_t)colors[colorIndex];

        if (brightness) { // See notes in setBrightness()
          r = (r * brightness) >> 8;
          g = (g * brightness) >> 8;
          b = (b * brightness) >> 8;
        }

        w = 0;
        if (stride == 4) {
          w = (uint8_t)(colors[colorIndex] >> 24);
        }

        if (stride == 4) {
          pixels[(firstLed+i)*stride + wOffset] = brightness ? ((w * brightness) >> 8) : w;
        }

        pixels[(firstLed+i)*stride + rOffset] = r;
        pixels[(firstLed+i)*stride + gOffset] = g;
        pixels[(firstLed+i)*stride + bOffset] = b;
      } else {
        setPixelColor(firstLed + i, colors[colorIndex]);
      }
      colorIndex = (colorIndex + 1) % colorsLength;
    }
  }
  if(bulkWrite) {
    uint8_t byte_offset = firstLed*stride;

    uint8_t bufIndex = 2;
    uint8_t writeBuf[30];

    for(int i = firstLed; i < firstLed + length; ++i) {
      for(int j = 0; j < stride; ++j) {
        writeBuf[bufIndex++] = pixels[i*stride + j];
      }

      if((i == firstLed + length - 1) || bufIndex >= 28) {
        writeBuf[0] = (byte_offset >> 8);
        writeBuf[1] = byte_offset;
        this->write(SEESAW_NEOPIXEL_BASE, SEESAW_NEOPIXEL_BUF, writeBuf, bufIndex);
        byte_offset += bufIndex - 2;
        bufIndex = 2;
      }
    }
  }
}

bool seesaw_NeoPixel::isBulkWriteFastest(uint16_t length, const BitMask* bitMask) {
  if(bitMask == nullptr) {
    return true;
  }

  uint8_t stride = (wOffset == rOffset) ? 3 : 4;

  uint16_t nOnes = bitMask->count(true);
  uint8_t nWrites = (1+length*stride/28);
  // 4 bytes overhead per write
  uint16_t bulkBytes = stride*length + 4*nWrites;

  return bulkBytes < 7*nOnes;
}
