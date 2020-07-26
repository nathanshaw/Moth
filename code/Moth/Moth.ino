/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2020
  The runtime, boot, and all other configurations are found in the Configuration.h file
*/
#include "Configuration.h"
#include "Configuration_datalogging.h"
#include "Mode.h"

// this is a high value to force jumper readings in the setup() loop
elapsedMillis last_jumper_read = 100000;

void updateFeatureCollectors() {
  // update the feature collectors
  fft_features.updateFFT();
#if NUM_FEATURE_COLLECTORS == 1
  fc[0].update();
#endif
#if NUM_FEATURE_COLLECTORS > 1
  for (int i = 0; i < NUM_FEATURE_COLLECTORS; i++) {
    fc[i].update();
  }
#endif
#if P_AUDIO_MEMORY_MAX > 0
  Serial.print("audio memory max: ");
  Serial.print(AudioMemoryUsageMax());
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
  Serial.println("Testing the PCB for jumpers");
  delay(2000);
  bool populated = true;
  bool values[10];
  values[0] = digitalRead(JMP1_PIN);
  values[1] = digitalRead(JMP2_PIN);
  values[2] = digitalRead(JMP3_PIN);
  values[3] = digitalRead(JMP4_PIN);
  values[4] = digitalRead(JMP5_PIN);
  values[5] = digitalRead(JMP6_PIN);
#if NUM_JUMPERS > 6
  values[6] = digitalRead(JMP7_PIN);
  values[7] = digitalRead(JMP8_PIN);
  values[8] = digitalRead(JMP9_PIN);
  values[9] = digitalRead(JMP10_PIN);
#endif // HV_major > 2
  printMinorDivide();

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

#if HV_MAJOR > 2
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

#endif // HV_MAJOR > 2
    if (populated == true) {
      Serial.print(".\t");
    }
    delay(100);
  }
  if (populated == true) {
    Serial.println("\nGreat news, the jumpers all seem to be in working order");
  } else {
    Serial.println("Crap, for some reason some of the jumpers are returning multiple values, are the headers present?");
  }
  return populated;
}


#if HV_MAJOR > 2
void readPots() {
  if (NUM_POTS > 0 && last_jumper_read > USER_CONTROL_POLL_RATE) {
    for (int i = 0; i < NUM_POTS; i++) {
      pot_vals[i] = 1023 - analogRead(pot_pins[i]);
    }
  }
  /////////////////////////////////// Use Pot Values to update appropiate runtime values //////////////////////////////////
  if (USER_BRIGHTNESS_OVERDRIVE == true) {
    double scaled_brightness = (double) pot_vals[BS_POT_NUM] / 1023;
    if (pot_vals[BS_POT_NUM] < 512) {
      scaled_brightness = scaled_brightness * 2;
      if (scaled_brightness < POT_BS_MIN) {
        scaled_brightness = POT_BS_MIN;
      }
    } else {
      scaled_brightness = (scaled_brightness - 0.5) * POT_BS_MAX;
      if (scaled_brightness < 1.0) {
        scaled_brightness = 1.0;
      }
    }
    Serial.print("user brightness scaler (pre lux scaler) is now: ");
    Serial.println(scaled_brightness);
    user_brightness_scaler = scaled_brightnes;
    printMinorDivide();
  }
  if (USER_ONSET_THRESH_OVERRIDE == true) {
    Serial.println("WARNING USER_ONSET_THRESH_OVERRIDE is not implemented");
    // _ONSET_THRESH = mapf(_ot, 0.0, 1.0, USER_OT_MIN, USER_OT_MAX);
  }
  // if
  if (P_POT_VALS) {
    printPots();
    }
}

void printPots() {
  printMinorDivide();
  Serial.print("Pot vals: ");
  for (int i = 0; i < NUM_POTS; i++) {
    Serial.print(pot_vals[i]);
    Serial.print("\t");
  }
  Serial.println();
}
#endif // HV_MAJOR > 2

