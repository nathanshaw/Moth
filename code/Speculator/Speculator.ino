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
  fc.update();
#if P_AUDIO_MEMORY_MAX > 0
  Serial.print("audio memory max: ");
  Serial.print(AudioMemoryUsageMax());
#endif
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
  Serial.print("Reading Pots: ");
  if (NUM_POTS > 0 && last_jumper_read > USER_CONTROL_POLL_RATE) {
    for (int i = 0; i < NUM_POTS; i++) {
      pot_vals[i] = 1023 - analogRead(pot_pins[i]);
      Serial.print(pot_vals[i]);
      Serial.print("\t");
    }
  }
  Serial.println();
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
    user_brightness_scaler = scaled_brightness;
  }
  
  BRIGHTNESS_CUTTOFF_THRESHOLD = (double) pot_vals[BC_POT_NUM] / 2046;
  Serial.print("BRIGHTNESS_CUTTOFF_THRESHOLD updated: ");
  Serial.println(BRIGHTNESS_CUTTOFF_THRESHOLD); 
  
  if (USER_ONSET_THRESH_OVERRIDE == true) {
    Serial.println("WARNING USER_ONSET_THRESH_OVERRIDE is not implemented");
    // _ONSET_THRESH = mapf(_ot, 0.0, 1.0, USER_OT_MIN, USER_OT_MAX);
  }
  // if
  if (P_POT_VALS) {
    //printPots();
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
  SQUARE_BRIGHTNESS = !digitalRead(JMP1_PIN);
  Serial.print("(pin1) - ");
  if (SQUARE_BRIGHTNESS == 0) {
    Serial.print(" ON  - WILL NOT scale down brightness");
  } else {
    Serial.print(" OFF - WILL scale down brightness");
  }
  Serial.println();
#else
  SQUARE_BRIGHTNESS = !digitalRead(JMP1_PIN);
  Serial.print("(pin1) - ");
  if (SQUARE_BRIGHTNESS == 0) {
    Serial.print(" ON  - WILL NOT scale down brightness");
  } else {
    Serial.print(" OFF - WILL scale down brightness");
  }
  Serial.println();
#endif//HV_MAJOR
  //////////// Jumper 2 ///////////////////////
  /////////// Boot Delay //////////////////////
  temp_b = !digitalRead(JMP2_PIN);
  BOOT_DELAY *= temp_b;
  Serial.print("(pin2) -  ");
  if (BOOT_DELAY == 0) {
    Serial.print("OFF - ");
  } else {
    Serial.print("ON  - ");
  }
  Serial.print("Boot Delay (in seconds) = ");
  Serial.println(BOOT_DELAY / 1000);

  //////////// Jumper 3 ///////////////////////
  
  temp_b = digitalRead(JMP3_PIN);
#if FIRMWARE_MODE == CICADA_MODE
  FLASH_DOMINATES = temp_b;
  if (temp_b == 1) {
    Serial.println("(pin3)  - ON  - FLASH_DOMINATES is true, flash will erase other brightness messages");
  } else {
    Serial.println("(pin3)  - OFF - FLASH_DOMINATES is false, flash will be added to other brightness messages");
  }
#else
  Serial.print("(pin3) - ");
  if (temp_b == 0) {
    SMOOTH_HSB = 0.1;
    Serial.print(" OFF - SMOOTH_HSB is now set at 0.1");
  } else {
    SMOOTH_HSB = 0.45;
    Serial.print(" ON - SMOOTH_HSB is now set at 0.45");
  }
  Serial.println();
#endif//FIRMWARE_MODE

  //////////// Jumper 4 ///////////////////////
  //////////// Major Sensitivity Attenuation //
  
#if HV_MAJOR < 3
double total_scaler = 0.0;
  temp_b = digitalRead(JMP4_PIN);
  if (temp_b == 1) {
    Serial.print("(pin4)  - ON  - user_brightness_scaler not decreased by 66% : ");
  } else {
    Serial.print("(pin4)  - OFF - user_brightness_scaler decreased by 66% : ");
    total_scaler -= 0.66;
  }
  Serial.println(total_scaler);
#else
  temp_b = digitalRead(JMP4_PIN);
  if (temp_b == 1) {
    Serial.print("(pin4)  - ON  - TODO ");
  } else {
    Serial.print("(pin4)  - OFF - TODO ");
  }
#endif//HV_MAJOR
  ///////////// Jumper 5 //////////////////////
  //////////// Minor Sensitivity Boost ///////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(JMP5_PIN);
  if (temp_b == 1) {
    total_scaler += 1.0;
    Serial.print("(pin5)  - ON  - user_brightness_scaler increased by 100% : ");
  } else {
    Serial.print("(pin5)  - OFF - user_brightness_scaler not increased by 100% : ");
  }
  Serial.println(total_scaler);
#else
  temp_b = digitalRead(JMP5_PIN);
  if (temp_b == 1) {
    Serial.print("(pin5)  - ON  - TODO");
  } else {
    Serial.print("(pin5)  - OFF - TODO");
  }
#endif//HV_MAJOR
  ///////////// Jumper 6 //////////////////////
  //////////// Starting Gain Boost ////////////////
#if HV_MAJOR < 3
  temp_b = digitalRead(JMP6_PIN);
  if (temp_b == 1) {
    ENC_ATTENUATION_FACTOR *= 1.5;
    Serial.print("(pin6)  - ON  - STARTING_GAIN increased by a factor of 50% : ");
  } else {
    Serial.print("(pin6)  - OFF - STARTING_GAIN not increased by a factor of 50% : ");
  }
  Serial.println(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
  // this has to be done at the end only for hv 2.1
  user_brightness_scaler = 1.0;
  user_brightness_scaler += total_scaler;
  Serial.print("\nuser_brightness_scaler set to : ");
  Serial.println(user_brightness_scaler);
#else
  temp_b = digitalRead(JMP6_PIN);
  if (temp_b == 1) {
    ENC_ATTENUATION_FACTOR *= 1.5;
    Serial.print("(pin6)  - ON  - STARTING_GAIN increased by a factor of 50% : ");
  } else {
    Serial.print("(pin6)  - OFF - STARTING_GAIN not increased by a factor of 50% : ");
  }
  Serial.println(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
#endif//HV_MAJOR
#if HV_MAJOR > 2
  ///////////// Jumper 7 //////////////////////
  ///////////// Center Out Mapping ////////////
  temp_b = digitalRead(JMP7_PIN);
  if (temp_b == 1) {
    Serial.println("(pin7)  - ON  - LED_MAPPING_MODE SET TO CENTER_OUT");
    LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
  } else {
    Serial.println("(pin7)  - OFF - LED_MAPPING_MODE remains STANDARD");
    LED_MAPPING_MODE = LED_MAPPING_STANDARD;
  }
  neos.changeMapping(LED_MAPPING_MODE);

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
  LBS_ACTIVE = digitalRead(JMP10_PIN);
  if (temp_b == 1) {
    Serial.print("(pin10) - ON  - Local Brightness Scaler Active");
  } else {
    Serial.print("(pin10) - OFF - Local Brightness Scaler Off");
  }
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
  printDivide();
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
  //////////////// Eenclosure Type ///////////////////////////
  Serial.print("Enclosure type is: ");
  if (ENCLOSURE_TYPE == GROUND_ENCLOSURE){
    Serial.println("Ground");
  } else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITHOUT_HOLE) {
    Serial.println("Orb without hole");
  } else if (ENCLOSURE_TYPE == ORB_ENCLOSURE_WITH_HOLE) {
    Serial.println("Orb with hole");
  } else if (ENCLOSURE_TYPE == NO_ENCLOSURE) {
    Serial.println("no enclosure");
  } else {
    Serial.println("ERROR, enclosure type is not recognized");
    neos.colorWipe(255, 0, 0, 0);
    delay(60000);
  }
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

  ///////////////////////// auto_gain //////////////////////////
  // target value, tolerance, min_thresh, max_thresh
  auto_gain.trackAvgRMS(0.1, 0.2, 0.05, 0.2);
  Serial.println("Setting auto_gain to track RMS Avg");
  auto_gain.setUpdateRate(AUTOGAIN_FREQUENCY);
  Serial.print("Setting auto_gain's update rate to: ");
  Serial.println(AUTOGAIN_FREQUENCY);
  auto_gain.setInitialUpdateRate(AUTOGAIN_START_DELAY);
  Serial.print("Setting auto_gain's start delay to: ");
  Serial.println(AUTOGAIN_START_DELAY);

  ///////////////////////// NeoPixels //////////////////////////

  neos.setFlashColors(ONSET_RED, ONSET_GREEN, ONSET_BLUE);
  neos.setSongColors(SONG_RED_HIGH, SONG_GREEN_HIGH, SONG_BLUE_HIGH);
  neos.setFlashBehaviour(FLASH_DOMINATES);
  neos.changeMapping(LED_MAPPING_MODE);
  // neos[i].setSongFeedbackMode(ROUND);

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
  lux_manager.linkNeoGroup(&neos);
  delay(200);
  // lux_manager.calibrate(LUX_CALIBRATION_TIME);
#else
  lux_manager.addSensorTcaIdx("Front", 0);
  lux_manager.addSensorTcaIdx("Rear", 1);
  lux_manager.startTCA7700Sensors(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  if ((lux_manager.sensor_active[0] | lux_manager.sensor_active[1]) > 0) {
    lux_manager.linkNeoGroup(&neos);
    delay(200);
  }
#endif // HV_MAJOR
  lux_manager.setBrightnessScalerMinMax(BRIGHTNESS_SCALER_MIN, BRIGHTNESS_SCALER_MAX);

#if FIRMWARE_MODE == TEST_MODE
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, 64, 64, 64);
    leds.show();
  }
  delay(10000000);
#endif
  ///////////////////////// Weather Manager /////////////////////
  // nothing is needed =P

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
    if (digitalRead(JMP1_PIN)) {
      delay(segment);
    }
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
#if HV_MAJOR > 2
    weather_manager.update();
#endif // HV_MAJOR
    lux_manager.update(); // always poll the sensors
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
