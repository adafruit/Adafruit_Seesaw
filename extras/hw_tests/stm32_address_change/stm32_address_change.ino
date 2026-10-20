// Adafruit STM32 seesaw address-change hardware test.
// Use an EEPROM-enabled peripheral at 0x49 with inactive address straps.
// Connect only that target to the host's level-shifted I2C bus.
// Back up the peripheral's flash before testing. This test changes its saved
// address, then restores it; a failure requires recovery before another test.
#include <Adafruit_seesaw.h>

Adafruit_seesaw seesaw;
const uint8_t originalAddress = 0x49;
const uint8_t testAddress = 0x4A;
const uint8_t addressLocation = 0xFF;

void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.print(F("Adafruit STM32 seesaw address-change test\n\r"));
  Serial.print(F("Send t to test and restore the saved address.\n\r"));
}

void loop() {
  if (!Serial.available() || Serial.read() != 't')
    return;

  if (!seesaw.begin(originalAddress))
    halt(F("Initial begin failed"));
  Serial.print(F("Begin at 0x49 succeeded\n\r"));

  uint8_t savedAddress = seesaw.EEPROMRead8(addressLocation);
  if (savedAddress != originalAddress && savedAddress != 0xFF)
    halt(F("Unexpected saved address; no writes attempted"));

  seesaw.setI2CAddr(testAddress);
  if (!acknowledges(testAddress) || acknowledges(originalAddress))
    halt(F("Bus address did not move to 0x4A"));
  if (!seesaw.begin(testAddress, -1, false) ||
      seesaw.getI2CAddr() != testAddress)
    halt(F("New-address identification or EEPROM readback failed"));
  Serial.print(F("Only 0x4A responds; identification and EEPROM passed\n\r"));

  if (!seesaw.SWReset())
    halt(F("Software reset write failed"));
  delay(20);
  if (!seesaw.begin(testAddress, -1, false))
    halt(F("Address did not survive software reset"));
  Serial.print(F("Software reset preserved 0x4A\n\r"));

  seesaw.setI2CAddr(originalAddress);
  if (!acknowledges(originalAddress) || acknowledges(testAddress))
    halt(F("Bus address did not return to 0x49"));
  Serial.print(F("Only 0x49 responds again\n\r"));

  seesaw.EEPROMWrite8(addressLocation, savedAddress);
  delay(250);
  if (!seesaw.SWReset())
    halt(F("Final software reset write failed"));
  delay(20);
  if (!seesaw.begin(originalAddress, -1, false) ||
      seesaw.EEPROMRead8(addressLocation) != savedAddress)
    halt(F("Original saved address did not restore"));
  Serial.print(F("Original EEPROM byte restored; all checks passed\n\r"));
}

bool acknowledges(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

void halt(const __FlashStringHelper *message) {
  Serial.print(message);
  Serial.print(F("\n\rStopped. Recover the saved firmware before retrying.\n\r"));
  while (true)
    delay(10);
}
