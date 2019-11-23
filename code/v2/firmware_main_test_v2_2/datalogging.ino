void datalogPrint(String data) {
  if (PRINT_LOG_WRITE) {
    Serial.print(data);
  }
}

void datalogPrintLn() {
  if (PRINT_LOG_WRITE) {
    Serial.println();
  }
}

void datalogPrintLn(String data) {
  if (PRINT_LOG_WRITE) {
    Serial.println(data);
  }
}

///////////////////////////////////////////////////////////////////////
//                            Datalogging
///////////////////////////////////////////////////////////////////////
void writeLongToEEPROM(int addr, uint32_t data) {
  if (data_logging_active) {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
      b[i] = data >> 8 * i;
      EEPROM.write(addr + i, b[i]);
    }
  }
}

uint32_t readLongFromEEPROM(int addr) {
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t n = EEPROM.read(addr + i) << 8 * i;
    data = n | data;
  }
  return data;
}

void writeShortToEEPROM(unsigned int addr, uint16_t data) {
  if (data_logging_active) {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = data & 0x00FF;
    uint8_t msb = data >> 8;
    EEPROM.write(addr, lsb);
    EEPROM.write(addr + 1, msb);
  }
}

uint16_t readShortFromEEPROM(unsigned int addr) {
  uint16_t data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return data;
}

void writeDoubleToEEPROM(unsigned int addr, double data) {
  // to do this is aweful, need to rewrite todo
  if (data_logging_active) {
    // store the dat with least significant bytes in lower index
    uint8_t b[4];
    uint32_t d = data * 10000;
    for (int i = 0; i < 4; i++) {
      b[i] = (d >> i*8) & 0x00FF;
      EEPROM.write(addr+i, b[i]);
    }
    // Serial.println("double wrote to EEPROM");
  }
  if (EEPROM_WRITE_CHECK && data_logging_active){
    // 
    // Serial.print(b[i], BIN);
    Serial.print("data check:\t");
    Serial.print(data);
    Serial.print("\t");
    Serial.println(readDoubleFromEEPROM(addr));
  }
}

double readDoubleFromEEPROM(unsigned int addr) {
  uint32_t data = EEPROM.read(addr+3);  
  for (int i = 2; i > -1; i--) {
    uint8_t reading = EEPROM.read(addr+i);
    // Serial.print(reading);
    // Serial.print("|");
    data = (data << 8) | reading;
  }
  return (double)data / 10000.0;
}

void writeSetupConfigsToEEPROM() {
  if (data_logging_active) {
    EEPROM.write(EEPROM_JMP1, cicada_mode);
    EEPROM.write(EEPROM_JMP2, stereo_audio);
    EEPROM.write(EEPROM_JMP3, both_lux_sensors);
    EEPROM.write(EEPROM_JMP4, combine_lux_readings);
    EEPROM.write(EEPROM_JMP5, gain_adjust_active);
    EEPROM.write(EEPROM_JMP6, data_logging_active);
    datalogPrintLn("logged jumper values to EEPROM");

    // log the starting gains to the min/max EEPROM
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_REAR, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN_FRONT, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN_REAR, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_START_FRONT, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_START_REAR, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_START_FRONT, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_START_REAR, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_REAR, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX_FRONT, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX_REAR, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT_FRONT, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT_REAR, song_gain[1]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT_FRONT, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT_REAR, click_gain[1]);

    writeLongToEEPROM(EEPROM_TOTAL_CLICKS_FRONT, 0);
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS_REAR, 0);

    EEPROM.write(EEPROM_AUDIO_MEM_MAX, AUDIO_MEMORY);
    datalogPrintLn("logged AUDIO_MEMORY to EEPROM");

    // auto-log values
    EEPROM.write(EEPROM_LOG_ACTIVE, data_logging_active);
    writeLongToEEPROM(EEPROM_LOG_POLLING_RATE, LOG_POLLING_RATE);
    writeLongToEEPROM(EEPROM_LOG_START_TIME, LOG_START_DELAY);
    writeLongToEEPROM(EEPROM_LOG_END_TIME,  LOG_START_DELAY + LOG_TIME_FRAME);
    datalogPrintLn("Logged Log info:");
    datalogPrint("Datalog Active :\t"); datalogPrintLn(data_logging_active);
    datalogPrint("Start time     :\t"); datalogPrintLn(LOG_START_DELAY / 60000);
    datalogPrint("End time       :\t"); datalogPrintLn((LOG_START_DELAY + LOG_TIME_FRAME) / 60000);
    datalogPrint("Logging Rate   :\t"); datalogPrintLn(LOG_POLLING_RATE / 60000);

    EEPROM.write(EEPROM_SERIAL_ID, SERIAL_ID);
    datalogPrint("logged serial number : ");
    datalogPrintLn(SERIAL_ID);
    datalogPrintLn("\nFinished logging setup configs to EEPROM");
    datalogPrintLn("|||||||||||||||||||||||||||||||||||||||||");
  }
}

