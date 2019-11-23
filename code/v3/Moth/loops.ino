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
  Serial.println("Leds turned yellow for setup loop");
  delay(2000);

  Serial.println("Checking Hardware Jumpers");
  if (JUMPERS_POPULATED) {
    printMinorDivide();
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
  if (data_logging_active) {
    writeSetupConfigsToEEPROM();
  }
  // todo make this proper
  if (I2C_MULTI) {
    luxSetupCal(true);
  }
  printMajorDivide("Setup Loop Finished");
}

void loop() {
  checkLuxSensors(); // this needs to stay in the faster main loop
  if (cicada_mode) {
    cicadaLoop();
  } else {
    Serial.print("Error operation mode not detected : "); Serial.println(cicada_mode);
  }
  tenSecondUpdate(); // this creates a slower ten second update loop (should likely be using a timer inturrupt thing)
}
