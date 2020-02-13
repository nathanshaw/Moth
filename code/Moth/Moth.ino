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
#if COMBINE_LUX_READINGS > 0
    if (updated) {
      // calculate what the combined lux is
      combined_lux = 0;
      for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
        combined_lux += lux_managers[i].getLux();
      }
      combined_lux /= NUM_LUX_MANAGERS;
      dprint(PRINT_LUX_DEBUG, " combined_lux value is : ");
      dprint(PRINT_LUX_DEBUG, combined_lux);
      // set the new combined lux value to both managers
      for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
        lux_managers[i].forceLuxReading(combined_lux);
        dprint(PRINT_LUX_DEBUG, "\t");
        dprint(PRINT_LUX_DEBUG, lux_managers[i].getName());
        dprint(PRINT_LUX_DEBUG, " lux reading : ");
        dprint(PRINT_LUX_DEBUG, lux_managers[i].getLux());
      }
    }
#endif
  }
}

void updateDatalog() {
  datalog_manager.update();
  runtime = (double)millis() / 60000;
}

void readJumpers() {
  Serial.println("Reading Jumpers");
  pinMode(JMP1_PIN, INPUT);
  pinMode(JMP2_PIN, INPUT);
  pinMode(JMP3_PIN, INPUT);
  pinMode(JMP4_PIN, INPUT);
  pinMode(JMP5_PIN, INPUT);
  pinMode(JMP6_PIN, INPUT);
  Serial.print(digitalRead(JMP1_PIN));
  Serial.print("\t");
  Serial.print(digitalRead(JMP2_PIN));
  Serial.print("\t");
  Serial.print(digitalRead(JMP3_PIN));
  Serial.print("\t");
  Serial.print(digitalRead(JMP4_PIN));
  Serial.print("\t");
  Serial.print(digitalRead(JMP5_PIN));
  Serial.print("\t");
  Serial.println(digitalRead(JMP6_PIN));

  num_channels = digitalRead(JMP1_PIN);
  num_channels++;
  Serial.print("Number of channels changed to : ");
  Serial.println(num_channels);
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  Serial.println("Serial begun");
  delay(1000);
  leds.begin();
  for (int i = 0; i < 10; i++) {
    leds.setPixel(i, 12, 12, 0);
    leds.show();
  }
  Serial.println("LEDS have been initalised");
  delay(3000); Serial.println("Setup Loop has started");
  if (JUMPERS_POPULATED) {
    readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  setupDLManager();
  setupAudio();

  for (int i = 0; i < NUM_NEO_GROUPS; i++) {
    neos[i].setFlashColors(FLASH_RED, FLASH_GREEN, FLASH_BLUE);
  }

  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    delay(200);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
  }
  for (int i = 0; i < 10; i++) {
    leds.setPixel(i, 0, 0, 0);
    leds.show();
  }
#if FIRMWARE_MODE == TEST_MODE
  for (int i = 0; i < 10; i++) {
    leds.setPixel(i, 64, 64, 64);
    leds.show();
  }
#endif
  printMajorDivide("Setup Loop Finished");
}

void listenForSerialCommands() {
  if (Serial.available() > 0) {
    int incByte = Serial.read();
    Serial.print("incbyte : ");
    Serial.println(incByte);
  }
}

elapsedMillis loop_length = 0;
unsigned long num_loops = 0;
unsigned long loop_totals = 0;
unsigned long longest_loop = 0;
unsigned long shortest_loop = 0;

void loop() {
  updateLuxManagers();
  updateFeatureCollectors();
  updateMode();
  updateAutogain();
  updateDatalog();
  listenForSerialCommands();
  #if LOOP_LENGTH == true
  if (num_loops > 0) {
    if (loop_length > longest_loop) {
      longest_loop = loop_length;
      Serial.print("new longest loop : ");
      Serial.println(longest_loop);
    }
    loop_totals += loop_length;
    if (loop_totals % 1000 == 1) {
      Serial.print("average loop length: ");
      Serial.println((double)loop_totals / (double) num_loops);
    }
  }
  #endif// LOOP_LENGTH == true
  num_loops++;
  loop_length = 0;
}
