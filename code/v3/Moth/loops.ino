/*
   Test firmware for the Moth bot
*/

///////////////////////////////////////////////////////////////////////
//                    Setup and Main Loops
///////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(57600);
  delay(5000);
  Serial.println("Setup Loop has started");
  leds.begin();
  Serial.println("LEDS have been initalised");
  delay(250);
  // create either front and back led group, or just one for both  
  neos[0].colorWipe(120, 70, 0); // turn off the LEDs
  neos[1].colorWipe(120, 70, 0); // turn off the LEDs
  Serial.println("Leds turned yellow for setup loop\n");
  delay(2000);

  if (JUMPERS_POPULATED) {
    printMinorDivide();
    Serial.println("Checking Hardware Jumpers");
    readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  if (PRINT_EEPROM_CONTENTS  > 0) {
    delay(1000);
    printEEPROMContents();
  } else {
    Serial.println("Not printing the EEPROM Datalog Contents");
  }
  Serial.println("Determining which mode to use");
  if (cicada_mode) {
    cicadaSetup();
  } else {
    while (1) {
      Serial.println("sorry the bird mode is not yet implemented");
      delay(1000);
    }
  }
  Serial.println("Testing Microphones");
  printTeensyDivide();
  testMicrophones();
  
  if (data_logging_active) {
    writeSetupConfigsToEEPROM();
  }
  Serial.println("turning off LEDs for Lux Calibration");
  // todo make this proper
  lux_sensors[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
  lux_sensors[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
  lux_sensors[0].calibrate(LUX_CALIBRATION_TIME);
  lux_sensors[1].calibrate(LUX_CALIBRATION_TIME);
  printMajorDivide("Setup Loop Finished");
}

void loop() {
  updateLuxSensors(); // this needs to stay in the faster main loop
  if (cicada_mode) {
    cicadaLoop();
  } else {
    Serial.print("Error operation mode not detected : "); Serial.println(cicada_mode);
  }
  tenSecondUpdate(); // this creates a slower ten second update loop (should likely be using a timer inturrupt thing)
}
