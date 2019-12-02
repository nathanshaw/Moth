#ifndef __DATALOG_CONF_H__
#define __DATALOG_CONF_H__

#define DATATYPE_DOUBLE        0
#define DATATYPE_SHORT         1
#define DATATYPE_LONG          2
#define DATATYPE_BYTE          3

//////////////////////////// Operating Modes for the Datalog /////////////////////
// will be written once at the setup loop then will never write again
#define DATALOG_TYPE_INIT       0
// will write to the same addr over and over again when commanded to do so
#define DATALOG_TYPE_UPDATE     1
// log consists of several memory locations for its values and will increment its index
// with each read until the space runs out then it will stop logging
// Note that the timing of the updates are determined by the datalogmanager class
#define DATALOG_TYPE_AUTO       2

class Datalog {
    public:
        Datalog(String _id, unsigned long address, double *val, int length, bool move, uint8_t _type);
        Datalog(String _id, unsigned long address, uint8_t *val, int length, bool move, uint8_t _type);
        Datalog(String _id, unsigned long address, uint16_t *val, int length, bool move, uint8_t _type);
        Datalog(String _id, unsigned long address, uint32_t *val, int length, bool move, uint8_t _type);

        bool update(); // could this be a while loop at some point?

        bool write(double data);
        bool write(uint8_t data);
        bool write(uint16_t data);
        bool write(uint32_t data);

        bool writeCheck(double);
        bool writeCheck(uint8_t);
        bool writeCheck(uint16_t);
        bool writeCheck(uint32_t);

        void clearLog();
        void printLog(uint8_t lines);

    private:
        // to reduce code on the overloaded init functions
        void _setup(String, unsigned long, int, bool, uint8_t);
        // for keeping track of index moving for autolog
        bool moving_index = false;
        // this is the user assigned name to the datalog
        String id;
        // for keeping track of what type of a log it is, there is UPDATING_LOG and ONE_TIME_LOG
        // the updating one will use a timer to update dependent on the amount of time which has passed since the last update
        bool type;
        uint8_t data_type;
        uint8_t value_size;

        // todo ...
        bool autolog_active = false;

        // pointers to the variables we want to track
        double   *dval;
        uint8_t  *bval;
        uint16_t *sval;
        uint32_t *lval;

        // for keeping track if the log is active
        bool active = false;

        // for keeping track of where to write
        unsigned long addr;
        unsigned int start_addr; // the starting addr
        unsigned int end_addr;   // if multiple values are stored by this log, then this is the end addr.
        unsigned long log_length;

        // writing to EEPROM
        void writeDouble(double data);
        void writeDouble();
        void writeShort(uint16_t data);
        void writeShort();
        void writeLong(uint32_t data);
        void writeLong();

        // reading from EEPROM
        double    readDouble(int);
        double    readDouble();
        uint16_t  readShort(int);
        uint16_t  readShort();
        uint32_t  readLong(int);
        uint32_t  readLong();
};


//////////////////////// High Level Methods /////////////////////////////////
Datalog::Datalog(String _id, unsigned long address, double *val, int length, bool move, uint8_t _type) {
    _setup(_id, address, length, move, _type);
    data_type = DATATYPE_DOUBLE;
    value_size = 4;
    dval = val;
}

Datalog::Datalog(String _id, unsigned long address, uint8_t *val, int length, bool move, uint8_t _type) {
    _setup(_id, address, length, move, _type);
    data_type = DATATYPE_BYTE;
    value_size = 1;
    bval = val;
}

Datalog::Datalog(String _id, unsigned long address, uint16_t *val, int length, bool move, uint8_t _type) {
    _setup(_id, address, length, move, _type);
    data_type = DATATYPE_SHORT;
    value_size = 2;
    sval = val;
}

Datalog::Datalog(String _id, unsigned long address, uint32_t *val, int length, bool move, uint8_t _type) {
    _setup(_id, address, length, move, _type);
    data_type = DATATYPE_LONG;
    value_size = 4;
    lval = val;
}

// to save code repitition
void Datalog::_setup(String _id, unsigned long address, int length, bool move, uint8_t _type) {
    start_addr = address;
    addr = address;
    id = _id;
    log_length = length;
    end_addr = address + (value_size * log_length);
    moving_index = move;
    type = _type;
}

//////////////////////// Writing Methods /////////////////////////////////
void Datalog::writeDouble(double data) {
    // to do this is aweful, need to rewrite todo
    // store the dat with least significant bytes in lower index
    uint8_t b[4];
    uint32_t d = data * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
      b[i] = (d >> i * 8) & 0x00FF;
      EEPROM.update(addr + i, b[i]);
    }
}

void Datalog::writeDouble() {
    // to do this is aweful, need to rewrite todo
    // store the dat with least significant bytes in lower index
    uint8_t b[4];
    uint32_t d = *dval * DOUBLE_PRECISION;
    for (int i = 0; i < 4; i++) {
      b[i] = (d >> i * 8) & 0x00FF;
      EEPROM.update(addr + i, b[i]);
    }
}

void Datalog::writeShort(uint16_t data) {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = data & 0x00FF;
    uint8_t msb = data >> 8;
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
}

void Datalog::writeShort() {
    // store the dat with least significant bytes in lower index
    uint8_t lsb = *sval & 0x00FF;
    uint8_t msb = *sval >> 8;
    EEPROM.update(addr, lsb);
    EEPROM.update(addr + 1, msb);
}

void Datalog::writeLong(uint32_t data) {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
      b[i] = data >> 8 * i;
      EEPROM.update(addr + i, b[i]);
    }
}

