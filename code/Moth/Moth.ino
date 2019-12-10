/**
   Moth Sonic Art Framework Firmware
   Written by Nathan Villicana-Shaw in 2019
   The runtime, boot, and all other configurations are found in the Configuration.h file
*/

#include "Configuration.h"
#if FIRMWARE_MODE == CICADA_MODE
#include "ModeCicada.h"
#elif FIRMWARE_MODE == PITCH_MODE
#include "ModePitch.h"
#endif

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  Serial.println("Serial begun");
  delay(1000);
  leds.begin();
  Serial.println("LEDS have been initalised");
  neos[0].colorWipe(250, 90, 0); // turn off the LEDs
  neos[1].colorWipe(250, 90, 0); // turn off the LED
  delay(3000); Serial.println("Setup Loop has started");
  if (JUMPERS_POPULATED) {
    // readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  // create either front and back led group, or just one for both
  neos[0].colorWipe(120, 70, 0); // turn off the LEDs
  neos[1].colorWipe(120, 70, 0); // turn off the LEDs
  Serial.println("Leds turned yellow for setup loop\n");
  delay(1000);
  setupDLManager();
  neos[0].colorWipe(100, 150, 0); // turn off the LEDs
  neos[1].colorWipe(100, 150, 0); // turn off the LEDs
  Serial.println("Running Use Specific Setup Loop...");
  audioSetup();
  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo

    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    neos[0].colorWipe(0, 0, 0); // turn off the LEDs
    neos[1].colorWipe(0, 0, 0); // turn off the LED
    delay(200);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
  }
  printMajorDivide("Setup Loop Finished");
}

void loop() {
  // this needs to stay in the faster main loop
  mainLoop();
}