void readJumpers() {
  printMinorDivide();
  //////////// Jumper 1 ///////////////////////
  bool temp_b;
#if HV_MAJOR < 3
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
#else
  temp_b = digitalRead(JMP1_PIN);
  Serial.print("(pin1) - ");
  if (temp_B == 0) {
    Serial.print(" OFF - TODO");
  } else {
    Serial.print(" ON - TODO");
  }
  Serial.println();
#endif//HV_MAJOR
  //////////// Jumper 2 ///////////////////////
  /////////// Boot Delay //////////////////////
  temp_b = digitalRead(JMP2_PIN);
  BOOT_DELAY *= !temp_b;
  Serial.print("(pin2) -  ");
  if (temp_b == 0) {
    Serial.print("OFF - ");
  } else {
    Serial.print("ON  - ");
  }
  Serial.print("Boot Delay (in seconds) = ");
  Serial.println(BOOT_DELAY / 1000);

  //////////// Jumper 3 ///////////////////////
  //////////// Flash Dominates //
  temp_b = digitalRead(JMP3_PIN);
#if FIRMWARE_MODE == CICADA_MODE
  FLASH_DOMINATES = temp_b;
  if (temp_b == 1) {
    Serial.println("(pin3)  - ON  - FLASH_DOMINATES is true, flash will erase other brightness messages");
  } else {
    Serial.println("(pin3)  - OFF - FLASH_DOMINATES is false, flash will be added to other brightness messages");
  }
#else
  temp_b = digitalRead(JMP1_PIN);
  Serial.print("(pin3) - ");
  if (temp_b == 0) {
    Serial.print(" OFF - TODO");
  } else {
    Serial.print(" ON - TODO");
  }
  Serial.println();
#endif//FIRMWARE_MODE

  //////////// Jumper 4 ///////////////////////
  //////////// Major Sensitivity Attenuation //
#if HV_MAJOR < 3
  double total_scaler = 0.0;
  temp_b = digitalRead(JMP4_PIN);
  if (temp_b == 1) {
    Serial.print("(pin4)  - ON  - user_brightness_scaler not decreased by 50% : ");
  } else {
    Serial.print("(pin4)  - OFF - user_brightness_scaler decreased by 50% : ");
    total_scaler -= 0.5;
  }
  Serial.println(total_scaler);
#else
  temp_b = digitalRead(JMP4_PIN);
  Serial.print("(pin4) - ");
  if (temp_B == 0) {
    Serial.print("(pin4)  - OFF - TODO");
  } else {
    Serial.print("(pin4  - ON - TODO");
  }
  Serial.println();
#endif//HV_MAJOR
  ///////////// Jumper 5 //////////////////////
  //////////// Minor Sensitivity Boost ///////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(JMP5_PIN);
  if (temp_b == 1) {
    total_scaler += 0.5;
    Serial.print("(pin5)  - ON  - user_brightness_scaler increased by 50% : ");
  } else {
    Serial.print("(pin5)  - OFF - user_brightness_scaler not increased by 50% : ");
  }
  Serial.println(total_scaler);
#else
  temp_b = digitalRead(JMP4_PIN);
  Serial.print("(pin4) - ");
  if (temp_B == 0) {
    Serial.print("(pin4)  - OFF - TODO");
  } else {
    Serial.print("(pin4  - ON - TODO");
  }
  Serial.println();
#endif//HV_MAJOR
  ///////////// Jumper 6 //////////////////////
  //////////// Major Sensitivity Boost ////////////////
  #if HV_MAJOR < 3
  temp_b = digitalRead(JMP6_PIN);
  if (temp_b == 1) {
    total_scaler += 1.0;
    Serial.print("(pin6)  - ON  - user_brightness_scaler increased by 100% : ");
  } else {
    Serial.print("(pin6)  - OFF - user_brightness_scaler not increased by 100% : ");
  }
  Serial.println(total_scaler);
#else
  temp_b = digitalRead(JMP6_PIN);
  Serial.print("(pin6) - ");
  if (temp_B == 0) {
    Serial.print("OFF - TODO");
  } else {
    Serial.print("ON - TODO");
  }
  Serial.println();
#endif//HV_MAJOR
#if HV_MAJOR > 2
  ///////////// Jumper 7 //////////////////////
  ///////////// Center Out Mapping ////////////
  temp_b = digitalRead(JMP7_PIN);
  if (temp_b == 1) {
    Serial.print("(pin7)  - ON  - FEEDBACK MODE SET TO CENTER_OUT");
    LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
  } else {
    Serial.print("(pin7)  - OFF - FEEDBACK MODE remains STANDARD");
    LED_MAPPING_MODE = LED_MAPPING_STANDARD;
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
#endif // HV_MAJOR > 2
  user_brightness_scaler = 1.0;
  user_brightness_scaler += total_scaler;
  Serial.print("\n\nuser_brightness_scaler set to : ");
  Serial.println(user_brightness_scaler);
  Serial.println();
  printMinorDivide();
}

void setupUserControls() {
  for (int i = 0; i < NUM_JUMPERS; i++) {
    pinMode(jmp_pins[i], INPUT);
  }
#if HV_MAJOR > 2
  for (int i = 0; i < NUM_POTS; i++) {
    pinMode(pot_pins[i], INPUT);
  }
#endif // HV_MAJOR
}

void readUserControls() {
  if (JUMPERS_POPULATED != true) {
    Serial.println("Sorry jumpers not populated, exiting readUserControl()");
    return;
  }
  if (last_jumper_read < USER_CONTROL_POLL_RATE) {
    // dprintln(P_USER_CONTROLS, "Sorry the last reading was too resceent exiting readUserControl();");
    return;
  }
#if HV_MAJOR > 2
  readPots();
#endif // HV_MAJOR
  readJumpers();
  last_jumper_read = 0;
  listenForSerialCommands();
}

void explainSerialCommands() {
  Serial.println("The Following Serial Commands are Supported: ");
  Serial.println("Print Commands, denoted by a p prefix: ");
  Serial.println("brightness_scaler (bs)");
  Serial.println("datalogs (dl)");
  printMinorDivide();
}

void listenForSerialCommands() {
  if (Serial.available() > 0) {
    int input = Serial.read();
    Serial.print("incbyte : ");
    Serial.println(input);
    //////////////// PRINT COMMANDS ////////////////
    if (input == 'p') {
      input = Serial.read();
      // datalogs
      if (input == 'd') {
        input = Serial.read();
        if (input == 'l') {
          // this is the command to print the datalog
          datalog_manager.printAllLogs();
        }
      }
      // print brightness scaler
      if (input == 'b') {
        input = Serial.read();
        if (input == 's') {
          Serial.print("Brightness Scalers: ");
          Serial.println(lux_manager.brightness_scaler);
          // Serial.print("\t");
          // Serial.println(brightness_scalers[1]);
        }
      }
      /////////////// Changing Values ////////////////////////
      if (input == 's') {
        input = Serial.read();
        if (input == 'g') {
          Serial.println("what would you like to change the gain to? Please enter in a gain in the format of 1.00.");
          input = Serial.read() - 48;
          Serial.print("A gain of ");
          Serial.print(input);
          Serial.println(" has been selected");
        }
      }
    }
  }
}

void setup() {
  ///////////////// Serial ///////////////////////////////////
  delay(2000); // to avoid booting to the bootloader
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);// so that our Serial messages will appear
  printDivide();
  Serial.println("Entering the Setup Loop");
  Serial.println("Serial begun");
  printMinorDivide();
  //////////////// Hardware/Software Version /////////////////
  Serial.print("Firmware is version : ");
  Serial.print(SV_MAJOR);
  Serial.print(".");
  Serial.print(SV_MINOR);
  Serial.print(".");
  Serial.println(SV_SUBVERSION);
  Serial.print("Firmware was compiled for a PCB with a hardware revsion of : ");
  Serial.print(HV_MAJOR);
  Serial.print(".");
  Serial.println(HV_MINOR);
  printMinorDivide();
  //////////////// User Controls /////////////////////////////
  explainSerialCommands();
  setupUserControls();
  testJumpers();
  readUserControls();
  //////////////// Leds //////////////////////////////////////
  leds.begin();
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 12, 12, 0);
    leds.show();
  }

  printMinorDivide();
  Serial.println("LEDS have been initalised");
  Serial.print("There are ");
  Serial.print(NUM_LED);
  Serial.println(" LEDs");
  delay(3000);
  printMinorDivide();
  ///////////////////////// Audio //////////////////////////
  setupAudio();
  Serial.print("global_peak_scaler = ");
  Serial.println(global_peak_scaler);
  Serial.print("global_rms_scaler  = ");
  Serial.println(global_rms_scaler);
  Serial.print("global_fft_scaler  = ");
  Serial.println(global_fft_scaler);
  ///////////////////////// NeoPixels //////////////////////////

  for (int i = 0; i < NUM_NEO_GROUPS; i++) {
    neos[i].setFlashColors(ONSET_RED, ONSET_GREEN, ONSET_BLUE);
    neos[i].setSongColors(SONG_RED_HIGH, SONG_GREEN_HIGH, SONG_BLUE_HIGH);
    neos[i].setFlashBehaviour(FLASH_DOMINATES);
    neos[i].changeMapping(LED_MAPPING_MODE);
    // neos[i].setSongFeedbackMode(ROUND);
  }

  //////////////////////////// Lux Sensors //////////////////////////////
  printMinorDivide();
  Serial.println("turning off LEDs for Lux Calibration");
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 0, 0, 0);
  }
  leds.show();
  Serial.println("LEDS off");
  delay(500);
  // todo make this proper