void writeAudioUsageToEEPROM(uint8_t used) {
  if (data_logging_active) {
    EEPROM.write(EEPROM_AUDIO_MEM_USAGE, used);
    datalogPrintLn("logged audio memory usage to EEPROM");
  }
}

void writeTotalClicksToEEPROM() {
  if (data_logging_active) {
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS_FRONT, total_clicks_detected[0]);
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS_REAR, total_clicks_detected[1]);
    datalogPrint("Updated EEPROM with total clicks detected front/rear: ");
    datalogPrint(total_clicks_detected[0]);
    datalogPrint("\t"); datalogPrintLn(total_clicks_detected[1]);
  }
}

void updateRuntimeAndClicks() {
  if (last_runtime_update > RUNTIME_POLL_DELAY) {
    // datalogPrint("time to update runtime");
    writeRuntimeToEEPROM();
    writeTotalClicksToEEPROM();
    last_runtime_update = 0;
  }
}

void writeRuntimeToEEPROM() {
  if (data_logging_active) {
    unsigned long t = millis();
    writeLongToEEPROM(EEPROM_RUN_TIME, t);
    datalogPrint("updated the runtime to EEPROM (in minutes): ");
    datalogPrintLn(t / 60000);
  }
}

uint8_t updateBrightnessAverageLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    Serial.print("Logging the average brightness scalers  :");
    for (int  i = 0; i < 3; i++) {
      Serial.print("\t");
      writeDoubleToEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4), avg_brightness_readings[i]);
      Serial.print(avg_brightness_readings[i]);
    }
    Serial.println();
    return 1;
  }
  return 0;
}

void printBrightnessAverageLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  Serial.print("Printing the average brightness scalers:");
  for (int  i = 0; i < 3; i++) {
    Serial.print("\t");
    Serial.print(readDoubleFromEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4)));
  }
  Serial.println();
}

uint8_t updateOnRateLog() {
  // TODO
  if (log_timer > LOG_POLLING_RATE) {
    Serial.print("Logging the LED on rates                :");
    for (int  i = 0; i < 2; i++) {
      Serial.print("\t");
      writeDoubleToEEPROM(EEPROM_LED_ON_RATIO + (i * 4), led_on_ratio[i]);
      Serial.print(led_on_ratio[i]);
    }
    Serial.println();
    return 1;
  }
  return 0;
}

void printOnRatioLog() {
  Serial.print("Reading the LED on rates                :");
  for (int  i = 0; i < 2; i++) {
    Serial.print("\t");
    Serial.print(readDoubleFromEEPROM(EEPROM_LED_ON_RATIO + (i * 4)));
  }
  Serial.println();
}

void updateLuxMinMax() {
  // give the program some time to settle
  if (data_logging_active && millis() > 20000) {
    // front
    for (int i = 0; i < num_lux_sensors; i++) {
      // is the current reading more than the max recorded?
      if (lux[i] > max_lux_reading[i]) {
        max_lux_reading[i] = lux[i];
        writeDoubleToEEPROM(EEPROM_MAX_LUX_READINGS + (i * 4) , max_lux_reading[i]);
        datalogPrint("logged new "); datalogPrint(i);
        datalogPrint(" max_lux_reading to EEPROM at addr: "); datalogPrint(EEPROM_MAX_LUX_READINGS + (i * 4));
        datalogPrint(" :\t"); datalogPrintLn(max_lux_reading[i]);
        // datalogPrint(" read back:\t");datalogPrintLn(readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + (i*4)));
      } // is the current reading less than the max recorded?
      if (lux[i] < min_lux_reading[i] && lux[i] != 0) {
        if (lux[i] > 0) {
          min_lux_reading[i] = lux[i];
          writeDoubleToEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4) , min_lux_reading[i]);
          datalogPrint("logged new "); datalogPrint(i);
          datalogPrint(" min_lux_reading to EEPROM\t"); datalogPrint(min_lux_reading[i]);
          datalogPrint(" read back:\t");datalogPrintLn(readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + (i*4)));
        }
      }
    }
    // combined
    if (lux[2] > max_lux_reading[2]) {
      max_lux_reading[2] = lux[2];
      writeDoubleToEEPROM(EEPROM_MAX_LUX_READING_COMBINED, max_lux_reading[2]);
      datalogPrint("logged new combined max_lux_reading to EEPROM\t"); datalogPrintLn(max_lux_reading[2]);
    } else if (lux[2] < min_lux_reading[2]) {
      min_lux_reading[2] = lux[2];
      writeDoubleToEEPROM(EEPROM_MIN_LUX_READING_COMBINED , min_lux_reading[2]);
      datalogPrint("logged new combined min_lux_reading to EEPROM\t"); datalogPrintLn(max_lux_reading[2]);
    }
  }
}

