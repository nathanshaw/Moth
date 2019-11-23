//#ifndef __LOGGING_CICADA_MODE_H__
//#define __LOGGING_CICADA_MODE_H__

void updateSongGain(double song_gain[]) {
  // todo right now it just updates the song gain for everything
  for (int i = 0; i < sizeof(song_gain) / sizeof(double); i++) {
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
      dprint(PRINT_LOG_WRITE, "num_cpm_clicks / cpm :\t"); dprint(PRINT_LOG_WRITE, i); dprint(PRINT_LOG_WRITE, "\t:\t");
      dprint(PRINT_LOG_WRITE, num_cpm_clicks[i]);
      cpm[i] = (double)num_cpm_clicks[i] / ((double)log_timer / 60000);
      dprint(PRINT_LOG_WRITE, "\t/\t"); dprintln(PRINT_LOG_WRITE, cpm[i]);
      // reset the average values
      num_cpm_clicks[i] = 0;
    }
    dprint(PRINT_LOG_WRITE, "Logging CPM Data into EEPROM location: "); dprintln(PRINT_LOG_WRITE, cpm_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "cpm :\t"); dprint(PRINT_LOG_WRITE, cpm[0]); dprint(PRINT_LOG_WRITE, "\t");
    dprintln(PRINT_LOG_WRITE, cpm[1]);
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
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START + 4));
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX));
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
// #endif // __LOGGING_SONG_H__
