#ifndef __DLMANAGER_H__
#define __DLMANAGER_H__

#include "../Configuration.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Datalog.h"

////////////////////////// Struct for One-Off logs ///////////////////////

struct oneofflog {
    String readMsg;
    uint32_t addr;
    uint8_t type;
    uint8_t len;
    bool written = false;
};

typedef struct oneofflog OneOffLog;

//////////////////////// Writing Methods /////////////////////////////////
// to do this is aweful, need to rewrite todo
// store the dat with least significant bytes in lower index
void writeDoubleToEEPROM(uint32_t addr, double data) {
    uint8_t b[4];
    uint32_t d = data * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
      b[i] = (d >> i * 8) & 0x00FF;
      EEPROM.update(addr + i, b[i]);
    }
}

void writeShortToEEPROM(uint32_t addr, uint16_t data) {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = data & 0x00FF;
    uint8_t msb = data >> 8;
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
}

void writeLongToEEPROM(uint32_t addr, uint32_t data) {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
      b[i] = data >> 8 * i;
      EEPROM.update(addr + i, b[i]);
    }
}

//////////////////////// Reading Methods /////////////////////////////////
double readDoubleFromEEPROM(int a) {
  uint32_t data = EEPROM.read(a + 3);
  for (int i = 2; i > -1; i--) {
    uint8_t reading = EEPROM.read(a + i);
    // dprint(PRINT_LOG_WRITE, reading);
    // dprint(PRINT_LOG_WRITE, "|");
    data = (data << 8) | reading;
  }
  return (double)data / DOUBLE_PRECISION;
}

uint16_t readShortFromEEPROM(int a) {
  int data = EEPROM.read(a + 1);
  data = (data << 8) + EEPROM.read(a);
  return data;
}

uint32_t readLongFromEEPROM(int a) {
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t n = EEPROM.read(a + i) << 8 * i;
    data = n | data;
  }
  return data;
}

class DLManager {
    public:
        // DLManager();
        DLManager();
        DLManager(String _id);
        ~DLManager();
        // DLManager(Datalog &l[], uint8_t);
        void addLog(Datalog *, uint8_t);
        void addLog(Datalog, uint8_t);

        void logSetupConfigByte(String str, uint8_t data);
        void logSetupConfigShort(String str, uint16_t data);
        void logSetupConfigLong(String str, uint32_t data);
        void logSetupConfigDouble(String str, double data);

        void addAutolog(String, uint8_t, double *);

        bool configureAutolog();
        void configureTimer(uint8_t num, uint32_t start_delay, uint32_t log_time, uint32_t logs_requested);
        void update();

        void printAutologs();
        void printOneOffLogs();
        void printOneOffLog(OneOffLog log);
        void printAllLogs();

        void printTimerConfigs();
        void clearLogs();

    private:
        String id = "";
        // for keeping track of the datalogging shizzz
        uint32_t total_log_size = EEPROM_LOG_SIZE;

        ///////////////////// One-off Logs /////////////////
        // These are logs which are only intended to be written to once
        // It includes things such as setup configurations, software and hardware versions
        // and similar things, it is not for logs in which a single value is updated over the
        // runtime of the program, only for setup configurations.

        // how much memory there is to store the single value logs
        uint32_t one_off_start_idx = 0;
        uint32_t one_off_log_size = EEPROM_WRITE_ONCE_LOG_SIZE;
        // how much unallocated single value space is left?
        uint32_t remaining_one_off_size = one_off_log_size;
        // what is the next unallocated space?
        uint32_t next_one_off_start_idx = 0;

        // the structs which track the one off log data
        OneOffLog one_off_logs[EEPROM_WRITE_ONCE_LOG_SIZE/2];
        // what is the next open index for the once-off log
        uint16_t one_off_write_idx = 0;

        // the number of one_off_logs which have actually been stored
        uint16_t active_one_off_logs = 0;

        //////////// autolog ////////////////////
        uint32_t autolog_start_idx = one_off_log_size;
        uint32_t autolog_write_idx = autolog_start_idx;
        uint32_t remaining_autolog_space = total_log_size - one_off_log_size;

