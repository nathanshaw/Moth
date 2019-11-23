

void tenSecondUpdate() {
  if (ten_second_timer > TEN_SECONDS) {
    checkAudioUsage();
#if (AUTO_GAIN)
    autoGainAdjust(); // will call rear as well if in stereo mode
#endif
    updateEEPROMLogs();
    updateRuntimeAndClicks();
    ten_second_timer = 0;
  }
}

void setup() {
  Serial.begin(57600);
  leds.begin();
  delay(250);
  colorWipe(120, 70, 0, 0); // turn off the LEDs
  colorWipe(120, 70, 0, 1); // turn off the LEDs
  delay(2000);

  if (JUMPERS_POPULATED) {
    printMinorDivide();
    readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  if (PRINT_EEPROM_CONTENTS  > 0) {
    delay(1000);
    printEEPROMContents();
  }
  if (CICADA_MODE_ACTIVE) {
    cicadaSetup();
  } else if (PITCH_MODE_ACTIVE) {
    pitchSetup((); 
  }else {
  }
    while (1) {
      Serial.println("sorry the bird mode is not yet implemented");
      delay(1000);
    }
  }
  if (data_logging_active) {
    writeSetupConfigsToEEPROM();
  }
  // todo make this proper
  if (I2C_MULTI) {
    luxSetupCal(true);
  }
}

void loop() {
  checkLuxSensors(); // this needs to stay in the faster main loop
  if (cicada_mode) {
    cicadaLoop();
  } else {
    Serial.print("Error operation mode not detected : "); Serial.println(cicada_mode);
  }
  tenSecondUpdate();
