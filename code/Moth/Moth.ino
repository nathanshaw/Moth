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
#elif FIRMWARE_MODE == TEST_MODE
#include "ModeTest.h"
#endif

void updateFeatureCollectors() {
  // update the feature collectors
#if NUM_FEATURE_COLLECTORS == 1
  fc.update();
#endif
#if NUM_FEATURE_COLLECTORS > 1
  for (int i = 0; i < NUM_FEATURE_COLLECTORS; i++) {
    fc[i].update();
  }
#endif
}

void updateLuxManagers() {
  // update the feature collectors
  if (LUX_SENSORS_ACTIVE) {
    uint8_t updated = 0;
    for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
      if (lux_managers[i].update()) {
        updated++;
      }
    }
    if (COMBINE_LUX_READINGS && updated) {
      for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
        lux_managers[i].forceLuxReading(lux_managers[NUM_LUX_MANAGERS - i].getLux());
        dprint(PRINT_LUX_DEBUG, lux_managers[i].getName());
        dprint(PRINT_LUX_DEBUG, " lux reading : ");
        dprint(PRINT_LUX_DEBUG, lux_managers[i].getLux());
      }
    }
  }
}

void updateDatalog() {
  datalog_manager.update();
  runtime = (double)millis() / 60000;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  Serial.println("Serial begun");
  delay(1000);
  leds.begin();
  Serial.println("LEDS have been initalised");
  delay(3000); Serial.println("Setup Loop has started");
  if (JUMPERS_POPULATED) {
    // readJumpers();
    Serial.println("SORRY THE JUMPERS POPULATED FUNCTIONALITY IS NOT IMPLEMENTED");
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  Serial.println("Leds turned yellow for setup loop\n");
  delay(1000);
  setupDLManager();
  setupAudio();

  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    delay(200);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
  }
#if FIRMWARE_MODE == TEST_MODE
  for (int i = 0; i < 10; i++) {
    leds.setPixel(i, 8, 8, 8);
    leds.show();
  }
#endif
  printMajorDivide("Setup Loop Finished");
}

void loop() {
  updateLuxManagers();
  updateFeatureCollectors();
  updateMode();
  updateAutogain();
  updateDatalog();
}
