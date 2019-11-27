#ifndef __DATALOG_CONF_H__
#define __DATALOG_CONF_H__

#include "Configuration.h"
#include <EEPROM.h>

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


#endif // datalog
