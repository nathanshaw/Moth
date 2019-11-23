// this is the primary use configurable portion of the program

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
// for things which can be either on or off, the name is simply defined?

///////////////////////// General Settings /////////////////////////////////
#define SERIAL_ID 7

#define CICADA_MODE 0
#define PITCH_MODE  1
#define FIRMWARE_MODE (CICADA_MODE)

///////////////////////// Datalog Settings /////////////////////////////////
// record the run time // last value is number of minutes
#define RUNTIME_POLL_DELAY 1000*60*5
// when should the value log start // last value is number of minutes
#define LOG_START_DELAY 1000*60*30
// how long should the logging last? // last value is number of hours
#define LOG_TIME_FRAME 1000*60*60*50

///////////////////////// Jumper Settings /////////////////////////////////
// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED 0

///////////////////////// Auto-Gain Settings /////////////////////////////////
// turn on/off auto gain. 0 is off, 1 is on
// #define AUTO_GAIN 1

// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define MIN_LED_ON_RATIO (0.3)
#define MAX_LED_ON_RATIO (0.95)

///////////////////////// Debuggings ////////////////////////////////////
#define PRINT_LUX_DEBUG       false
#define PRINT_LUX_READINGS    true
// TODO
#define PRINT_SONG_DATA       false
// TODO
#define PRINT_CLICK_FEATURES  false
#define PRINT_CLICK_DEBUG     true
// TODO
#define PRINT_LED_VALUES      false
#define PRINT_AUTO_GAIN       false

#define PRINT_LED_DEBUG       false

#define PRINT_LOG_WRITE       true

#define EEPROM_WRITE_CHECK    false

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS true

// turns on/off debug printing
#define DEBUG 0

// data logging related
#define AUDIO_USAGE_POLL_RATE 200000

// Neo Pixels
#define MAX_BRIGHTNESS 250
#define MIN_BRIGHTNESS 20

// Audio
#define USB_OUTPUT 1
#define MAX_GAIN_ADJUSTMENT 0.10

const uint32_t auto_gain_frequency = 1000 * 60 * 0.5; // how often to calculate auto-gain (in ms)

////////////////////////////////////// lux

unsigned long lux_max_reading_delay = long(1000 * 60 * 6); // every 6 minutes
unsigned long lux_min_reading_delay = long(1000 * 60 * 0.05); // 3 seconds

#define SMOOTH_LUX_READINGS true

#define MIN_LUX_EXPECTED 1.0
#define MAX_LUX_EXPECTED 400.0
// on scale of 1-1000 what is the min multiplier for lux sensor brightness adjustment
// 500 would equate to a dimming of 50%
#define BRIGHTNESS_SCALER_MIN 800

// song gain
//////// Song Settings

// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
#define SONG_FEATURE "peak"

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.005
#define MAX_SONG_PEAK_AVG 0.20

#define STARTING_SONG_GAIN 12.0

#define SONG_BQ1_THRESH 13500
#define SONG_BQ1_Q 0.85
#define SONG_BQ1_DB -12
#define SONG_BQ2_THRESH 14000
#define SONG_BQ2_Q 0.85
#define SONG_BQ2_DB -12

///////// Click Settings
// what feature will be used to determine if a click has been found
// "rms_delta" will use that feature along with CLICK_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with CLICK_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
#define CLICK_FEATURE "rms_delta"
#define CLICK_RMS_DELTA_THRESH 0.03
#define CLICK_PEAK_DELTA_THRESH 0.03

// One click per ten minutes
#define MIN_CLICKS_PER_MINUTE 0.1
#define MAX_CLICKS_PER_MINUTE 40.0

#define STARTING_CLICK_GAIN 6.0

#define MIN_CLICK_GAIN 0.5
#define MAX_CLICK_GAIN 24.0

#define MIN_SONG_GAIN 0.5
#define MAX_SONG_GAIN 20

#define CLICK_BQ1_THRESH 1200
#define CLICK_BQ1_Q 0.95
#define CLICK_BQ1_DB -24
#define CLICK_BQ2_THRESH 2500
#define CLICK_BQ2_Q 0.95
#define CLICK_BQ2_DB -24

// how high the click flash timer will go up to
#define MAX_FLASH_TIME 300
// where the click flash timer will start
#define MIN_FLASH_TIME 100

//////////////////// Leds


#endif // CONFIGURATION_H
