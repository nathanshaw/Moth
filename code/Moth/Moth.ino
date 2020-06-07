/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2020
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

elapsedMillis last_jumper_read;

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
#if NUM_LUX_MANAGERS > 1
    for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
      lux_managers[i].update();
    }
#else
    lux_manager.update();
#endif
  }
}

bool testJumpers() {
  delay(2000);
  bool populated = true;
  bool values[10];
  values[0] = digitalRead(JMP1_PIN);
  values[1] = digitalRead(JMP2_PIN);
  values[2] = digitalRead(JMP3_PIN);
  values[3] = digitalRead(JMP4_PIN);
  values[4] = digitalRead(JMP5_PIN);
  values[5] = digitalRead(JMP6_PIN);
  values[6] = digitalRead(JMP7_PIN);
  values[7] = digitalRead(JMP8_PIN);
  values[8] = digitalRead(JMP9_PIN);
  values[9] = digitalRead(JMP10_PIN);
  printMinorDivide();
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
    if (values[6] != digitalRead(JMP7_PIN)) {
      populated = false;
      Serial.println("JMP7_PIN returned multiple values");
    }
    if (values[7] != digitalRead(JMP8_PIN)) {
      populated = false;
      Serial.println("JMP8_PIN returned multiple values");
    }
    if (values[8] != digitalRead(JMP9_PIN)) {
      populated = false;
      Serial.println("JMP9_PIN returned multiple values");
    }
    if (values[9] != digitalRead(JMP10_PIN)) {
      populated = false;
      Serial.println("JMP10_PIN returned multiple values");
    }
    delay(50);
  }
  Serial.print("Populated: "); Serial.println(populated);
  return populated;
}

void readUserControls() {
  if (JUMPERS_POPULATED == true && last_jumper_read > 2000) {
    readPots();
    readJumpers();
    last_jumper_read = 0;
  }
}

void readPots() {
  if (NUM_POTS > 0 && last_jumper_read > 1000) {
    for (int i = 0; i < NUM_POTS; i++) {
      pot_vals[i] = 1023 - analogRead(pot_pins[i]);
    }
  }
  if (PRINT_POT_VALS) {
    printPots();
  }
}

