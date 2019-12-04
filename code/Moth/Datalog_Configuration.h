// contains all the datalogging configuration goodies
#ifndef __DATALOG_CONFIGURATION_H__
#define __DATALOG_CONFIGURATION_H__
#define DOUBLE_PRECISION ((double)100000.0)

// EEPROM MEMORY ADDRESSES
#define EEPROM_JMP1                       0
#define EEPROM_JMP2                       1
#define EEPROM_JMP3                       2
#define EEPROM_JMP4                       3
#define EEPROM_JMP5                       4
#define EEPROM_JMP6                       5

#define EEPROM_S_VERSION_MAJOR            6
#define EEPROM_S_VERSION_MINOR            7
#define EEPROM_S_SUBVERION                8

#define EEPROM_H_VERSION_MAJOR            9
#define EEPROM_H_VERSION_MINOR            10

#define EEPROM_SERIAL_ID                  40
#define EEPROM_RUN_TIME                   41

#define EEPROM_AUDIO_MEM_USAGE            45
#define EEPROM_AUDIO_MEM_MAX              46

#define EEPROM_LOG_POLLING_RATE           50
#define EEPROM_LOG_START_TIME             54
#define EEPROM_LOG_END_TIME               58
#define EEPROM_LOG_ACTIVE                 63

// lux readings, each value takes up 4 spots
#define EEPROM_MIN_LUX_READINGS           100
#define EEPROM_MIN_LUX_READING_COMBINED   108
#define EEPROM_MAX_LUX_READINGS           112
#define EEPROM_MAX_LUX_READING_COMBINED   120

// audio
#define EEPROM_CLICK_GAIN_MIN             200
#define EEPROM_CLICK_GAIN_MAX             208

#define EEPROM_CLICK_GAIN_START           216

#define EEPROM_SONG_GAIN_MIN              224
#define EEPROM_SONG_GAIN_MAX              232
#define EEPROM_SONG_GAIN_START            240

#define EEPROM_TOTAL_CLICKS               248

// current values
#define EEPROM_SONG_GAIN_CURRENT          256
#define EEPROM_CLICK_GAIN_CURRENT         264


// clicks per second log (100 spots, double)
#define EEPROM_CPM_LOG_START              300
// #define EEPROM_CPM_INT                 400
#define EEPROM_CPM_LOG_LENGTH             40

// Lux Log
#define EEPROM_LUX_LOG_START              1000
#define EEPROM_LUX_LOG_LENGTH             40

// Led logs/info
// how often are the LEDs on instead of off?
// what is the average brightness scaler?
#define EEPROM_LED_ON_RATIO               1800
#define EEPROM_AVG_BRIGHTNESS_SCALER      1808

// cicada specific
unsigned int cpm_eeprom_idx = EEPROM_CPM_LOG_START;
const long EEPROM_CPM_LOG_END = EEPROM_CPM_LOG_START + (4 * 2 * EEPROM_CPM_LOG_LENGTH);//4 bits to double, front and rear, log length
const long EEPROM_LUX_LOG_END = EEPROM_LUX_LOG_START + (4 * 2 * EEPROM_LUX_LOG_LENGTH);//4 bits to double, front and rear, log length

// calculate the actual start and end times based on this
//
#define EEPROM_LOG_SIZE                 2000
// How much space will be allocated for the write once logs
#define EEPROM_WRITE_ONCE_LOG_SIZE      400
#define AUTO_LOG_SPACE                  (EEPROM_LOG_SIZE - WRITE_ONCE_LOG_SPACE)


#endif // __DATALOG_CONFIGURATION_H__
