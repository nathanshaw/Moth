/**
   Moth Sonic Art Framework Firmware
   Written by Nathan Villicana-Shaw in 2019
   The runtime, boot, and all other configurations are found in the Configuration.h file
*/
#include "Configuration.h"

#include "ModeUtils.h"
#if FIRMWARE_MODE == CICADA_MODE
#include "ModeCicada.h"
#elif FIRMWARE_MODE == PITCH_MODE
#include "ModePitch.h"
#endif


void setup() {
  mainSetup();
}

void loop() {
  // this needs to stay in the faster main loop
  mainLoop();
}
