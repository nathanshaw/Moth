/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2019
  The runtime, boot, and all other configurations are found in the Configuration.h file
*/
#include "Configuration.h"
#include "Configuration_datalogging.h"
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
    if (updated) {
      // calculate what the combined lux is
      double combined_lux = 0;
      for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
        if (combined_lux < lux_managers[i].getLux()) {
          combined_lux = lux_managers[i].getLux();
        }
        dprint(PRINT_LUX_DEBUG, " combined_lux value is : ");
        dprint(PRINT_LUX_DEBUG, combined_lux);
        // set the new combined lux value to both managers
        for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
          lux_managers[i].setLuxValue(combined_lux);
          dprint(PRINT_LUX_DEBUG, "\t");
          dprint(PRINT_LUX_DEBUG, lux_managers[i].getName());
          dprint(PRINT_LUX_DEBUG, " lux reading : ");
          dprint(PRINT_LUX_DEBUG, lux_managers[i].getLux());
        }
      }
    }
  }
}

bool testJumpers() {
  bool populated = true;
  bool values[6];
  values[0] = digitalRead(JMP1_PIN);
  values[1] = digitalRead(JMP2_PIN);
  values[2] = digitalRead(JMP3_PIN);
  values[3] = digitalRead(JMP4_PIN);
  values[4] = digitalRead(JMP5_PIN);
  values[5] = digitalRead(JMP6_PIN);
  Serial.println("Testing the PCB for jumpers");
  for (int i = 0; i < 10; i++) {
    if (values[0] != digitalRead(JMP1_PIN)) {
      populated = false;
      Serial.println("JMP1_PIN returned multiple values");
    }
    if (values[1] != digitalRead(JMP2_PIN)) {
      populated = false;
      Serial.println("JMP2_PIN returned multiple values");
    }
    if (values[2] != digitalRead(JMP3_PIN)) {
      populated = false;
      Serial.println("JMP3_PIN returned multiple values");
    }
    if (values[3] != digitalRead(JMP4_PIN)) {
      populated = false;
      Serial.println("JMP4_PIN returned multiple values");
    }
    if (values[4] != digitalRead(JMP5_PIN)) {
      populated = false;
      Serial.println("JMP5_PIN returned multiple values");
    }
    if (values[5] != digitalRead(JMP6_PIN)) {
      populated = false;
      Serial.println("JMP6_PIN returned multiple values");
    }
    delay(10);
  }
  return populated;
}