#if HV_MAJOR > 2
  lux_manager.add6030Sensors(0.125, 25);
  lux_manager.linkNeoGroup(&neos[0]);
  lux_manager.linkNeoGroup(&neos[1]);
  delay(200);
  lux_manager.calibrate(LUX_CALIBRATION_TIME);
  if ((lux_manager.sensor_active[0] | lux_manager.sensor_active[1]) > 0) {
    LUX_SENSORS_ACTIVE = true;
  }
#else
  lux_manager.addSensorTcaIdx("Front", 0);
  lux_manager.addSensorTcaIdx("Rear", 1);
  lux_manager.startTCA7700Sensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  if ((lux_manager.sensor_active[0] | lux_manager.sensor_active[1]) > 0) {
    lux_manager.linkNeoGroup(&neos[0]);
    lux_manager.linkNeoGroup(&neos[1]);
    delay(200);
    lux_manager.calibrate(LUX_CALIBRATION_TIME);
    if ((lux_manager.sensor_active[0] | lux_manager.sensor_active[1]) > 0) {
      LUX_SENSORS_ACTIVE = true;
    }
  }
#endif // HV_MAJOR

#if FIRMWARE_MODE == TEST_MODE
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 64, 64, 64);
    leds.show();
  }
  delay(10000000);
