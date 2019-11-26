/*
  #ifndef __DATALOG_CONF_H__
  #define __DATALOG_CONF_H__
*/

void writeLongToEEPROM(int addr, uint32_t data) {
  if (data_logging_active) {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
      b[i] = data >> 8 * i;
      EEPROM.update(addr + i, b[i]);
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
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
  }
}

uint16_t readShortFromEEPROM(unsigned int addr) {
  uint16_t data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return data;
}

double readDoubleFromEEPROM(unsigned int addr) {
  uint32_t data = EEPROM.read(addr + 3);
  for (int i = 2; i > -1; i--) {
    uint8_t reading = EEPROM.read(addr + i);
    // Serial.print(reading);
    // Serial.print("|");
    data = (data << 8) | reading;
  }
  return (double)data / DOUBLE_PRECISION;
}

void writeDoubleToEEPROM(unsigned int addr, double data) {
  // to do this is aweful, need to rewrite todo
  if (data_logging_active) {
    // store the dat with least significant bytes in lower index
    uint8_t b[4];
    uint32_t d = data * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
      b[i] = (d >> i * 8) & 0x00FF;
      EEPROM.update(addr + i, b[i]);
    }
    // Serial.println("double wrote to EEPROM");
  }
  if (EEPROM_WRITE_CHECK && data_logging_active) {
    //
    // Serial.print(b[i], BIN);
    Serial.print("data check:\t");
    Serial.print(data);
    Serial.print("\t");
    Serial.println(readDoubleFromEEPROM(addr));
  }
}

void writeAudioUsageToEEPROM(uint8_t used) {
  if (data_logging_active) {
    EEPROM.update(EEPROM_AUDIO_MEM_USAGE, used);
    dprintln(PRINT_LOG_WRITE, "logged audio memory usage to EEPROM");
  }
}

void writeRuntimeToEEPROM() {
  if (data_logging_active) {
    unsigned long t = millis();
    writeLongToEEPROM(EEPROM_RUN_TIME, t);
    dprint(PRINT_LOG_WRITE, "updated the runtime to EEPROM (in minutes): ");
    dprintln(PRINT_LOG_WRITE, (String)(t / 60000));
  }
}

// break this into two functions todo
void printAndClearDoubleLog(uint32_t _start, uint32_t _end, String _name) {
  if (data_logging_active) {
    dprint(PRINT_LOG_WRITE, "\nClearing and Reading the "); Serial.print(_name);
  } else {
    dprint(PRINT_LOG_WRITE, "\nReading the ");
  }
  dprint(PRINT_LOG_WRITE, "Log:");
  dprint(PRINT_LOG_WRITE, "\nFRONT\n");
  for (unsigned int side = 0; side < 5; side = side + 4) {
    if (side > 0) {
      dprint(PRINT_LOG_WRITE, "\nREAR\n");
    }
    for (unsigned int i = _start + side; i < _end; i += 8) {
      double val = readDoubleFromEEPROM(i);
      //  dprint(PRINT_LOG_WRITE, "");
      // dprint(PRINT_LOG_WRITE, i);
      dprint(PRINT_LOG_WRITE, ",");
      dprint(PRINT_LOG_WRITE, val);
      if (data_logging_active) {
        writeDoubleToEEPROM(i, 0.0);
      }

    }
    dprintln(PRINT_LOG_WRITE, "");
  }
} // TODO

uint8_t updateLuxLog() {
  // if datalogging is active
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && lux_eeprom_idx < EEPROM_LUX_LOG_END) {
    dprint(PRINT_LOG_WRITE, "Logging Lux Data into EEPROM location: "); dprintln(PRINT_LOG_WRITE, (int)lux_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "lux_average                          :\t");  
    // store the current lux readings
    // increment the index, 4 bytes to a double
    if (front_lux_active) {
      dprint(PRINT_LOG_WRITE, lux_sensors[0].getAvgLux()); 
      writeDoubleToEEPROM(lux_eeprom_idx, lux_sensors[0].getAvgLux());
      lux_eeprom_idx += 4;
    } else{
      dprint(PRINT_LOG_WRITE, "Front sensor deactivated ");
    }
    if (rear_lux_active) {
      writeDoubleToEEPROM(lux_eeprom_idx, lux_sensors[1].getAvgLux());
      lux_eeprom_idx += 4;
      dprint(PRINT_LOG_WRITE, "\t");
      dprintln(PRINT_LOG_WRITE, lux_sensors[1].getAvgLux());
    } else {
      dprint(PRINT_LOG_WRITE, "Front sensor deactivated ");
    }
    return 1;
  }
  return 0;
}