void printPots() {
  Serial.println("-------------------------------------------");
  Serial.print("Pot vals: ");
  for (int i = 0; i < NUM_POTS; i++) {
    Serial.print(pot_vals[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void readJumpers() {
    Serial.println("-------------------------------------------");
    //////////// Jumper 1 ///////////////////////
    bool temp_b;
    ENCLOSURE_TYPE = digitalRead(JMP1_PIN);
    Serial.print("(pin1) - ");
    if (ENCLOSURE_TYPE == 0) {
      num_channels = 1;
      Serial.print(" OFF - Ground Enclosure - num_channels = 1");
    } else {
      num_channels = 2;
      Serial.print(" ON - Orb Enclosure - num_channels = 2");
    }
    Serial.println();
    //////////// Jumper 2 ///////////////////////
    /////////// Boot Delay //////////////////////
    temp_b = digitalRead(JMP2_PIN);
    BOOT_DELAY *= temp_b;
    Serial.print("(pin2) -  ");
    if (temp_b == 0) {
      Serial.print("OFF - ");
    } else {
      Serial.print("ON  - ");
    }
    Serial.print("Boot Delay (in seconds) = ");
    Serial.println(BOOT_DELAY / 1000);

    //////////// Jumper 3 ///////////////////////
    //////////// Minor Sensitivity Attenuation //
    temp_b = digitalRead(JMP3_PIN);
    FLASH_DOMINATES = temp_b;
    if (temp_b == 1) {
      Serial.println("(pin3)  - ON  - FLASH_DOMINATES is true, flash will erase other brightness messages");
    } else {
      Serial.println("(pin3)  - OFF - FLASH_DOMINATES is false, flash will be added to other brightness messages");
    }

    //////////// Jumper 4 ///////////////////////
    //////////// Major Sensitivity Attenuation //
    double total_scaler = 0.0;
    temp_b = digitalRead(JMP4_PIN);
    if (temp_b == 1) {
      Serial.println("(pin4)  - ON  - MASTER_SENSITIVITY_SCALER not decreased by 50%");
    } else {
      Serial.println("(pin4)  - OFF - MASTER_SENSITIVITY_SCALER decreased by 50%");
      total_scaler -= 0.5;
    }

    ///////////// Jumper 5 //////////////////////
    //////////// Minor Sensitivity Boost ///////////////

    temp_b = digitalRead(JMP5_PIN);
    if (temp_b == 1) {
      total_scaler += 0.5;
      Serial.println("(pin5)  - ON  - MASTER_SENSITIVITY_SCALER increased by 50% : ");
    } else {
      Serial.println("(pin5)  - OFF - MASTER_SENSITIVITY_SCALER not increased by 50% : ");
    }

    ///////////// Jumper 6 //////////////////////
    //////////// Major Sensitivity Boost ////////////////
    temp_b = digitalRead(JMP6_PIN);
    if (temp_b == 1) {
      total_scaler += 1.0;
      Serial.print("(pin6)  - ON  - MASTER_SENSITIVITY_SCALER increased by 100% : ");
    } else {
      Serial.print("(pin6)  - OFF - MASTER_SENSITIVITY_SCALER not increased by 100% : ");
    }
    MASTER_SENSITIVITY_SCALER = 1.0;
    MASTER_SENSITIVITY_SCALER += total_scaler;
    Serial.println(MASTER_SENSITIVITY_SCALER);
    Serial.print("\n MASTER_SENSITIVITY_SCALER set to : ");

    ///////////// Jumper 7 //////////////////////
    temp_b = digitalRead(JMP7_PIN);
    if (temp_b == 1) {
      Serial.print("(pin7)  - ON  - TODO");
    } else {
      Serial.print("(pin7)  - OFF - TODO");
    }

    ///////////// Jumper 8 //////////////////////
    temp_b = digitalRead(JMP8_PIN);
    if (temp_b == 1) {
      Serial.print("(pin8)  - ON  - TODO");
    } else {
      Serial.print("(pin8)  - OFF - TODO");
    }
    Serial.println();

    ///////////// Jumper 9 //////////////////////
    temp_b = digitalRead(JMP9_PIN);
    if (temp_b == 1) {
      Serial.print("(pin9)  - ON  - TODO");
    } else {
      Serial.print("(pin9)  - OFF - TODO");
    }
    Serial.println();

    ///////////// Jumper 10 //////////////////////
    temp_b = digitalRead(JMP10_PIN);
    if (temp_b == 1) {
      Serial.print("(pin10) - ON  - TODO");
    } else {
      Serial.print("(pin10) - OFF - TODO");
    }
    MASTER_SENSITIVITY_SCALER += total_scaler;
    Serial.println();
  printMinorDivide();
}

void setupUserControls() {
  Serial.println("Jumpers passed continuity test...");
  printMinorDivide();
  for (int i = 0; i < NUM_JUMPERS; i++) {
    pinMode(jmp_pins[i], INPUT);
  }
  for (int i = 0; i < NUM_POTS; i++) {
    pinMode(pot_pins[i], INPUT);
  }
  /*
    pinMode(JMP1_PIN, INPUT);
    pinMode(JMP2_PIN, INPUT);
    pinMode(JMP3_PIN, INPUT);
    pinMode(JMP4_PIN, INPUT);
    pinMode(JMP5_PIN, INPUT);
    pinMode(JMP6_PIN, INPUT);
    pinMode(JMP7_PIN, INPUT);
    pinMode(JMP8_PIN, INPUT);
    pinMode(JMP9_PIN, INPUT);
    pinMode(JMP10_PIN, INPUT);

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
    Serial.print(digitalRead(JMP6_PIN));
    Serial.print("\t");
    Serial.print(digitalRead(JMP7_PIN));
    Serial.print("\t");
    Serial.print(digitalRead(JMP8_PIN));
    Serial.print("\t");
    Serial.print(digitalRead(JMP9_PIN));
    Serial.print("\t");
    Serial.println(digitalRead(JMP10_PIN));
  */
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
  Serial.print("There are ");
  Serial.print(NUM_LED);
  Serial.println(" LEDs");
  setupUserControls();
  testJumpers();
  Serial.println("LEDS have been initalised");
  delay(3000); Serial.println("Setup Loop has started");
  readUserControls();
  setupDLManagerCicada();
  setupAudio();

  for (int i = 0; i < NUM_NEO_GROUPS; i++) {
    neos[i].setFlashColors(CLICK_RED, CLICK_GREEN, CLICK_BLUE);
    neos[i].setSongColors(SONG_RED_HIGH, SONG_GREEN_HIGH, SONG_BLUE_HIGH);
    neos[i].setFlashBehaviour(FLASH_DOMINATES);
    // neos[i].setSongFeedbackMode(ROUND);
  }
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 0, 0, 0);
    leds.show();
  }
  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_manager.add6030Sensors();
    // lux_manager.addLuxSensor(0, "Front");
    // lux_manager.addLuxSensor(1, "Rear");
    // lux_manager.linkNeoGroup(&neos[0]);
    // lux_manager.linkNeoGroup(&neos[0]);
    // lux_manager.startSensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    delay(200);
    // lux_manager.calibrate(LUX_CALIBRATION_TIME);
  }

#if FIRMWARE_MODE == TEST_MODE
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 64, 64, 64);
    leds.show();
  }
  delay(10000000);
#endif
  printMajorDivide("Setup Loop Finished");
  uint32_t segment = (uint32_t)((double)BOOT_DELAY / (double)NUM_LED);
  // Serial.print("segment : ");
  // Serial.println(segment);
  for (int it = 0; it < NUM_LED; it++) {
    leds.setPixel(it, 10, 32, 20);
    leds.show();
    delay(segment);
  }
  for (int it = 0; it < NUM_LED; it++) {
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
  readUserControls();
  listenForSerialCommands();
}
