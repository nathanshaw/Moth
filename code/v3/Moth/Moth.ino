/**
 * Moth Sonic Art Framework Firmware
 * 
 * Written by Nathan Villicana-Shaw in 2019
 * 
 * Inspired by the Marlin 3D Printer Firmware
 * 
 */

 // #include "MothConfig.h"

 // todo need logic for which libraries to include
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "Neos.h" // has to be before elapsedmillis
#include <elapsedMillis.h>

#include "Macros.h"
#include "Datalog_Configuration.h"
#include "Hardware_Configuration.h"
#include "Configuration.h"
#include "Configuration_adv.h"

#include "PrintUtils.h"
// that is all for this file

// desired classes
// - neoP (initalised with mode specific data)
// lux (initalised with mode specific data)
// audio engine (should be shared with Lyre?) 
// datalogging (should be shared with Lyre)
// mode (for different installations of the Moth mode)