bool updateLuxMinMaxDatalog() {
  // give the program some time to settle
  if (data_logging_active && millis() > 20000) {
    // front
    for (int i = 0; i < NUM_LUX_SENSORS; i++) {
      // is the current reading more than the max recorded?
      // if the current reading is the same then nothing is written, if it is different something is writen
      writeDoubleToEEPROM(EEPROM_MAX_LUX_READINGS + (i * 4) , lux_sensors[i].getMaxLux());
      dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, lux_sensors[i].getName());
      dprint(PRINT_LOG_WRITE, " max_lux_reading to EEPROM at addr: "); dprint(PRINT_LOG_WRITE, EEPROM_MAX_LUX_READINGS + (i * 4));
      dprint(PRINT_LOG_WRITE, " :\t"); dprintln(PRINT_LOG_WRITE, lux_sensors[i].getMaxLux());
      dprint(PRINT_LOG_WRITE, " read back:\t");dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + (i*4)));

      writeDoubleToEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4) , lux_sensors[i].getMinLux());
      dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, lux_sensors[i].getName());
      dprint(PRINT_LOG_WRITE, " min_lux_reading to EEPROM\t"); dprint(PRINT_LOG_WRITE, lux_sensors[i].getMinLux());
      dprint(PRINT_LOG_WRITE, " read back:\t"); dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4)));
    }
  }
  // combined
  if (combined_lux > combined_max_lux_reading) {
    combined_max_lux_reading = combined_lux;
    writeDoubleToEEPROM(EEPROM_MAX_LUX_READING_COMBINED, combined_max_lux_reading);
    dprint(PRINT_LOG_WRITE, "logged new combined max_lux_reading to EEPROM\t"); dprintln(PRINT_LOG_WRITE, combined_max_lux_reading);
    return 1;
  } else if (combined_lux < combined_min_lux_reading) {
    combined_min_lux_reading = combined_lux;
    writeDoubleToEEPROM(EEPROM_MIN_LUX_READING_COMBINED , combined_min_lux_reading);
    dprint(PRINT_LOG_WRITE, "logged new combined min_lux_reading to EEPROM\t"); dprintln(PRINT_LOG_WRITE, combined_max_lux_reading);
    return 1;
  }
  return 0;
}

uint8_t updateBrightnessScalerAvgLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    dprint(PRINT_LOG_WRITE,"Logging the average brightness scalers  :");
    for (int  i = 0; i < 2; i++) { // replace 2 with something variable
      dprint(PRINT_LOG_WRITE,"\t");
      writeDoubleToEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4), neos[i].getAvgBrightnessScaler());
      dprint(PRINT_LOG_WRITE,neos[i].getAvgBrightnessScaler());
    }
    dprintln(PRINT_LOG_WRITE);
    return 1;
  }
  return 0;
}

void printBrightnessAverageLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  Serial.print("Printing the average brightness scalers :");
  for (int  i = 0; i < 3; i++) {
    Serial.print("\t");
    Serial.print(readDoubleFromEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4)));
  }
  Serial.println();
}

uint8_t updateOnRateLog() {
  // TODO
  if (log_timer > LOG_POLLING_RATE) {
    dprint(PRINT_LOG_WRITE, "Logging the LED on rates                :");
    for (int  i = 0; i < 2; i++) {
      dprint(PRINT_LOG_WRITE,"\t");
      writeDoubleToEEPROM(EEPROM_LED_ON_RATIO + (i * 4), neos[i].getOnRatio());
      Serial.print(neos[i].getOnRatio());
    }
    dprintln(PRINT_LOG_WRITE);
    return 1;
  }
  return 0;
}