void printAndClearDoubleLog(uint32_t _start, uint32_t _end, String _name) {
  if (data_logging_active) {
    datalogPrint("\nClearing and Reading the "); Serial.print(_name);
  } else {
    datalogPrint("\nReading the ");
  }
  datalogPrint("Log:");
  datalogPrint("\nFRONT\n");
  for (int side = 0; side < 5; side = side + 4) {
    if (side > 0) {
      datalogPrint("\nREAR\n");
    }
    for (int i = _start + side; i < _end; i += 8) {
      double val = readDoubleFromEEPROM(i);
      datalogPrintLn();
      datalogPrint(i);
      datalogPrint(",");
      datalogPrint(val);
      datalogPrint("\t");
      if (data_logging_active) {
        writeDoubleToEEPROM(i, 0.0);
      }

    }
    datalogPrintLn();
  }
}; // TODO

void updateEEPROMLogs() {
  if (data_logging_active) {
    // if enough time has passed since init for data-logging to be active
    uint8_t updates = 0;
    if (millis() > LOG_START_DELAY) {
      //  if one of the logs update then update the log timer
      updates += updateOnRateLog();
      updates += updateBrightnessAverageLog();
      updates += updateLuxLog();
      updates += updateCPMLog();
    }
    if (updates) {
      log_timer = 0;
      Serial.print("A Total of "); Serial.print(updates); Serial.println(" logs were updated");
      printDivide();
    }
  }
}

uint8_t updateLuxLog() {
  // if datalogging is active
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && lux_eeprom_idx < EEPROM_LUX_LOG_END) {
    double lux_average[2];
    for (int i = 0; i < 2; i++) {
      lux_average[i] = lux_total[i] / (double)lux_readings[i];
      // reset the average values
      lux_total[i] = 0;
      lux_readings[i] = 0;
    }
    datalogPrint("Logging Lux Data into EEPROM location: "); datalogPrintLn(lux_eeprom_idx);
    datalogPrint("lux_average :\t"); datalogPrint(lux_average[0]); datalogPrint("\t");
    datalogPrintLn(lux_average[1]);
    // store the current lux readings
    // increment the index, 4 bytes to a double
    writeDoubleToEEPROM(lux_eeprom_idx, lux_average[0]);
    lux_eeprom_idx += 4;
    writeDoubleToEEPROM(lux_eeprom_idx, lux_average[1]);
    lux_eeprom_idx += 4;
    return 1;
  }
}

