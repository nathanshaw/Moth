/**
   Moth Sonic Art Framework Firmware

   Written by Nathan Villicana-Shaw in 2019

   Inspired by the Marlin 3D Printer Firmware

*/
// todo need logic for which libraries to include
#include "Configuration.h"

#if FIRMWARE_MODE == CICADA_MODE
#include "ModeCicada.h"
#elif FIRMWARE_MODE == PITCH_MODE
#include "ModePitch.h"
#endif

#include "PrintUtils.h"
// that is all for this file

// desired classes
// - neoP (initalised with mode specific data)
// lux (initalised with mode specific data)
// audio engine (should be shared with Lyre?)
// datalogging (should be shared with Lyre)
// mode (for different installations of the Moth mode)

///////////////////////////////////////////////////////////////////////
//                    Setup and Main Loops
///////////////////////////////////////////////////////////////////////

void setup() {
  mothSetup();
}

void loop() {
  // this needs to stay in the faster main loop
  mothLoop();
}