void printOnRatioLog() {
  dprint(PRINT_LOG_WRITE,"Reading the LED on rates                :");
  for (int  i = 0; i < 2; i++) {
    dprint(PRINT_LOG_WRITE,"\t");
    dprint(PRINT_LOG_WRITE,readDoubleFromEEPROM(EEPROM_LED_ON_RATIO + (i * 4)));
  }
  dprintln(PRINT_LOG_WRITE);
}

//#ifndef __LOGGING_CICADA_MODE_H__
//#define __LOGGING_CICADA_MODE_H__

void updateSongGain(double song_gain[]) {
  // todo right now it just updates the song gain for everything
  for (unsigned int i = 0; i < num_channels; i++) {
    if (i == 0) {
      song_input_amp1.gain(song_gain[i]);
      song_mid_amp1.gain(song_gain[i]);
      song_post_amp1.gain(song_gain[i]);
      dprint(PRINT_SONG_DATA, "\nupdated song gain "); dprint(PRINT_SONG_DATA, frText(i));dprint(PRINT_SONG_DATA, ":\t");
      dprintln(PRINT_SONG_DATA, song_gain[i]);
      writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT + (i * 4), song_gain[i]);
    } else if (i == 1) {
      song_input_amp2.gain(song_gain[1]);
      song_mid_amp2.gain(song_gain[1]);
      song_post_amp2.gain(song_gain[1]);
      dprint(PRINT_SONG_DATA, "\nupdated song gain "); dprint(PRINT_SONG_DATA, frText(i)); dprint(PRINT_SONG_DATA, ":\t");
      dprintln(PRINT_SONG_DATA, song_gain[i]);
      writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT + (i * 4), song_gain[i]);
    }
  }
}

void updateSongGainMinMax() {
  if (song_gain[0] > song_gain_max[0]) {
    song_gain_max[0] = song_gain[0];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX, song_gain_max[0]);
    dprint(PRINT_SONG_DATA, "logged new front song gain high : "); dprintln(PRINT_SONG_DATA, song_gain_max[0]);
  } else if (song_gain[0] < song_gain_min[0]) {
    song_gain_min[0] = song_gain[0];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN, song_gain_min[0]);
    dprint(PRINT_SONG_DATA, "logged new front song gain low  : "); dprintln(PRINT_SONG_DATA, song_gain_min[0]);
  }
  if (song_gain[1] > song_gain_max[1]) {
    song_gain_max[1] = song_gain[1];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX + 4, song_gain_max[1]);
    dprint(PRINT_SONG_DATA, "logged new rear  song gain high : "); dprintln(PRINT_SONG_DATA, song_gain_max[1]);
  } else if (song_gain[1] < song_gain_min[1]) {
    song_gain_min[1] = song_gain[1];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN + 4, song_gain_min[1]);
    dprint(PRINT_SONG_DATA, "logged new rear song gain low   : "); dprintln(PRINT_SONG_DATA, song_gain_min[1]);
  }
}

void writeTotalClicksToEEPROM() {
  if (data_logging_active) {
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS, total_clicks_detected[0]);
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS + 4, total_clicks_detected[1]);
    dprint(PRINT_LOG_WRITE, "Updated EEPROM with total clicks detected front/rear: ");
    dprint(PRINT_LOG_WRITE, (int)total_clicks_detected[0]);
    dprint(PRINT_LOG_WRITE, "\t"); dprintln(PRINT_LOG_WRITE, (int)total_clicks_detected[1]);
  }
}

void updateRuntimeAndClicks() {
  if (last_runtime_update > RUNTIME_POLL_DELAY) {
    // dprint(PRINT_LOG_WRITE,"time to update runtime");
    writeRuntimeToEEPROM();
    writeTotalClicksToEEPROM();
    last_runtime_update = 0;
  }
}

