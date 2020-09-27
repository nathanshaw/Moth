
void readJumpers() {
  printMinorDivide();
  //////////// Jumper 1 ///////////////////////
  bool temp_b;
#if HV_MAJOR < 3
  SQUARE_BRIGHTNESS = !digitalRead(BUT1_PIN);
  dprint(P_USER_CONTROLS, "(but1)  - ");
  if (SQUARE_BRIGHTNESS == 0) {
    dprint(P_USER_CONTROLS, "ON  - WILL NOT square brightness");
  } else {
    dprint(P_USER_CONTROLS, "OFF - WILL square brightness");
  }
  dprintln(P_USER_CONTROLS);
#else
  SQUARE_BRIGHTNESS = !digitalRead(BUT1_PIN);
  dprint(P_USER_CONTROLS, "(but1)  - ");
  if (SQUARE_BRIGHTNESS == 0) {
    dprint(P_USER_CONTROLS, "ON - WILL NOT scale down brightness");
  } else {
    dprint(P_USER_CONTROLS, "OFF  - WILL scale down brightness");
  }
  dprintln(P_USER_CONTROLS);
#endif//HV_MAJOR
  //////////// Jumper 2 ///////////////////////
  /////////// Boot Delay //////////////////////
  temp_b = !digitalRead(BUT2_PIN);
  BOOT_DELAY *= temp_b;
  dprint(P_USER_CONTROLS, "(but2)  - ");
  if (BOOT_DELAY == 0) {
    dprint(P_USER_CONTROLS, "ON  - ");
  } else {
    dprint(P_USER_CONTROLS, "OFF - ");
  }
  dprint(P_USER_CONTROLS, "Boot Delay (in seconds) = ");
  dprintln(P_USER_CONTROLS, BOOT_DELAY / 1000);

  //////////// Jumper 3 ///////////////////////

  temp_b = digitalRead(BUT3_PIN);
#if FIRMWARE_MODE == CICADA_MODE
  FLASH_DOMINATES = temp_b;
  if (temp_b == 1) {
    dprintln(P_USER_CONTROLS, "(but3)  - ON  - FLASH_DOMINATES is true, flash will erase other brightness messages");
  } else {
    dprintln(P_USER_CONTROLS, "(but3)  - OFF - FLASH_DOMINATES is false, flash will be added to other brightness messages");
  }
#else
  if (temp_b == 0) {
    USE_TARGET_BRIGHTNESS = true;
    dprintln(P_USER_CONTROLS, "(but3)  - OFF  - USE_TARGET_BRIGHTNESS is true");
  } else {
     USE_TARGET_BRIGHTNESS = false;
     dprintln(P_USER_CONTROLS, "(but3)  - ON  - USE_TARGET_BRIGHTNESS is false");
  }
#endif//FIRMWARE_MODE

  //////////// Jumper 4 ///////////////////////
  //////////// Major Sensitivity Attenuation //

#if HV_MAJOR < 3
  double total_scaler = 0.0;
  temp_b = digitalRead(BUT4_PIN);
  if (temp_b == 1) {
    dprint(P_USER_CONTROLS, "(but4)  - ON  - user_brightness_scaler not decreased by 66% : ");
  } else {
    dprint(P_USER_CONTROLS, "(but4)  - OFF - user_brightness_scaler decreased by 66% : ");
    total_scaler -= 0.66;
  }
  dprintln(P_USER_CONTROLS, total_scaler);
#else
  temp_b = digitalRead(BUT4_PIN);
  if (temp_b == 1) {
    dprint(P_USER_CONTROLS, "(but4)  - ON  - SATURATION_FEATURE is now FEATURE_FFT_RELATIVE_ENERGY");
    SATURATION_FEATURE = FEATURE_FFT_RELATIVE_ENERGY;
  } else {
    SATURATION_FEATURE = FEATURE_FLUX;
    dprint(P_USER_CONTROLS, "(but4)  - OFF - SATURATION_FEATURE is now FEATURE_FLUX");
  }
  dprintln(P_USER_CONTROLS);
#endif//HV_MAJOR
  ///////////// Jumper 5 //////////////////////
  //////////// Minor Sensitivity Boost ///////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(BUT5_PIN);
  if (temp_b == 1) {
    total_scaler += 1.0;
    dprint(P_USER_CONTROLS, "(but5)  - ON  - user_brightness_scaler increased by 100% : ");
  } else {
    dprint(P_USER_CONTROLS, "(but5)  - OFF - user_brightness_scaler not increased by 100% : ");
  }
  dprintln(P_USER_CONTROLS, total_scaler);
#else
  temp_b = digitalRead(BUT5_PIN);
  if (temp_b == 1) {
    dprint(P_USER_CONTROLS, "(but5)  - ON  - HUE_FEATURE is now FEATURE_CENTROID");
    HUE_FEATURE = FEATURE_CENTROID;
  } else {
    dprint(P_USER_CONTROLS, "(but5)  - OFF - HUE_FEATURE is now FEATURE_FLUX");
    HUE_FEATURE = FEATURE_FLUX;
  }
  dprintln(P_USER_CONTROLS);
#endif//HV_MAJOR
  ///////////// Jumper 6 //////////////////////
  //////////// Starting Gain Boost ////////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(BUT6_PIN);
  if (temp_b == 1) {
    USER_CONTROL_GAIN_ADJUST = 1.5;
    dprint(P_USER_CONTROLS, "(but6)  - ON  - STARTING_GAIN increased by a factor of 50% : ");
  } else {
    dprint(P_USER_CONTROLS, "(but6)  - OFF - STARTING_GAIN not increased by a factor of 50% : ");
  }
  dprintln(P_USER_CONTROLS, STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
  // this has to be done at the end only for hv 2.1
  user_brightness_scaler = 1.0;
  user_brightness_scaler += total_scaler;
  dprint(P_USER_CONTROLS, "\nuser_brightness_scaler set to : ");
  dprintln(P_USER_CONTROLS, user_brightness_scaler);
#else
  temp_b = digitalRead(BUT6_PIN);
  if (temp_b == 1) {

    dprint(P_USER_CONTROLS, "(but6)  - ON  - STARTING_GAIN not increased by a factor of 50% : ");
  } else {
    dprint(P_USER_CONTROLS, "(but6)  - OFF - STARTING_GAIN increased by a factor of 50% : ");
    USER_CONTROL_GAIN_ADJUST = 1.5;
  }
  dprint(P_USER_CONTROLS, "STARTING_GAIN/ENC_GAIN_ADJ/USER_CONTROL_GAIN :\t");
  dprint(P_USER_CONTROLS, STARTING_GAIN);
  dprint(P_USER_CONTROLS, "\t");
  dprint(P_USER_CONTROLS, ENC_GAIN_ADJUST);
  dprint(P_USER_CONTROLS, "\t");
  dprint(P_USER_CONTROLS, USER_CONTROL_GAIN_ADJUST);
  dprint(P_USER_CONTROLS, "\ttotal: ");
  dprintln(P_USER_CONTROLS, STARTING_GAIN * ENC_GAIN_ADJUST * USER_CONTROL_GAIN_ADJUST);
#endif//HV_MAJOR
#if HV_MAJOR > 2
  ///////////// Jumper 7 //////////////////////
  ///////////// Center Out Mapping ////////////
  temp_b = digitalRead(BUT7_PIN);
  if (temp_b == 1) {
    dprintln(P_USER_CONTROLS, "(but7)  - ON  - LED_MAPPING_MODE SET TO CENTER_OUT");
    LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
  } else {
    dprintln(P_USER_CONTROLS, "(but7)  - OFF - LED_MAPPING_MODE remains STANDARD");
    LED_MAPPING_MODE = LED_MAPPING_STANDARD;
  }
  neos.changeMapping(LED_MAPPING_MODE);

  ///////////// Jumper 8 //////////////////////
  temp_b = digitalRead(BUT8_PIN);
  if (temp_b == 1) {
    dprint(P_USER_CONTROLS, "(but8)  - ON  - REVERSE_HUE set to false");
    REVERSE_SATURATION = false;
  } else {
    dprint(P_USER_CONTROLS, "(but8)  - OFF - REVERSE_HUE set to true");
    REVERSE_SATURATION = true;
  }
  dprintln(P_USER_CONTROLS);

  ///////////// Jumper 9 //////////////////////
  temp_b = digitalRead(BUT9_PIN);
  if (temp_b == 1) {
    dprint(P_USER_CONTROLS, "(but9)  - ON  - REVERSE_SATURATION set to false");
    REVERSE_SATURATION = false;
  } else {
    dprint(P_USER_CONTROLS, "(but9)  - OFF - REVERSE_SATURATION set to true");
    REVERSE_SATURATION = true;
  }
  dprintln(P_USER_CONTROLS);

  ///////////// Jumper 10 //////////////////////
  temp_b = digitalRead(BUT10_PIN);
  if (temp_b == 1) {
    dprint(P_USER_CONTROLS, "(but10) - ON  - REVERSE_BRIGHTNESS set to false");
    REVERSE_BRIGHTNESS = false;
  } else {
    dprint(P_USER_CONTROLS, "(but10) - OFF - REVERSE_BRIGHTNESS set to true");
    REVERSE_BRIGHTNESS = true;
  }
  dprintln(P_USER_CONTROLS);
#endif // HV_MAJOR > 2

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
  #if P_FUNCTION_TIMES 
  elapsedMicros m = 0;
  #endif
  if (JUMPERS_POPULATED != true) {
    dprintln(P_USER_CONTROLS, "Sorry jumpers not populated, exiting readUserControl()");
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
  #if P_FUNCTION_TIMES
  Serial.print("readUserControls() took ");
  Serial.print(m);
  Serial.println(" micro seconds to execute");
  #endif
}

void explainSerialCommands() {
  dprintln(P_USER_CONTROLS, "The Following Serial Commands are Supported: ");
  dprintln(P_USER_CONTROLS, "Print Commands, denoted by a p prefix: ");
  dprintln(P_USER_CONTROLS, "brightness_scaler (bs)");
  dprintln(P_USER_CONTROLS, "datalogs (dl)");
  printMinorDivide();
}

void listenForSerialCommands() {
  if (Serial.available() > 0) {
    int input = Serial.read();
    dprint(P_USER_CONTROLS, "incbyte : ");
    dprintln(P_USER_CONTROLS, input);
    //////////////// PRINT COMMANDS ////////////////
    if (input == 'p') {
      input = Serial.read();
      // datalogs
      if (input == 'd') {
        input = Serial.read();
        if (input == 'l') {
          // this is the command to print the datalog
          // datalog_manager.printAllLogs();
        }
      }
      // print brightness scaler
      if (input == 'b') {
        input = Serial.read();
        if (input == 's') {
          dprint(P_USER_CONTROLS, "Brightness Scalers: ");
          dprintln(P_USER_CONTROLS, lux_manager.getBrightnessScaler());
          // dprint(P_USER_CONTROLS, "\t");
          // dprintln(P_USER_CONTROLS, brightness_scalers[1]);
        }
      }
      /////////////// Changing Values ////////////////////////
      if (input == 's') {
        input = Serial.read();
        if (input == 'g') {
          dprintln(P_USER_CONTROLS, "what would you like to change the gain to? Please enter in a gain in the format of 1.00.");
          input = Serial.read() - 48;
          dprint(P_USER_CONTROLS, "A gain of ");
          dprint(P_USER_CONTROLS, input);
          dprintln(P_USER_CONTROLS, " has been selected");
        }
      }
    }
  }
}


#if HV_MAJOR > 2
void readPots() {
  dprint(P_USER_CONTROLS, "Reading Pots: ");
  if (NUM_POTS > 0 && last_jumper_read > USER_CONTROL_POLL_RATE) {
    for (int i = 0; i < NUM_POTS; i++) {
      pot_vals[i] = 1023 - analogRead(pot_pins[i]);
      dprint(P_USER_CONTROLS, pot_vals[i]);
      dprint(P_USER_CONTROLS, "\t");
    }
  }
  dprintln(P_USER_CONTROLS);
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
    dprint(P_USER_CONTROLS, "user brightness scaler (pre lux scaler) is now: ");
    dprintln(P_USER_CONTROLS, scaled_brightness);
    user_brightness_scaler = scaled_brightness;
  }

  BRIGHTNESS_CUTTOFF_THRESHOLD = (double) pot_vals[BC_POT_NUM] / 3400;// results in a range between 0.0 and 0.66 about
  dprint(P_USER_CONTROLS, "BRIGHTNESS_CUTTOFF_THRESHOLD updated: ");
  dprintln(P_USER_CONTROLS, BRIGHTNESS_CUTTOFF_THRESHOLD);

  if (USER_ONSET_THRESH_OVERRIDE == true) {
    dprintln(P_USER_CONTROLS, "WARNING USER_ONSET_THRESH_OVERRIDE is not implemented");
    // _ONSET_THRESH = mapf(_ot, 0.0, 1.0, USER_OT_MIN, USER_OT_MAX);
  }
  // if
  if (P_POT_VALS) {
    //printPots();
  }
}
void printPots() {
  printMinorDivide();
  dprint(P_USER_CONTROLS, "Pot vals: ");
  for (int i = 0; i < NUM_POTS; i++) {
    dprint(P_USER_CONTROLS, pot_vals[i]);
    dprint(P_USER_CONTROLS, "\t");
  }
  dprintln(P_USER_CONTROLS);
}
#endif // HV_MAJOR > 2


bool testJumpers() {
  dprintln(P_USER_CONTROLS, "Testing the PCB for jumpers");
  delay(2000);
  bool populated = true;
  bool values[10];
  values[0] = digitalRead(BUT1_PIN);
  values[1] = digitalRead(BUT2_PIN);
  values[2] = digitalRead(BUT3_PIN);
  values[3] = digitalRead(BUT4_PIN);
  values[4] = digitalRead(BUT5_PIN);
  values[5] = digitalRead(BUT6_PIN);
#if NUM_JUMPERS > 6
  values[6] = digitalRead(BUT7_PIN);
  values[7] = digitalRead(BUT8_PIN);
  values[8] = digitalRead(BUT9_PIN);
  values[9] = digitalRead(BUT10_PIN);
#endif // HV_major > 2
  printMinorDivide();

  for (int i = 0; i < 10; i++) {
    if (values[0] != digitalRead(BUT1_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT1_PIN returned multiple values");
    }
    if (values[1] != digitalRead(BUT2_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT2_PIN returned multiple values");
    }
    if (values[2] != digitalRead(BUT3_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT3_PIN returned multiple values");
    }
    if (values[3] != digitalRead(BUT4_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT4_PIN returned multiple values");
    }
    if (values[4] != digitalRead(BUT5_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT5_PIN returned multiple values");
    }
    if (values[5] != digitalRead(BUT6_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT6_PIN returned multiple values");
    }

#if HV_MAJOR > 2
    if (values[6] != digitalRead(BUT7_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT7_PIN returned multiple values");
    }
    if (values[7] != digitalRead(BUT8_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT8_PIN returned multiple values");
    }
    if (values[8] != digitalRead(BUT9_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT9_PIN returned multiple values");
    }
    if (values[9] != digitalRead(BUT10_PIN)) {
      populated = false;
      dprintln(P_USER_CONTROLS, "BUT10_PIN returned multiple values");
    }

#endif // HV_MAJOR > 2
    if (populated == true) {
      dprint(P_USER_CONTROLS, ".\t");
    }
    delay(100);
  }
  if (populated == true) {
    dprintln(P_USER_CONTROLS, "\nGreat news, the jumpers all seem to be in working order");
  } else {
    dprintln(P_USER_CONTROLS, "Crap, for some reason some of the jumpers are returning multiple values, are the headers present?");
  }
  return populated;
}