void Datalog::writeLong() {
    uint8_t b[4];
    for (int i = 0; i < 4; i++) {
      b[i] = *dval> 8 * i;
      EEPROM.update(addr + i, b[i]);
    }
}

//////////////////////// Reading Methods /////////////////////////////////
double Datalog::readDouble(int a) {
  uint32_t data = EEPROM.read(a + 3);
  for (int i = 2; i > -1; i--) {
    uint8_t reading = EEPROM.read(a + i);
    // Serial.print(reading);
    // Serial.print("|");
    data = (data << 8) | reading;
  }
  return (double)data / DOUBLE_PRECISION;
}

double Datalog::readDouble() {
  uint32_t data = EEPROM.read(addr + 3);
  for (int i = 2; i > -1; i--) {
    uint8_t reading = EEPROM.read(addr + i);
    // Serial.print(reading);
    // Serial.print("|");
    data = (data << 8) | reading;
  }
  return (double)data / DOUBLE_PRECISION;
}

uint16_t Datalog::readShort(int a) {
  int data = EEPROM.read(a + 1);
  data = (data << 8) + EEPROM.read(a);
  return data;
}

uint16_t Datalog::readShort() {
  int data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return data;
}

uint32_t Datalog::readLong(int a) {
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t n = EEPROM.read(a + i) << 8 * i;
    data = n | data;
  }
  return data;
}

uint32_t Datalog::readLong() {
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t n = EEPROM.read(addr + i) << 8 * i;
    data = n | data;
  }
  return data;
}

//////////////////////// Data Checking Methods /////////////////////////
bool Datalog::writeCheck(double data) {
  if (EEPROM_WRITE_CHECK) {
    double temp = readDouble(addr);
    Serial.print("data check:\t");
    Serial.print(data);
    Serial.print("\t");
    Serial.println(temp);
    if (data != temp) {
        return false;
    }
    else {return true;};
  }
  return false;
}

bool Datalog::writeCheck(uint8_t data) {
  if (EEPROM_WRITE_CHECK) {
    double temp = EEPROM.read(addr);
    Serial.print("data check:\t");
    Serial.print(data);
    Serial.print("\t");
    Serial.println(temp);
    if (data != temp) {
        return false;
    }
    else {return true;};
  }
  return false;
}

bool Datalog::writeCheck(uint16_t data) {
  if (EEPROM_WRITE_CHECK) {
    double temp = readShort(addr);
    Serial.print("data check:\t");
    Serial.print(data);
    Serial.print("\t");
    Serial.println(temp);
    if (data != temp) {
        return false;
    }
    else {return true;};
  }
  return false;
}

bool Datalog::writeCheck(uint32_t data) {
  if (EEPROM_WRITE_CHECK) {
    double temp = readLong(addr);
    Serial.print("data check:\t");
    Serial.print(data);
    Serial.print("\t");
    Serial.println(temp);
    if (data != temp) {
        return false;
    }
    else {return true;};
  }
  return false;
}

//////////////////////// Misc. Methods /////////////////////////////////

bool Datalog::update() {
    // if the log is currently active
    if (!active) {
        return 0;
    }
    // if we got this far then everything is good for an update
    dprint(PRINT_LOG_WRITE, "updated the ");dprint(PRINT_LOG_WRITE, " log : ");
    switch(data_type) {
        case DATATYPE_SHORT:
            writeShort();
            dprintln(PRINT_LOG_WRITE, (String)*sval);
            break;
        case DATATYPE_DOUBLE:
            writeDouble();
            dprintln(PRINT_LOG_WRITE, (String)*dval);
            break;
        case DATATYPE_BYTE:
            EEPROM.update(addr, *bval);
            dprintln(PRINT_LOG_WRITE, (String)*bval);
            break;
        case DATATYPE_LONG:
            writeLong();
            dprintln(PRINT_LOG_WRITE, (String)*lval);
            break;
    }
    // print some feedback if the appropiate flag is set
    /////////// autolog stuff
    if (autolog_active == true) {
        // increment the addr counter
        addr += value_size;
        // if the current address plus the data_length is greater than the end index and 
        // if the datalogger is on autolog then stop the autologger
        if (addr + value_size > end_addr) {
            autolog_active = false;
            active = false;
        }
    }
    return 1;
}

void Datalog::clearLog() {
    for (unsigned int i = start_addr; i < end_addr; i++) {
        EEPROM.update(i, 0);
    }
    dprint(PRINT_LOG_WRITE, "Cleared the "); dprint(PRINT_LOG_WRITE, id);
    dprintln(PRINT_LOG_WRITE, " Datalog");
}

void Datalog::printLog(uint8_t lines) {
  printDivide();
  Serial.print("Printing the ");
  Serial.print(id);
  Serial.print(" Datalog:");
  uint8_t per_line = (end_addr - start_addr) / lines;
  uint8_t itters = 0;
  double d = 0.0;
  uint32_t l = 0;
  uint16_t iv = 0;
  uint8_t b = 0;

  for (uint16_t i = start_addr; i < end_addr; i += value_size) {
        itters++;
        switch(data_type){
            case DATATYPE_LONG:
                l = readLong(i);
                Serial.print(l);
                break;
            case DATATYPE_BYTE:
                b = EEPROM.read(i);
                Serial.print(b);
                break;
            case DATATYPE_SHORT:
                iv = readShort(i);
                Serial.print(iv);
                break;
            case DATATYPE_DOUBLE:
                d = readDouble(i);
                Serial.print(d);
                break;
        }
        // print tab or line depending on how many prints have occured so far
        if (itters % per_line == 0) {
            Serial.println();
        } else {
            Serial.print("\t");
        }
  }
  printDivide();
}

#endif // __DATALOG_CONF_H__
