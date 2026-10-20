# Adafruit_Seesaw [![Build Status](https://github.com/adafruit/Adafruit_Seesaw/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit_Seesaw/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](http://adafruit.github.io/Adafruit_Seesaw/html/index.html)

Arduino driver for seesaw multi-use chip

## NeoTrellis and NeoKey callback migration

NeoTrellis and NeoKey callbacks now use the declared `TrellisCallback` and
`NeoKey1x4Callback` types: a function that takes `keyEvent` and returns `void`.
This applies to both single-board and multi-board classes.

Older sketches that declare `TrellisCallback blink(keyEvent evt)` or
`NeoKey1x4Callback blink(keyEvent evt)` must change the return type to `void`
and remove the callback's `return 0;`. Keep the `registerCallback()` calls
unchanged. For example:

```cpp
void blink(keyEvent evt) {
  // Handle the key event here; no return value is needed.
}
```

## STM32 peripherals

This driver recognizes the STM32 targets in
[Adafruit seesaw Peripheral 2.0.0](https://github.com/adafruit/Adafruit_seesawPeripheral/releases/tag/2.0.0):
C011 (`0x90`), C031 (`0x91`), C051 (`0x92`), and G071 (`0x93`).
GPIO, ADC, and PWM use the peripheral's digital pin indices. Use the target's
pin map in the firmware README; enabled features can reserve pins, and timer
channels and frequencies can be shared between outputs.

`analogRead()` retains its documented 0–1023 range by shifting each STM32's
native 12-bit register value by two bits. The persistent I2C-address byte is at
`0xFF` on all four targets. `getTemp()` reads the internal die temperature when
the firmware has ADC enabled; it is not available in spectrum-only firmware.
`setI2CAddr()` requires EEPROM-enabled firmware and inactive address straps.
It verifies the saved address and resets the STM32 at its old address before
reconnecting at the new one.

The experimental `seesaw_SPI` class adds an I2C-to-SPI controller bridge for
SPI-enabled STM32 firmware. Call `begin()`, then `beginSPI(frequency, mode,
lsbFirst)`. `transfer(tx, rx, length, begin, end)` splits buffers into 29-byte
chunks and can hold CS across calls; null TX sends 0xFF and null RX discards
received bytes. `getSPIClockFrequency()` reports the actual divided clock.
`abortSPI()` releases CS and the four SPI pins. See
`examples/communication/SPI_bridge` for electrical loopback wiring.
Clock requests range from 187500 to 24000000 Hz on C011/C031/C051, or 250000 to
32000000 Hz on G071. The firmware selects the SPI pins.
This is not a drop-in `SPIClass` or e-paper driver. The bridge uses module 0x13;
its companion firmware documents the register protocol.

Check out the [documentation](https://adafruit.github.io/Adafruit_Seesaw/html/class_adafruit__seesaw.html) for a listing and explanation of the available methods!
