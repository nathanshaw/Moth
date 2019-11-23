// contains all the datalogging configuration goodies

#define DOUBLE_PRECISION 1000000.0

// EEPROM MEMORY ADDRESSES
#define EEPROM_JMP1                       0
#define EEPROM_JMP2                       1
#define EEPROM_JMP3                       2
#define EEPROM_JMP4                       3
#define EEPROM_JMP5                       4
#define EEPROM_JMP6                       5

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