        // Start and stop indexes for each log
        Datalog logs[DATALOG_MANAGER_MAX_LOGS];
        bool first_reading[DATALOG_MANAGER_MAX_LOGS];
        uint8_t log_timer_map[DATALOG_MANAGER_MAX_LOGS]; // what timer is associated with the log
        uint8_t active_logs = 0;
        // for keeping track of which is the next index for a log to add
        uint8_t add_log_idx = 0;

        // for handling pointer logs
        Datalog *logs_p[DATALOG_MANAGER_MAX_LOGS];
        bool first_reading_p[DATALOG_MANAGER_MAX_LOGS];
        uint8_t log_timer_map_p[DATALOG_MANAGER_MAX_LOGS]; // what timer is associated with the log
        uint8_t active_logs_p = 0;
        // for keeping track of which is the next index for a log to add
        uint8_t add_log_idx_p = 0;

        // for keeping track of the timers / triggers
        elapsedMillis log_timers[DATALOG_MANAGER_TIMER_NUM];
        uint32_t log_refresh_length[DATALOG_MANAGER_TIMER_NUM]; // how long each timer lasts for
        uint32_t start_delays[DATALOG_MANAGER_TIMER_NUM];
        uint32_t remaining_logs[DATALOG_MANAGER_TIMER_NUM];
};

///////////////////////// constructors ///////////////////////////////////////////
DLManager::DLManager(){};
DLManager::~DLManager(){};
/*
DatalogManager::DatalogManager(Datalog &l[], uint8_t num_logs) {
    id = _id;
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++ ) {
        first_reading[i] = true;
    }
    for (int i = 0; i < num_logs; i++ ) {
        logs[i] = &l[i];
    }
}
*/

DLManager::DLManager(String _id) {
    id = _id;
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++ ) {
        first_reading[i] = true;
    }
}

/////////////////////// One-off logs ////////////////////////////////////////

