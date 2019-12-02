#ifndef __DATALOG_MANAGER_CONF_H__
#define __DATALOG_MANAGER_CONF_H__

#include "../Configuration.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Datalog.h"

#define UPDATING_LOG 0
#define ONE_TIME_LOG 1
#define DATALOG_MANAGER_MAX_LOGS  10
#define DATALOG_MANAGER_TIMER_NUM 4

class DatalogManager {
    public:
        DatalogManager(unsigned long timer_lengths[], uint8_t num_timers);

        void addLog(Datalog *, uint8_t timer);
        bool configureAutolog(uint8_t timer_group, unsigned long wait, unsigned long length, unsigned int vals);

        void update();
        void printLogs();
        void printTimerConfigs();

    private:
        uint8_t add_log_idx = 0;
        Datalog *logs[DATALOG_MANAGER_MAX_LOGS];
        uint8_t log_timer_map[DATALOG_MANAGER_MAX_LOGS]; // what timer is associated with the log
        uint8_t active_logs = 0;

        elapsedMillis log_timers[DATALOG_MANAGER_TIMER_NUM];
        unsigned long log_refresh_length[DATALOG_MANAGER_TIMER_NUM]; // how long each timer lasts for
};

DatalogManager::DatalogManager(unsigned long timer_lengths[], uint8_t num_timers) {
    for (int i = 0; i <  num_timers; i++) {
        log_refresh_length[i] = timer_lengths[i];
    }
}

void DatalogManager::printLogs() {
    printMajorDivide(" Printing All Datalogs ");
    for (int i = 0; i < active_logs; i++) {
        logs[i]->printLog(8);
    }
}

void DatalogManager::printTimerConfigs()  {
    printMinorDivide();
    Serial.println("Printing the Timer Configurations for the Datalog Manager");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print("current: ");Serial.print(log_timers[i]);Serial.print("\tmax\t");
        Serial.println(log_refresh_length[i]);
    }
    Serial.print("log_timer_map:\t");
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++) {
        Serial.print(log_timer_map[i]);Serial.print("\t");
    }
    Serial.println();
    printMinorDivide();
}

void DatalogManager::addLog(Datalog *log, uint8_t timer_num) {
    if (add_log_idx >= DATALOG_MANAGER_MAX_LOGS) {
        Serial.println("ERROR, Datalog Manager can only handle ");
        Serial.print(DATALOG_MANAGER_MAX_LOGS);
        Serial.println(" logs at a time, resetting index to 0");
        add_log_idx = 0;
    }
    logs[add_log_idx] = log;
    log_timer_map[add_log_idx] = timer_num;
    active_logs = max(add_log_idx, active_logs);
    add_log_idx++;
    Serial.print("Added log to the datamanager under timer ");
    Serial.println(timer_num);
}

void DatalogManager::update() {
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        // take the elapsedd millis value at start of loop to ensure that
        // all related logs either update or dont update at the same time
        unsigned long u_time = log_timers[i];
        // if it is time to update these logs then do so
        uint8_t updates = 0;
        for (int l = 0; l < active_logs; l++) {
         if (log_timer_map[l] == i && u_time > log_refresh_length[i]) {
            logs[l]->update();
            updates++;
            Serial.print("Updating the ");Serial.print(i);Serial.println(" timer logs");
         }
        }
        if (updates > 0) {
            log_timers[i] = 0;
        }
    }
}

