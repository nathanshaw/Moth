// this is the primary use configurable portion of the program

#ifndef CONFIGURATION_H
#define CONFIGURATION_H 

#include "Configuration_adv.h"
#include "Macros.h"
#include <PrintUtils.h>
#include "Macros.h"

// for things which can be either on or off, the name is simply defined?

///////////////////////// General Settings /////////////////////////////////
#define SERIAL_ID 3

///////////////////////// Operating Modes //////////////////////////////////
#define CICADA_MODE 0
#define PITCH_MODE  1
#define FIRMWARE_MODE (PITCH_MODE)

#if FIRMWARE_MODE == PITCH_MODE
  #include "Configuration_pitch.h"
#endif

///////////////////////// Lux    Settings /////////////////////////////////
#define LUX_SENSORS_ACTIVE        false
#define LUX_CALIBRATION_TIME      4000
#define SMOOTH_LUX_READINGS       true

#define LOW_LUX_THRESHOLD         16.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD         100
#define HIGH_LUX_THRESHOLD        450.0

// Neo Pixels
#define MIN_BRIGHTNESS            10
#define MAX_BRIGHTNESS            255

// on scale of 0-1.0 what is the min multiplier for lux sensor brightness adjustment
#define BRIGHTNESS_SCALER_MIN     0.5
#define BRIGHTNESS_SCALER_MAX     1.5

unsigned long lux_max_reading_delay = long(1000.0 * 60.0 * 6); // every 6 minutes
unsigned long lux_min_reading_delay = long(1000.0 * 60.0 * 1); // one minute

bool front_lux_active = true;
bool rear_lux_active = true;

////////////////////////////////////////////////////////////////////////////
///////////////////////// Datalog Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// record the run time // last value is number of minutes
#define RUNTIME_POLL_DELAY 1000*60*5
// when should the value log start // last value is number of minutes
#define LOG_START_DELAY 1000*60*1
// how long should the logging last? // last value is number of hours
#define LOG_TIME_FRAME 1000*60*60*0.5

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
///////////////////////// Cicada ////////////////////////////////////////
#define PRINT_LUX_DEBUG       true
#define PRINT_LUX_READINGS    true
#define PRINT_BRIGHTNESS_SCALER_DEBUG false
// TODO
#define PRINT_SONG_DATA       true
// TODO
#define PRINT_CLICK_FEATURES  false
#define PRINT_CLICK_DEBUG     false
// TODO
#define PRINT_LED_VALUES      false
#define PRINT_AUTO_GAIN       false

#define PRINT_LED_DEBUG       false

#define PRINT_LOG_WRITE       true

#define EEPROM_WRITE_CHECK    false

///////////////////////// Feature Collector ///////////////////////////////
// feature collector related
#define PRINT_RMS_VALS        false
#define PRINT_PEAK_VALS       false
#define PRINT_TONE_VALS       false
#define PRINT_FREQ_VALS       false
#define PRINT_FFT_VALS        false

// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS false

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////


// turns on/off debug printing
#define DEBUG 0

// data logging related
#define AUDIO_USAGE_POLL_RATE 200000

// Audio
#define USB_OUTPUT 1
#define MAX_GAIN_ADJUSTMENT 0.10

const uint32_t auto_gain_frequency = 1000 * 60 * 10; // how often to calculate auto-gain (in ms)

// song gain
#define RMS_DELTA  0
#define PEAK_DELTA 1
#define ALL_FEATURES 10
//////// Song Settings

// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
#define SONG_FEATURE PEAK_DELTA

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.005
#define MAX_SONG_PEAK_AVG 0.20

#define STARTING_SONG_GAIN 8.0

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

#define CLICK_FEATURE RMS_DELTA
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

//////////////////// Leds

#endif // CONFIGURATION_H