uint8_t updateCPMLog() {
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    double cpm[2];
    for (int i = 0; i < 2; i++) {
      dprint(PRINT_LOG_WRITE, "num_cpm_clicks / cpm "); 
      if ( i == 0) {
        dprint(PRINT_LOG_WRITE, "Front           :\t");
      } else{
        dprint(PRINT_LOG_WRITE, "Rear            :\t");
      }
      dprint(PRINT_LOG_WRITE, num_cpm_clicks[i]);
      cpm[i] = (double)num_cpm_clicks[i] / ((double)log_timer / 60000);
      dprint(PRINT_LOG_WRITE, "/"); dprintln(PRINT_LOG_WRITE, cpm[i]);
      // reset the average values
      num_cpm_clicks[i] = 0;
    }
    dprint(PRINT_LOG_WRITE, "Logging CPM Data into EEPROM location: "); dprint(PRINT_LOG_WRITE, cpm_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "\tcpm :\t"); dprint(PRINT_LOG_WRITE, cpm[0]); dprint(PRINT_LOG_WRITE, "\t");
    dprintln(PRINT_LOG_WRITE, cpm[1]);
    dprintMinorDivide(PRINT_LOG_WRITE);
    // store the current lux readings
    // increment the index, 4 bytes to a double
    writeDoubleToEEPROM(cpm_eeprom_idx, cpm[0]);
    cpm_eeprom_idx += 4;
    writeDoubleToEEPROM(cpm_eeprom_idx, cpm[1]);
    cpm_eeprom_idx += 4;
    return 1;
  }
  return 0;
}
/*
  void readLuxLogFromEEPROM() {
  Serial.print("luxlog as stored in EEPROM");
  for (int i = EEPROM_LUX_LOG; i < LUX_LOG_LAST_ADDR; i  += 4) {
    double val = readDoubleFromEEPROM(i);
    if ( val > 0) {
      Serial.print(i - 1000 / 4); Serial.print("\t"); Serial.print(val); dprintln(PRINT_LOG_WRITE, "\t");
      if (i % 20 ==  0) {
        dprintln(PRINT_LOG_WRITE, );
      }
    }
  }
  }
*/
void updateEEPROMLogs() {
  if (data_logging_active) {
    // if enough time has passed since init for data-logging to be active
    uint8_t updates = 0;
    if (millis() > LOG_START_DELAY) {
      //  if one of the logs update then update the log timer
      updates += updateOnRateLog();
      updates += updateBrightnessScalerAvgLog();
      updates += updateLuxLog();
      updates += updateCPMLog();
      updates += updateLuxMinMaxDatalog();
    }
    if (updates) {
      log_timer = 0;
      Serial.print("A Total of "); Serial.print(updates); Serial.println(" logs were updated");
      printDivide();
    }
  }
}