#endif
  ///////////////////////// DL Manager //////////////////////////
  setupDLManagerCicada();
  printMinorDivide();
  printMajorDivide("Setup Loop Finished");

  /////////////////////////////// Main Loop Delay ////////////////////////////////
  
  // Serial.print("segment : ");
  // Serial.println(segment);
  for (int it = 0; it < NUM_LED; it++) {
    // we keep readng the jumpers so if we change the jumpers and dont
    // want the boot delay that can happen
    readJumpers();
    uint32_t segment = (uint32_t)((double)BOOT_DELAY / (double)NUM_LED);
    leds.setPixel(it, 10, 32, 20);
    leds.show();
    delay(segment);
  }
  for (int it = 0; it < NUM_LED; it++) {
    leds.setPixel(it, 0, 0, 0);
  }
  leds.show();
  printMajorDivide("Now starting main() loop");
}

void loop() {
  // loop_tmr is used when frame limiting the program and determining what the FPS is for the program
  if (loop_tmr > loop_length) {
    updateLuxManagers();
    if (lux_manager.getExtremeLux() == true) {
      dprintln(P_LUX, "WARNING ------------ updateMode() returning due extreme lux conditions, not updating onset or song...");
    } else {
      updateFeatureCollectors();
      updateMode();
      updateAutogain();
      updateDatalog();
      readUserControls();
      loop_tmr = 0;
    }
  }
}