/*
uint8_t updateLuxLog(LuxManager *lux_mng[]) {
  // if datalogging is active
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && lux_eeprom_idx < EEPROM_LUX_LOG_END) {
    dprint(PRINT_LOG_WRITE, "Logging Lux Data into EEPROM location: "); dprintln(PRINT_LOG_WRITE, (int)lux_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "lux_average                          :\t");
    // store the current lux readings
    // increment the index, 4 bytes to a double
    if (front_lux_active) {
      dprint(PRINT_LOG_WRITE, lux_mng[0]->getLux()); 
      writeDouble(lux_eeprom_idx, lux_mng[0]->getLux());
      lux_eeprom_idx += 4;
    } else{
      dprint(PRINT_LOG_WRITE, "Front sensor deactivated ");
    }
    if (rear_lux_active) {
      writeDouble(lux_eeprom_idx, lux_mng[1]->getLux());
      lux_eeprom_idx += 4;
      dprint(PRINT_LOG_WRITE, "\t");
      dprintln(PRINT_LOG_WRITE, lux_mng[1]->getLux());
    } else {
      dprint(PRINT_LOG_WRITE, "Front sensor deactivated ");
    }
    return 1;
  }
  return 0;
}

bool updateLuxMinMaxDatalog(LuxManager *lux_sensors[]) {
  // give the program some time to settle
  if (data_logging_active && millis() > LOG_START_DELAY) {
      for (int channel = 0; channel < 2; channel++) {
          // front
          // is the current reading more than the max recorded?
          // if the current reading is the same then nothing is written, if it is different something is writen
          writeDouble(EEPROM_MAX_LUX_READINGS + (channel * 4) , lux_sensors[channel]->getMaxLux());
          dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, lux_sensors[channel]->getName());
          dprint(PRINT_LOG_WRITE, " max_lux_reading to EEPROM at addr: "); dprint(PRINT_LOG_WRITE, EEPROM_MAX_LUX_READINGS + (channel * 4));
          dprint(PRINT_LOG_WRITE, " :\t"); dprintln(PRINT_LOG_WRITE, lux_sensors[channel]->getMaxLux());
          dprint(PRINT_LOG_WRITE, " read back:\t");dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + (channel * 4)));

          writeDouble(EEPROM_MIN_LUX_READINGS + (channel * 4) , lux_sensors[channel]->getMinLux());
          dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, lux_sensors[channel]->getName());
          dprint(PRINT_LOG_WRITE, " min_lux_reading to EEPROM\t"); dprint(PRINT_LOG_WRITE, lux_sensors[channel]->getMinLux());
          dprint(PRINT_LOG_WRITE, " read back:\t"); dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + (channel * 4)));
          return 1;
    }
  }
  return 0;
}

uint8_t updateBrightnessScalerAvgLog(NeoGroup neos[]) {
  // TODO
  // write the current brightness scaler average to EEPROM
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    dprint(PRINT_LOG_WRITE,"Logging the average brightness scalers  :");
    for (int  i = 0; i < 2; i++) { // replace 2 with something variable
      dprint(PRINT_LOG_WRITE,"\t");
      writeDouble(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4), neos[i].getAvgBrightnessScaler());
      dprint(PRINT_LOG_WRITE, neos[i].getAvgBrightnessScaler());
    }
    dprintln(PRINT_LOG_WRITE);
    return 1;
  }
  return 0;
}

void updateEEPROMLogs(NeoGroup neos[], LuxManager lux_mng[]) {
  if (data_logging_active) {
    // if enough time has passed since init for data-logging to be active
    uint8_t updates = 0;
    if (millis() > LOG_START_DELAY) {
      //  if one of the logs update then update the log timer
      updates += updateOnRateLog(neos);
      updates += updateBrightnessScalerAvgLog(neos);
      updates += updateLuxLog(&lux_mng);
      updates += updateCPMLog();
      updates += updateLuxMinMaxDatalog(&lux_mng);
    }
    if (updates) {
      log_timer = 0;
      Serial.print("A Total of "); Serial.print(updates); Serial.println(" logs were updated");
      printDivide();
    }
  }
}

void writeSetupConfigsToEEPROM() {
  if (data_logging_active) {
    #if JUMPERS_POPULATED
    EEPROM.update(EEPROM_JMP1, cicada_mode);
    EEPROM.update(EEPROM_JMP2, stereo_audio);
    EEPROM.update(EEPROM_JMP3, NUM_LUX_SENSORS);
    EEPROM.update(EEPROM_JMP4, combine_lux_readings);
    EEPROM.update(EEPROM_JMP5, gain_adjust_active);
    EEPROM.update(EEPROM_JMP6, data_logging_active);
    dprintln(PRINT_LOG_WRITE, "logged jumper values to EEPROM");
    #endif // jumpers_populated

    #if FIRMWARE_MODE == CICADA_MODE
    // log the starting gains to the min/max EEPROM
    writeDouble(EEPROM_CLICK_GAIN_MIN, click_gain[0]);
    writeDouble(EEPROM_CLICK_GAIN_MIN + 4, click_gain[1]);
    writeDouble(EEPROM_SONG_GAIN_MIN, song_gain[0]);
    writeDouble(EEPROM_SONG_GAIN_MIN + 4, song_gain[1]);

    writeDouble(EEPROM_CLICK_GAIN_START, click_gain[0]);
    writeDouble(EEPROM_CLICK_GAIN_START + 4, click_gain[1]);
    writeDouble(EEPROM_SONG_GAIN_START, song_gain[0]);
    writeDouble(EEPROM_SONG_GAIN_START + 4, song_gain[1]);

    writeDouble(EEPROM_CLICK_GAIN_MAX, click_gain[0]);
    writeDouble(EEPROM_CLICK_GAIN_MAX + 4, click_gain[1]);
    writeDouble(EEPROM_SONG_GAIN_MAX, song_gain[0]);
    writeDouble(EEPROM_SONG_GAIN_MAX + 4, song_gain[1]);

    writeDouble(EEPROM_SONG_GAIN_CURRENT, song_gain[0]);
    writeDouble(EEPROM_SONG_GAIN_CURRENT + 4, song_gain[1]);
    writeDouble(EEPROM_CLICK_GAIN_CURRENT, click_gain[0]);
    writeDouble(EEPROM_CLICK_GAIN_CURRENT + 4, click_gain[1]);

    writeLong(EEPROM_TOTAL_CLICKS, 0);
    writeLong(EEPROM_TOTAL_CLICKS + 4, 0);

    #endif // cicada_mode

    EEPROM.update(EEPROM_AUDIO_MEM_MAX, AUDIO_MEMORY);
    dprintln(PRINT_LOG_WRITE, "logged AUDIO_MEMORY to EEPROM");
    
    // auto-log values
    EEPROM.update(EEPROM_LOG_ACTIVE, data_logging_active);
    writeLong(EEPROM_LOG_POLLING_RATE, LOG_POLLING_RATE);
    writeLong(EEPROM_LOG_START_TIME, LOG_START_DELAY);
    writeLong(EEPROM_LOG_END_TIME,  LOG_START_DELAY + LOG_TIME_FRAME);
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
  // printAndClearDoubleLog(EEPROM_CPM_LOG_START, EEPROM_CPM_LOG_END, "Clicks Per Minute ");

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
  // printOnRatioLog();
  // printBrightnessAverageLog();

  printMajorDivide("Finished Printing EEPROM Datalog");
}

*/ 

#endif // datalog_manager_h