void printEEPROMContents() {
  // todo add printing for the serial_id
  printMajorDivide((String)"Printing EEPROM CONTENTS ...");
  Serial.print("hardware version    :\t");Serial.print(EEPROM.read(EEPROM_H_VERSION_MAJOR));
  Serial.print(".");Serial.println(EEPROM.read(EEPROM_H_VERSION_MINOR));
  
  Serial.print("firmware version    :\t");Serial.print(EEPROM.read(EEPROM_S_VERSION_MAJOR));
  Serial.print(".");Serial.print(EEPROM.read(EEPROM_S_VERSION_MINOR));Serial.print(".");
  Serial.println(EEPROM.read(EEPROM_S_SUBVERION));
  
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
  Serial.print("front/rear starting gain          :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START + 4));
  Serial.print("front min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX));
  Serial.print("rear  min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN + 4)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX + 4));
  Serial.print("last recorded gain front/rear     :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_CURRENT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_CURRENT + 4));
  Serial.print("total clicks detected front/rear  :\t");
  Serial.print(readLongFromEEPROM(EEPROM_TOTAL_CLICKS)); Serial.print("\t");
  Serial.println(readLongFromEEPROM(EEPROM_TOTAL_CLICKS + 4));
  Serial.print("Average number of CPM front/rear  :\t");
  Serial.print(readLongFromEEPROM(EEPROM_TOTAL_CLICKS) / rt); Serial.print("\t");
  Serial.println(readLongFromEEPROM(EEPROM_TOTAL_CLICKS + 4) / rt);
  printAndClearDoubleLog(EEPROM_CPM_LOG_START, EEPROM_CPM_LOG_END, "Clicks Per Minute ");

  Serial.println("\n -  SONG  ");
  printMinorDivide();
  Serial.print("front/read starting gain          :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_START)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_START + 4));
  Serial.print("front min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MIN)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MAX));
  Serial.print("rear min/max recorded gain        :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MIN + 4)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MAX + 4));
  Serial.print("last recorded gain front/rear     :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_CURRENT)); Serial.print("\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_CURRENT + 4)); Serial.println();

  Serial.println("\nLux Settings");
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

void writeSetupConfigsToEEPROM() {
  if (data_logging_active) {
    EEPROM.update(EEPROM_JMP1, cicada_mode);
    EEPROM.update(EEPROM_JMP2, stereo_audio);
    EEPROM.update(EEPROM_JMP3, NUM_LUX_SENSORS);
    EEPROM.update(EEPROM_JMP4, combine_lux_readings);
    EEPROM.update(EEPROM_JMP5, gain_adjust_active);
    EEPROM.update(EEPROM_JMP6, data_logging_active);
    dprintln(PRINT_LOG_WRITE, "logged jumper values to EEPROM");

    // log the starting gains to the min/max EEPROM
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN + 4, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN + 4, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_START, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_START + 4, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_START, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_START + 4, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX + 4, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX + 4, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT + 4, song_gain[1]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT + 4, click_gain[1]);

    writeLongToEEPROM(EEPROM_TOTAL_CLICKS, 0);
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS + 4, 0);


    EEPROM.update(EEPROM_AUDIO_MEM_MAX, AUDIO_MEMORY);
    dprintln(PRINT_LOG_WRITE, "logged AUDIO_MEMORY to EEPROM");

    // auto-log values
    EEPROM.update(EEPROM_LOG_ACTIVE, data_logging_active);
    writeLongToEEPROM(EEPROM_LOG_POLLING_RATE, LOG_POLLING_RATE);
    writeLongToEEPROM(EEPROM_LOG_START_TIME, LOG_START_DELAY);
    writeLongToEEPROM(EEPROM_LOG_END_TIME,  LOG_START_DELAY + LOG_TIME_FRAME);
    dprintln(PRINT_LOG_WRITE, "Logged Log info (in minutes):");
    dprint(PRINT_LOG_WRITE, "Datalog Active :\t"); dprintln(PRINT_LOG_WRITE, data_logging_active);
    dprint(PRINT_LOG_WRITE, "Start time     :\t"); dprintln(PRINT_LOG_WRITE, LOG_START_DELAY / ONE_MINUTE);
    dprint(PRINT_LOG_WRITE, "End time       :\t"); dprintln(PRINT_LOG_WRITE, (LOG_START_DELAY + LOG_TIME_FRAME) / ONE_MINUTE);
    dprint(PRINT_LOG_WRITE, "Logging Rate   :\t"); dprintln(PRINT_LOG_WRITE, (String)(LOG_POLLING_RATE / ONE_MINUTE));

    EEPROM.update(EEPROM_SERIAL_ID, SERIAL_ID);
    dprint(PRINT_LOG_WRITE, "logged serial number : ");
    dprintln(PRINT_LOG_WRITE, SERIAL_ID);

    // the software and hardware version numbers
    dprint(PRINT_LOG_WRITE, "Software Version:\t"); 
    dprint(PRINT_LOG_WRITE, S_VERSION_MAJOR);
    dprint(PRINT_LOG_WRITE,".");dprint(PRINT_LOG_WRITE, S_VERSION_MINOR);
    dprint(PRINT_LOG_WRITE,".");dprintln(PRINT_LOG_WRITE, S_SUBVERSION);
    EEPROM.update(EEPROM_S_SUBVERION, S_SUBVERSION);
    EEPROM.update(EEPROM_S_VERSION_MINOR, S_VERSION_MINOR);
    EEPROM.update(EEPROM_S_VERSION_MAJOR, S_VERSION_MAJOR);

    dprint(PRINT_LOG_WRITE, "hardware Version:\t"); 
    dprint(PRINT_LOG_WRITE, H_VERSION_MAJOR);
    dprint(PRINT_LOG_WRITE,".");dprint(PRINT_LOG_WRITE, H_VERSION_MINOR);
    EEPROM.update(EEPROM_H_VERSION_MINOR, H_VERSION_MINOR);
    EEPROM.update(EEPROM_H_VERSION_MAJOR, H_VERSION_MAJOR);
    
    dprintln(PRINT_LOG_WRITE, "\nFinished logging setup configs to EEPROM");
    dprintln(PRINT_LOG_WRITE, "|||||||||||||||||||||||||||||||||||||||||");
  }
}
