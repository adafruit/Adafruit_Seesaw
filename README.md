# Adafruit_Seesaw [![Build Status](https://github.com/adafruit/Adafruit_Seesaw/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit_Seesaw/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](http://adafruit.github.io/Adafruit_Seesaw/html/index.html)

Arduino driver for seesaw multi-use chip

## STM32C011 bring-up

The experimental C011 peripheral uses hardware ID `0x90`. GPIO indices 0–8 map
to PA0–PA8, 9 to PA11, 10 to PA12, and 15 to PC14. ADC and PWM use those same
indices; enabled peripheral features can reserve individual pins. PA12 has no
PWM output, and PC14 has no ADC input. PWM frequency is shared, with additional
timer-channel aliases documented by the peripheral firmware.

`analogRead()` retains its documented 0–1023 range by shifting the C011's native
12-bit register value by two bits. The persistent I2C-address byte is at `0xFF`.
The hardware ID remains provisional until the C011 firmware/host changes are
accepted together; this does not claim support in older released host libraries.

Check out the [documentation](https://adafruit.github.io/Adafruit_Seesaw/html/class_adafruit__seesaw.html) for a listing and explanation of the available methods!
