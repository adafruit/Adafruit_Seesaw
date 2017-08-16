#include "seesaw.h"

//create a seesaw with m0 DAP support
dap_m0p ss;

//create the target options
//TODO: set some actual options here
options_t g_target_options;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  
  if(!ss.begin()){
    Serial.println("ERROR!");
    while(1);
  }
  else Serial.println("seesaw started");
  
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

  /* TODO: should be able to program from an SD file
  Serial.print("Programming... ");
  ss.program();
  Serial.println(" done.");
  */
}

void loop() {
  //blink led on the host to show we're done
  digitalWrite(13, HIGH);
  delay(500); 
  digitalWrite(13, LOW);
  delay(500);  
}
