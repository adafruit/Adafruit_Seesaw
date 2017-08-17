#include "seesaw.h"
#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
#define FILENAME "fw.bin"

#define BUFSIZE 256 //don't change

//create a seesaw with m0 DAP support
dap_m0p ss;

//create the target options
//TODO: set some actual options here
options_t g_target_options;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  while(!Serial);
  
  if(!ss.begin()){
    Serial.println("ERROR!");
    return;
  }
  else Serial.println("seesaw started");

    // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  File dataFile = SD.open(FILENAME);
  uint8_t buf[256];

  if(dataFile){
    ss.dap_disconnect();
    ss.dap_get_debugger_info();
    ss.dap_connect();
    ss.dap_transfer_configure(0, 128, 128);
    ss.dap_swd_configure(0);
    ss.dap_reset_link();
    ss.dap_swj_clock(DAP_FREQ);
    ss.dap_target_prepare();
  
    ss.select(&g_target_options);
  
    Serial.print("Erasing... ");
    ss.erase();
    Serial.println(" done.");
  
    Serial.print("Programming... ");
    uint32_t addr = ss.program_start();

    while (dataFile.available()) {
      dataFile.read(buf, 256);
      addr = ss.program(addr, buf);
    }
    dataFile.close();
    
    Serial.println(" done.");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
    return;
  }
  ss.deselect();
  ss.dap_disconnect();
}

void loop() {
  //blink led on the host to show we're done
  digitalWrite(13, HIGH);
  delay(500); 
  digitalWrite(13, LOW);
  delay(500);  
}