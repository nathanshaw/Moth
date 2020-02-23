# 1 "/Users/nathan/workspace/Victoria/Moth/code/Moth/Moth.ino"
# 1 "/Users/nathan/workspace/Victoria/Moth/code/Moth/Moth.ino"
/**
  Moth Sonic Art Framework Firmware
  Written by Nathan Villicana-Shaw in 2019
  The runtime, boot, and all other configurations are found in the Configuration.h file
*/
# 7 "/Users/nathan/workspace/Victoria/Moth/code/Moth/Moth.ino" 2

# 9 "/Users/nathan/workspace/Victoria/Moth/code/Moth/Moth.ino" 2






void updateFeatureCollectors() {
  // update the feature collectors




  for (int i = 0; i < 4; i++) {
    fc[i].update();
  }

}

void updateLuxManagers() {
  // update the feature collectors
  if (true) {
    uint8_t updated = 0;
    for (int i = 0; i < 2; i++) {
      if (lux_managers[i].update()) {
        updated++;
      }
    }

    if (updated) {
      // calculate what the combined lux is
      double combined_lux = 0;
      for (int i = 0; i < 2; i++) {
        if (combined_lux < lux_managers[i].getLux()) {
          combined_lux = lux_managers[i].getLux();
        }
        dprint(false, " combined_lux value is : ");
        dprint(false, combined_lux);
        // set the new combined lux value to both managers
        for (int i = 0; i < 2; i++) {
          lux_managers[i].setLuxValue(combined_lux);
          dprint(false, "\t");
          dprint(false, lux_managers[i].getName());
          dprint(false, " lux reading : ");
          dprint(false, lux_managers[i].getLux());
        }
      }

    }
  }
}
void updateDatalog() {
  datalog_manager.update();
  runtime = (double)millis() / 60000;
}

bool testJumpers() {
  bool populated = true;
  bool values[6];
  values[0] = digitalRead(12);
  values[1] = digitalRead(11);
  values[2] = digitalRead(14);
  values[3] = digitalRead(15);
  values[4] = digitalRead(16);
  values[5] = digitalRead(17);
  Serial.println("Testing the PCB for jumpers");
  for (int i = 0; i < 10; i++) {
    if (values[0] != digitalRead(12)){
        populated = false;
        Serial.println("JMP1_PIN returned multiple values");
    }
        if (values[1] != digitalRead(11)){
        populated = false;
        Serial.println("JMP2_PIN returned multiple values");
    }
        if (values[2] != digitalRead(14)){
        populated = false;
        Serial.println("JMP3_PIN returned multiple values");
    }
        if (values[3] != digitalRead(15)){
        populated = false;
        Serial.println("JMP4_PIN returned multiple values");
    }
        if (values[4] != digitalRead(16)){
        populated = false;
        Serial.println("JMP5_PIN returned multiple values");
    }
        if (values[5] != digitalRead(17)){
        populated = false;
        Serial.println("JMP6_PIN returned multiple values");
    }
    delay(10);
  }
  return populated;
}

void readJumpers() {
  if (testJumpers() == true) {
    Serial.println("Jumpers passed continuity test...");
    pinMode(12, 0);
    pinMode(11, 0);
    pinMode(14, 0);
    pinMode(15, 0);
    pinMode(16, 0);
    pinMode(17, 0);
    Serial.print(digitalRead(12));
    Serial.print("\t");
    Serial.print(digitalRead(11));
    Serial.print("\t");
    Serial.print(digitalRead(14));
    Serial.print("\t");
    Serial.print(digitalRead(15));
    Serial.print("\t");
    Serial.print(digitalRead(16));
    Serial.print("\t");
    Serial.println(digitalRead(17));

    //////////// Jumper 1 ///////////////////////
    bool temp_b;
    ENCLOSURE_TYPE = digitalRead(12);
    Serial.print("(pin1) Enclosure                      : ");
    Serial.println(ENCLOSURE_TYPE);
    /*
    //////////// Jumper 4 ///////////////////////
    temp_b = digitalRead(JMP4_PIN);
    if (temp_b == 0) {
      SONG_FEATURE = RMS_RAW;
    } else {
      SONG_FEATURE = PEAK_RAW;
    }
    Serial.print("(pin4) Song Feature                  : ");
    Serial.println(SONG_FEATURE);

    //////////// Jumper 5 ///////////////////////
    temp_b = digitalRead(JMP5_PIN);
    if (temp_b == 0) {
      CLICK_FEATURE = RMS_DELTA;
    } else {
      CLICK_FEATURE = PEAK_DELTA;
    }
    Serial.print("(pin5) Click Feature                 : ");
    Serial.println(CLICK_FEATURE);
    */
    ///////////// Jumper 6 //////////////////////
    temp_b = digitalRead(17);
    if (temp_b == 1) {
      MASTER_GAIN_SCALER *= 1.5;
    }
    Serial.print("(pin6) MASTER_GAIN_SCALER increased by 50% : ");
    Serial.println(MASTER_GAIN_SCALER);
  } else {
    Serial.println("ERROR - this PCB does not contain jumpers, or jumper pins are not populated");
  }
}

void setup() {
  delay(2000); // to avoid booting to the bootloader
  Serial.begin(115200);
  Serial.println("Serial begun");
  leds.begin();
  for (int i = 0; i < 10; i++) {
    leds.setPixel(i, 12, 12, 0);
    leds.show();
  }
  Serial.println("LEDS have been initalised");
  delay(3000); Serial.println("Setup Loop has started");
  if (1) {// TODO this should also check the READ_JUMPERS bool, jumpers populated should be determined by hardware revision
    readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  setupDLManager();
  setupAudio();

  for (int i = 0; i < 2; i++) {
    neos[i].setFlashColors(150, 100, 255);
  }
  for (int i = 0; i < 10; i++) {
    leds.setPixel(i, 0, 0, 0);
    leds.show();
  }
  if (true) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(0x00, 0x0C); // todo add this to config_adv? todo
    lux_managers[1].startSensor(0x00, 0x0C);
    delay(200);
    lux_managers[0].calibrate(3000);
    lux_managers[1].calibrate(3000);
  }







  printMajorDivide("Setup Loop Finished");
}

void listenForSerialCommands() {
  if (Serial.available() > 0) {
    int incByte = Serial.read();
    Serial.print("incbyte : ");
    Serial.println(incByte);
  }
}

/*
  #if PRINT_LOOP_LENGTH == true
  elapsedMicros loop_length = 0;
  unsigned long num_loops = 0;
  unsigned long loop_totals = 0;
  unsigned long longest_loop = 0;
  unsigned long shortest_loop = 0;

  void updateLoopLength() {
  if (num_loops > 0) {
    if (loop_length > longest_loop) {
      longest_loop = loop_length;
      Serial.print("new longest loop (in  micros)   : ");
      Serial.println(longest_loop);
    }
    loop_totals += loop_length;
    if (loop_totals % 1000 == 1) {
      Serial.print("average loop length (in micros) : ");
      Serial.println((double)loop_totals / (double) num_loops);
    }
  }
  num_loops++;
  loop_length = 0;
  }
  #endif//print loop length
*/

void loop() {
  updateLuxManagers();
  updateFeatureCollectors();
  updateMode();
  updateAutogain();
  updateDatalog();
  listenForSerialCommands();
  //updateLoopLength();
}