void readJumpers() {
  if (testJumpers() == true) {
    Serial.print("Jumpers passed continuity test...");
    printMinorDivide();
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

    //////////// Jumper 1 ///////////////////////
    bool temp_b;
    ENCLOSURE_TYPE = digitalRead(JMP1_PIN);
    Serial.print("(pin1) Enclosure                              : ");

    if (ENCLOSURE_TYPE == ORB_ENCLOSURE) {
      num_channels = 2;
      Serial.print("Orb Enclosure: ");
      Serial.println("num_channels set to 2");
    } else if (ENCLOSURE_TYPE == GROUND_ENCLOSURE) {
      num_channels = 1;
      Serial.print("Ground Enclosure: ");
      Serial.println("num_channels set to 1");
    }
    //////////// Jumper 2 ///////////////////////
    /////////// Boot Delay //////////////////////
    temp_b = digitalRead(JMP2_PIN);
    BOOT_DELAY *= temp_b;
    Serial.print("(pin2) Boot Delay (in seconds)                : ");
    Serial.println(BOOT_DELAY / 1000);

    //////////// Jumper 3 ///////////////////////
    //////////// Minor Sensitivity Attenuation //
    double total_scaler = 0.0;
    temp_b = digitalRead(JMP3_PIN);
    if (temp_b == 1) {
      Serial.println("(pin3) MASTER_SENSITIVITY_SCALER not decreased by 33%");
    } else {
      Serial.println("(pin3) MASTER_SENSITIVITY_SCALER decreased by 33%");
      total_scaler -= 0.33;
    }

    //////////// Jumper 4 ///////////////////////
    //////////// Major Sensitivity Attenuation //
    temp_b = digitalRead(JMP4_PIN);
    if (temp_b == 1) {
      Serial.println("(pin4) MASTER_SENSITIVITY_SCALER not decreased by 50%");
    } else {
      Serial.println("(pin4) MASTER_SENSITIVITY_SCALER decreased by 50%");
      total_scaler -= 0.5;
    }

    ///////////// Jumper 5 //////////////////////
    //////////// Minor Sensitivity Boost ///////////////

    temp_b = digitalRead(JMP5_PIN);
    if (temp_b == 1) {
      total_scaler += 0.5;
      Serial.println("(pin5) MASTER_SENSITIVITY_SCALER increased by 50% : ");
    } else {
      Serial.println("(pin5) MASTER_SENSITIVITY_SCALER not increased by 50% : ");
    }

    ///////////// Jumper 6 //////////////////////
    //////////// Major Sensitivity Boost ////////////////
    temp_b = digitalRead(JMP6_PIN);
    if (temp_b == 1) {
      total_scaler += 1.0;
      Serial.print("(pin6) MASTER_SENSITIVITY_SCALER increased by 100% : ");
    } else {
      Serial.print("(pin6) MASTER_SENSITIVITY_SCALER not increased by 100% : ");
    }
    MASTER_SENSITIVITY_SCALER += total_scaler;
    Serial.println(MASTER_SENSITIVITY_SCALER);

  } else {
    Serial.println("ERROR - this PCB does not contain jumpers, or jumper pins are not populated");
  }
  printMinorDivide();
}

void setup() {
  delay(2000); // to avoid booting to the bootloader
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Serial begun");
  leds.begin();
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 12, 12, 0);
    leds.show();
  }
  Serial.println("LEDS have been initalised");
  delay(3000); Serial.println("Setup Loop has started");
  if (JUMPERS_POPULATED) {// TODO this should also check the READ_JUMPERS bool, jumpers populated should be determined by hardware revision
    readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  setupDLManagerCicada();
  setupAudio();

  for (int i = 0; i < NUM_NEO_GROUPS; i++) {
    neos[i].setFlashColors(CLICK_RED, CLICK_GREEN, CLICK_BLUE);
  }
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 0, 0, 0);
    leds.show();
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

// #if FIRMWARE_MODE == TEST_MODE
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 64, 64, 64);
    leds.show();
   
  }
  delay(10000000);
// #endif
  printMajorDivide("Setup Loop Finished");
  uint32_t segment = (uint32_t)((double)BOOT_DELAY / (double)NUM_LED * 0.5);
  // Serial.print("segment : ");
  // Serial.println(segment);
  for (int it = 0; it < NUM_LED / 2; it++) {
    leds.setPixel(it, 10, 32, 20);
    leds.show();
    delay(segment);
  }
  for (int it = 0; it < NUM_LED / 2; it++) {
    leds.setPixel(it, 0, 0, 0);
  }
  leds.show();
}

void listenForSerialCommands() {
  if (Serial.available() > 0) {
    int input = Serial.read();
    Serial.print("incbyte : ");
    Serial.println(input);
    if (input == 'd') {
      // this is the command to print the datalog
      datalog_manager.printAllLogs();
    }
    if (input == 's') {
      input = Serial.read();
      if (input == 'e') {
        input = Serial.read();
        if (input == 't') {
          Serial.println("What setting would you like to change?");
          Serial.println("enter g for gain");
          input = Serial.read();
          if (input == 'g') {
            Serial.println("what would you like to change the gain to?");
            input = Serial.read() - 48;
            Serial.print("A gain of ");
            Serial.print(input);
            Serial.println(" has been selected");
          }
        }

      }
    }
  }
}

void loop() {
  updateLuxManagers();
  updateFeatureCollectors();
  updateMode();
  updateAutogain();
  updateDatalog();
  listenForSerialCommands();
}