void DLManager::logSetupConfigByte(String str, uint8_t data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 1;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type
        Serial.print(str);Serial.println(EEPROM.read(one_off_write_idx));
        EEPROM.update(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_BYTE, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        dprint(PRINT_LOG_WRITE, one_off_write_idx);
        dprint(PRINT_LOG_WRITE, "\t");
        dprint(PRINT_LOG_WRITE, " Logging new byte to EEPROM : ");
        dprint(PRINT_LOG_WRITE, str);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

void DLManager::logSetupConfigShort(String str, uint16_t data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 2;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type
        Serial.print(str);Serial.println(readShortFromEEPROM(one_off_write_idx));
        writeShortToEEPROM(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_BYTE, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        dprint(PRINT_LOG_WRITE, one_off_write_idx);
        dprint(PRINT_LOG_WRITE, "\t");
        dprint(PRINT_LOG_WRITE, " Logging new short to EEPROM : ");
        dprint(PRINT_LOG_WRITE, str);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

void DLManager::logSetupConfigLong(String str, uint32_t data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 4;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type.
        Serial.print(str);Serial.println(readLongFromEEPROM(one_off_write_idx));
        writeLongToEEPROM(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_BYTE, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        dprint(PRINT_LOG_WRITE, one_off_write_idx);
        dprint(PRINT_LOG_WRITE, "\t");
        dprint(PRINT_LOG_WRITE, " Logging new long to EEPROM : ");
        dprint(PRINT_LOG_WRITE, str);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}

void DLManager::logSetupConfigDouble(String str, double data) {
    // if there is enough room in the log to track another value
    uint8_t dlen = 4;
    if (one_off_write_idx + dlen < one_off_start_idx + one_off_log_size) {
        // write the data to eeprom according to type
        Serial.print(str);Serial.println(readDoubleFromEEPROM(one_off_write_idx));
        writeDoubleToEEPROM(one_off_write_idx, data);
        // update a new struct with the log data
        one_off_logs[active_one_off_logs] = {str, one_off_write_idx, DATATYPE_BYTE, dlen, true};
        // incrment the one off timer write idx
        one_off_write_idx += dlen;
        // update the stored one off log active structs idx
        active_one_off_logs++;
        // dprint
        dprint(PRINT_LOG_WRITE, one_off_write_idx);
        dprint(PRINT_LOG_WRITE, "\t");
        dprint(PRINT_LOG_WRITE, " Logging new double to EEPROM : ");
        dprint(PRINT_LOG_WRITE, str);
        dprint(PRINT_LOG_WRITE, "\t");
        dprintln(PRINT_LOG_WRITE, data);
    } else {
        Serial.println("ERROR - Sorry can't create any more one-off logs, there is not enough allocated space on the EEPROM for one-off logging");
    }
}



/////////////////////////////// Adding External Datalog Objects ////////////////////////////////

void DLManager::addLog(Datalog log, uint8_t timer_num) {
    if (add_log_idx >= DATALOG_MANAGER_MAX_LOGS) {
        Serial.println("ERROR, Datalog Manager can only handle ");
        Serial.print(DATALOG_MANAGER_MAX_LOGS);
        Serial.println(" logs at a time, resetting index to 0");
        add_log_idx = 0;
    }
    logs[add_log_idx] = log;
    log_timer_map[add_log_idx] = timer_num;
    active_logs = max(add_log_idx, active_logs++);
    add_log_idx++;
    Serial.print("Added log to the datamanager under timer ");
    Serial.print(timer_num);Serial.print(" active_logs now: ");Serial.println(active_logs);
}

void DLManager::addLog(Datalog *log, uint8_t timer_num) {
    if (add_log_idx_p >= DATALOG_MANAGER_MAX_LOGS) {
        Serial.println("ERROR, Datalog Manager can only handle ");
        Serial.print(DATALOG_MANAGER_MAX_LOGS);
        Serial.println(" logs at a time, resetting index to 0");
        add_log_idx_p = 0;
    }
    logs_p[add_log_idx_p] = log;
    log_timer_map_p[add_log_idx_p] = timer_num;
    active_logs_p = max(add_log_idx_p, active_logs_p++);
    add_log_idx_p++;
    Serial.print("Added log to the datamanager under timer ");
    Serial.println(timer_num);
}

/////////////////////////////// Auto Logging ////////////////////////////////
void DLManager::addAutolog(String _id,  uint8_t _timer, double *_val) {
    uint32_t log_size = remaining_logs[_timer] * 4;
    if (log_size < remaining_autolog_space) {
        addLog(Datalog(_id, autolog_write_idx, _val, remaining_logs[_timer], true), _timer);
        autolog_write_idx += log_size;
        remaining_autolog_space -= log_size;
        printMinorDivide();
        Serial.print("Adding a new AutoLog with size:\t");dprintln(PRINT_LOG_WRITE, log_size);
        Serial.print("remaining autolog space       :\t");dprintln(PRINT_LOG_WRITE, remaining_autolog_space);
        Serial.print("starting EEPROM idx           :\t");dprintln(PRINT_LOG_WRITE, autolog_write_idx - log_size);
        printMinorDivide();
    } else {
        Serial.println("ERROR - sorry the autolog is not initiated due to a lack of remaining EEPROM space.");
    }
}
/////////////////////////////// Update Functions ////////////////////////////////
void DLManager::update() {
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        // if it is the first reading then only update if it has been longer than the start delay
        // take the elapsedd millis value at start of loop to ensure that
        // all related logs either update or dont update at the same time
        unsigned long u_time = log_timers[i];
        if ((first_reading[i] == true && u_time > start_delays[i]) || (first_reading[i] == false )) {
            first_reading[i] = false;
            // if it is time to update these logs then do so
            uint8_t updates = 0;
            for (int l = 0; l <= active_logs; l++) {
                 if (log_timer_map[l] == i && u_time > log_refresh_length[i]) {
                    logs[l].update();
                    updates++;
                    // Serial.print("Updating the ");Serial.print(i);Serial.println(" timer logs");
                 }
                 else {
                     // Serial.print(u_time);Serial.print("\t");Serial.println(log_refresh_length[i]);
                 }
            }
            for (int l = 0; l <= active_logs_p; l++) {
                 if (log_timer_map_p[l] == i && u_time > log_refresh_length[i]) {
                    logs_p[l]->update();
                    updates++;
                    // Serial.print("Updating the ");Serial.print(i);Serial.println(" timer pointer logs");
                 }
                 else {
                     // Serial.print(u_time);Serial.print("\t");Serial.println(log_refresh_length[i]);
                 }
            }
            if (updates > 0) {
                log_timers[i] = 0;
            }
        }
    }
}
//
/////////////////////////////////// Utility Functions /////////////////////////////////////////
void DLManager::configureTimer(uint8_t num, uint32_t start_delay, uint32_t log_time, uint32_t logs_requested) {
    log_refresh_length[num] = log_time / logs_requested;
    start_delays[num] = start_delay;
    remaining_logs[num] = logs_requested;
}

void DLManager::clearLogs() {
    for (int i = 0; i <= active_logs; i++) {
        logs[i].clear();
    }
    for (int i = 0; i <= active_logs_p; i++) {
        logs_p[i]->clear();
    }
}


////////////////////////////////////// Printing Functions //////////////////////////////
void DLManager::printOneOffLog(OneOffLog log) {
    Serial.print(log.addr);printTab();
    Serial.print(log.readMsg);printTab();
    if (log.type == DATATYPE_BYTE) {
        Serial.print(EEPROM.read(log.addr));
    } else if (log.type == DATATYPE_SHORT) {
        Serial.print(readShortFromEEPROM(log.addr));
    } else if (log.type == DATATYPE_LONG) {
        Serial.print(readLongFromEEPROM(log.addr));
    } else if (log.type == DATATYPE_DOUBLE) {
        Serial.print(readDoubleFromEEPROM(log.addr));
    }
    printTab();
    Serial.print("written = ");
    Serial.println(log.written);
}

void DLManager::printOneOffLogs() {
    Serial.println("Printing one-off logs");
    for (int i = 0; i < active_one_off_logs; i++) {
        printOneOffLog(one_off_logs[i]);
    }
    Serial.println();
}

void DLManager::printAutologs() {
    Serial.println(" Printing Autologs");
    for (int i = 0; i <= active_logs; i++) {
        Serial.print("printing log                    :\t");Serial.println(i);
        logs[i].printLog(8);
    }
    for (int i = 0; i <= active_logs_p; i++) {
        Serial.print("printing pointer log            :\t");Serial.println(i);
        logs_p[i]->printLog(8);
    }
    Serial.println();
}

void DLManager::printAllLogs() {
    printMajorDivide("All Logs Stored in EEPROM");
    printOneOffLogs();
    printAutologs();
    printMajorDivide("Finished Printing EEPROM Contents");
}

void DLManager::printTimerConfigs()  {
    printMinorDivide();
    Serial.println("Printing the Timer Configurations for the Datalog Manager");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print("current: ");Serial.print(log_timers[i]);Serial.print("\tmax\t");
        Serial.println(log_refresh_length[i]);
    }

    Serial.println("start_delays :\t");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(i);Serial.print("=");Serial.print(start_delays[i]);printTab();
    }
    Serial.println();

    Serial.println("remaining_logs:\t");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(i);Serial.print("=");Serial.print(remaining_logs[i]);printTab();

    }
    Serial.println();

    Serial.println("log_refresh_length:\t");
    for (int i = 0; i < DATALOG_MANAGER_TIMER_NUM; i++) {
        Serial.print(i);Serial.print("=");Serial.print(log_refresh_length[i]);printTab();

    }
    Serial.println();

    Serial.print("log_timer_map:\t");
    for (int i = 0; i < DATALOG_MANAGER_MAX_LOGS; i++) {
        Serial.print(log_timer_map[i]);Serial.print("\t");
    }
    Serial.println();
    printMinorDivide();
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
