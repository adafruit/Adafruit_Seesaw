// Adafruit experimental STM32C011 seesaw I2C-to-SPI bridge.
// For this loopback demonstration, connect PA7/MOSI to PA6/MISO on the
// C011 breakout. Its SPI pins are 3.3 V outputs, not 5 V power connections.
// PA4 is chip-select and PA5 is clock. Use separate GPIOs for display D/C,
// reset, and BUSY when adapting the bridge to an e-paper display.
#include <seesaw_spi.h>

seesaw_SPI bridge;

void setup() {
  Serial.begin(115200);
  // Wait for Serial Monitor on native-USB boards. Remove this for standalone use.
  while (!Serial) delay(10);
  delay(250);
  Serial.println("Adafruit C011 seesaw SPI bridge");
  if (!bridge.begin() || !bridge.beginSPI(1000000, 0)) {
    Serial.println("SPI bridge did not start. Check the wiring and SPI firmware.");
    while (true) delay(10);
  }
  Serial.print("Actual SPI clock: ");
  Serial.print(bridge.getSPIClockFrequency());
  Serial.println(" Hz");
}

void loop() {
  uint8_t sent[] = {0x00, 0x55, 0xAA, 0xFF};
  uint8_t received[sizeof(sent)];
  // Larger buffers are chunked automatically. Pass NULL for received to write
  // only. The last two optional arguments keep CS low across separate calls:
  // transfer(command, NULL, commandLength, true, false), change D/C, then
  // transfer(data, NULL, dataLength, false, true).
  if (bridge.transfer(sent, received, sizeof(sent))) {
    Serial.print("Received:");
    for (uint8_t value : received) {
      Serial.print(' ');
      Serial.print(value, HEX);
    }
    Serial.println();
  } else {
    Serial.print("SPI transfer error: ");
    Serial.println(bridge.getLastSPIError());
    bridge.beginSPI(1000000, 0);
  }
  delay(1000);
}