uint8_t updateCPMLog() {
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    double cpm[2];
    for (int i = 0; i < 2; i++) {
      datalogPrint("num_cpm_clicks / cpm :\t"); datalogPrint(i); datalogPrint("\t:\t");
      datalogPrint(num_cpm_clicks[i]);
      cpm[i] = (double)num_cpm_clicks[i] / ((double)log_timer / 60000);
      datalogPrint("\t/\t"); datalogPrintLn(cpm[i]);
      // reset the average values
      num_cpm_clicks[i] = 0;
    }
    datalogPrint("Logging CPM Data into EEPROM location: "); datalogPrintLn(cpm_eeprom_idx);
    datalogPrint("cpm :\t"); datalogPrint(cpm[0]); datalogPrint("\t");
    datalogPrintLn(cpm[1]);
    // store the current lux readings
    // increment the index, 4 bytes to a double
    writeDoubleToEEPROM(cpm_eeprom_idx, cpm[0]);
    cpm_eeprom_idx += 4;
    writeDoubleToEEPROM(cpm_eeprom_idx, cpm[1]);
    cpm_eeprom_idx += 4;
    return 1;
  }
}
/*
  void readLuxLogFromEEPROM() {
  Serial.print("luxlog as stored in EEPROM");
  for (int i = EEPROM_LUX_LOG; i < LUX_LOG_LAST_ADDR; i  += 4) {
    double val = readDoubleFromEEPROM(i);
    if ( val > 0) {
      Serial.print(i - 1000 / 4); Serial.print("\t"); Serial.print(val); datalogPrintLn("\t");
      if (i % 20 ==  0) {
        datalogPrintLn();
      }
    }
  }
  }
*/
void printEEPROMContents() {
  // todo add printing for the serial_id
  printMajorDivide((String)"Printing EEPROM CONTENTS ...");
  Serial.print("bot serial number   :\t");
  Serial.println(EEPROM.read(EEPROM_SERIAL_ID));
  Serial.print("run time in ms      :\t");
  Serial.println(readLongFromEEPROM(EEPROM_RUN_TIME));
  Serial.print("run time in minutes :\t");
  double rt = readLongFromEEPROM(EEPROM_RUN_TIME) / 1000 / 60;
  Serial.println(rt);
  printMinorDivide();
  Serial.print("onboard jumper settings\t\t");
  Serial.print(EEPROM.read(EEPROM_JMP1));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP2));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP3));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP4));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP5));
  Serial.print("\t");
  Serial.println(EEPROM.read(EEPROM_JMP6));
  printMinorDivide();

  Serial.println("\nDatalogging settings");
  printMinorDivide();
  Serial.print("Datalog Active :\t"); Serial.println(EEPROM.read(EEPROM_LOG_ACTIVE));
  Serial.print("Start time     :\t"); Serial.println((double)readLongFromEEPROM(EEPROM_LOG_START_TIME) / 60000);
  Serial.print("End time       :\t"); Serial.println((double)readLongFromEEPROM(EEPROM_LOG_END_TIME) / 60000);
  Serial.print("Logging Rate   :\t"); Serial.println((double)readLongFromEEPROM(EEPROM_LOG_POLLING_RATE) / 60000);

  Serial.println("\nAudio Settings");
  printMinorDivide();
  Serial.print("Audio memory usage/max            :\t"); Serial.print(EEPROM.read(EEPROM_AUDIO_MEM_USAGE));
  Serial.print("\t"); Serial.println(EEPROM.read(EEPROM_AUDIO_MEM_MAX));

  Serial.println("\n - CLICK");
  printMinorDivide();
  Serial.print("front/read starting gain          :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START_REAR));
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT));
  Serial.print("front min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT));
  Serial.print("rear  min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN_REAR)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX_REAR));
  Serial.print("last recorded gain front/rear     :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_CURRENT_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_CURRENT_REAR));
  Serial.print("total clicks detected front/rear  :\t");
  Serial.print(readLongFromEEPROM(EEPROM_TOTAL_CLICKS_FRONT)); Serial.print("\t");
  Serial.println(readLongFromEEPROM(EEPROM_TOTAL_CLICKS_REAR));
  Serial.print("Average number of CPM front/rear  :\t");
  Serial.print(readLongFromEEPROM(EEPROM_TOTAL_CLICKS_FRONT) / rt); Serial.print("\t");
  Serial.println(readLongFromEEPROM(EEPROM_TOTAL_CLICKS_REAR) / rt);
  printAndClearDoubleLog(EEPROM_CPM_LOG_START, EEPROM_CPM_LOG_END, "Clicks Per Minute ");

  Serial.println("\n -  SONG  ");
  printMinorDivide();
  Serial.print("front/read starting gain          :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_START_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_START_REAR));
  Serial.print("front min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MIN_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MAX_FRONT));
  Serial.print("rear min/max recorded gain        :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MIN_REAR)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MAX_REAR));
  Serial.print("last recorded gain front/rear     :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_CURRENT_FRONT)); Serial.print("\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_CURRENT_REAR)); Serial.println();

  Serial.println("\nLux Settings\n");
  printDivide();
  Serial.print("min/max front lux reading         : \t");
  Serial.print(readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS));
  Serial.print("min/max rear lux reading          : \t");
  Serial.print(readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + 4)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + 4));
  Serial.print("min/max combined lux reading      : \t");
  Serial.print(readDoubleFromEEPROM(EEPROM_MIN_LUX_READING_COMBINED)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_MAX_LUX_READING_COMBINED));
  printAndClearDoubleLog(EEPROM_LUX_LOG_START, EEPROM_LUX_LOG_END, " LUX ");

  Serial.println("\nLED Settings");
  printDivide();
  printOnRatioLog();
  printBrightnessAverageLog();

  printMajorDivide("Finished Printing EEPROM Datalog");
}

void printTeensyDivide() {
  Serial.println("- - - - - - - - - - - - - - - - - - -\n");
}

void printMinorDivide() {
  Serial.println("-------------------------------------\n");
}

void printDivide() {
  Serial.println("|||||||||||||||||||||||||||||||||||||\n");
}

void printMajorDivide(String text) {
  Serial.println("|||||||||||||||||||||||||||||||||||||");
  if (text != "") {
    Serial.println(text);
  } else {
    Serial.println();
  };
  Serial.println("|||||||||||||||||||||||||||||||||||||\n");
}
