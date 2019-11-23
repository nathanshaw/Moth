/*
  #ifndef __DATALOG_CONF_H__
  #define __DATALOG_CONF_H__
*/
#include <EEPROM.h>

unsigned int cpm_eeprom_idx = EEPROM_CPM_LOG_START;
const long EEPROM_CPM_LOG_END = EEPROM_CPM_LOG_START + (4 * 2 * EEPROM_CPM_LOG_LENGTH);//4 bits to double, front and rear, log length
const long EEPROM_LUX_LOG_END = EEPROM_LUX_LOG_START + (4 * 2 * EEPROM_LUX_LOG_LENGTH);//4 bits to double, front and rear, log length

elapsedMillis log_timer;
const long LOG_POLLING_RATE = (long)((double)LOG_TIME_FRAME / (double)EEPROM_LUX_LOG_LENGTH * 2.0);

unsigned int lux_eeprom_idx = EEPROM_LUX_LOG_START;
int lux_total[2];
int lux_readings[2];

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
  for (int side = 0; side < 5; side = side + 4) {
    if (side > 0) {
      dprint(PRINT_LOG_WRITE, "\nREAR\n");
    }
    for (int i = _start + side; i < _end; i += 8) {
      double val = readDoubleFromEEPROM(i);
      dprintln(PRINT_LOG_WRITE, "");
      dprint(PRINT_LOG_WRITE, i);
      dprint(PRINT_LOG_WRITE, ",");
      dprint(PRINT_LOG_WRITE, val);
      dprint(PRINT_LOG_WRITE, "\t");
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
    double lux_average[2];
    for (int i = 0; i < 2; i++) {
      lux_average[i] = lux_total[i] / (double)lux_readings[i];
      // reset the average values
      lux_total[i] = 0;
      lux_readings[i] = 0;
    }
    dprint(PRINT_LOG_WRITE, "Logging Lux Data into EEPROM location: "); dprintln(PRINT_LOG_WRITE, (int)lux_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "lux_average :\t"); dprint(PRINT_LOG_WRITE, lux_average[0]); dprint(PRINT_LOG_WRITE, "\t");
    dprintln(PRINT_LOG_WRITE, lux_average[1]);
    // store the current lux readings
    // increment the index, 4 bytes to a double
    writeDoubleToEEPROM(lux_eeprom_idx, lux_average[0]);
    lux_eeprom_idx += 4;
    writeDoubleToEEPROM(lux_eeprom_idx, lux_average[1]);
    lux_eeprom_idx += 4;
    return 1;
  }
  return 0;
}

void updateLuxMinMax(double lux[], double min_lux_reading[], double max_lux_reading[]) {
  // give the program some time to settle
  if (data_logging_active && millis() > 20000) {
    // front
    for (int i = 0; i < num_lux_sensors; i++) {
      // is the current reading more than the max recorded?
      if (lux[i] > max_lux_reading[i]) {
        max_lux_reading[i] = lux[i];
        writeDoubleToEEPROM(EEPROM_MAX_LUX_READINGS + (i * 4) , max_lux_reading[i]);
        dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, i);
        dprint(PRINT_LOG_WRITE, " max_lux_reading to EEPROM at addr: "); dprint(PRINT_LOG_WRITE, EEPROM_MAX_LUX_READINGS + (i * 4));
        dprint(PRINT_LOG_WRITE, " :\t"); dprintln(PRINT_LOG_WRITE, max_lux_reading[i]);
        // dprint(PRINT_LOG_WRITE, " read back:\t");dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + (i*4)));
      } // is the current reading less than the max recorded?
      if (lux[i] < min_lux_reading[i] && lux[i] != 0) {
        if (lux[i] > 0) {
          min_lux_reading[i] = lux[i];
          writeDoubleToEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4) , min_lux_reading[i]);
          dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, i);
          dprint(PRINT_LOG_WRITE, " min_lux_reading to EEPROM\t"); dprint(PRINT_LOG_WRITE, min_lux_reading[i]);
          dprint(PRINT_LOG_WRITE, " read back:\t"); dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4)));
        }
      }
    }
    // combined
    if (lux[2] > max_lux_reading[2]) {
      max_lux_reading[2] = lux[2];
      writeDoubleToEEPROM(EEPROM_MAX_LUX_READING_COMBINED, max_lux_reading[2]);
      dprint(PRINT_LOG_WRITE, "logged new combined max_lux_reading to EEPROM\t"); dprintln(PRINT_LOG_WRITE, max_lux_reading[2]);
    } else if (lux[2] < min_lux_reading[2]) {
      min_lux_reading[2] = lux[2];
      writeDoubleToEEPROM(EEPROM_MIN_LUX_READING_COMBINED , min_lux_reading[2]);
      dprint(PRINT_LOG_WRITE, "logged new combined min_lux_reading to EEPROM\t"); dprintln(PRINT_LOG_WRITE, max_lux_reading[2]);
    }
  }
}
// #endif // __DATALOG_CONF_H__
