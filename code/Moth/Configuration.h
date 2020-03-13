#ifndef CONFIGURATION_H
#define CONFIGURATION_H 


/* This is the main configuration file for the Moth Framework
 * Using this file, along with the other configuration files you
 * Can cistomise how the firmware performs.
 * 
 * ----------------------- JUMPER PINS --------------------------
 * 
 */
#include "Configuration_hardware.h"
#include <Audio.h>
#include "Macros.h"
#include <PrintUtils.h>

////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Brightness Scalers////////////////////////
////////////////////////////////////////////////////////////////////////////
// the local brightness scaler will adjust the brightness that would normally be displayed
// to utalise the entire dynamic range available
#define LBS_ACTIVE                     true
// how often should the LBS be recalculated?
#define LBS_TIME_FRAME                 (1000 * 60 * 3)
// once the local min and max have been overwritten how long to collect readings for
// a new min and max before using the new values?
#define LBS_OVERLAP_TIME               (1000 * 30)

elapsedMillis lbs_timer;
uint8_t lbs_min =                     255;
uint8_t lbs_max =                     0;
// to keep track of 
double lbs_min_temp =                 999999999.9; 
double lbs_max_temp =                 0.0;
// this is what the LBS will map the lowest feature results and highest feature results 
// TODO will perhaps need to make a 16bit version of this?, or change all my brightnesses to be stored using 16 bits instead of 8?
uint8_t lbs_brightness_low =          0;
uint8_t lbs_brightness_high =         255;

// what percent from the low and high will be truncated to the lowest and highest value
#define LBS_LOW_TRUNCATE_THRESH       0.2
#define LBS_HIGH_TRUNCATE_THRESH      0.8
uint8_t lbs_scaler_min_thresh =       255;
uint8_t lbs_scaler_max_thresh =       0;

////////////////////////////////////////////////////////////////////////////
///////////////////////// General Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define SERIAL_ID                     6

uint32_t  BOOT_DELAY      =           (1000 * 60 * 8);

double MASTER_GAIN_SCALER =           1.0;

// if false, a click detected on either side results in a LED flash on both sides
// if true, a click detected on one side will only result in a flash on that side
bool INDEPENDENT_FLASHES =               false; // WARNING NOT IMPLEMENTED - TODO

// WARNING NOT IMPLEMENTED - TODO
#if ENCLOSURE_TYPE == ORB_ENCLOSURE
#define   COMBINE_LUX_READINGS           true
#elif ENCLOSURE_TYPE == GROUND_ENCLOSURE
#define   COMBINE_LUX_READINGS           true
#endif // enclosure type

bool gain_adjust_active =                false;

// WARNING NOT IMPLEMENTED - TODO
#define DEACTIVATE_UNDER_EXTREME_LUX     true

// FIRMWARE MODE should be set to  CICADA_MODE, PITCH_MODE, or TEST_MODE
// depending on what functionality you want
#define FIRMWARE_MODE                   CICADA_MODE

// this needs to be included after the firmware_mode line so everything loads properly
#if FIRMWARE_MODE == PITCH_MODE
  #define NUM_AUTOGAINS                 1
  #define NUM_FEATURE_COLLECTORS        1
  #define NUM_NEO_GROUPS                2
  #define NUM_LUX_MANAGERS              2
  #include "Configuration_pitch.h"
#elif FIRMWARE_MODE == CICADA_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        2
  #define NUM_NEO_GROUPS                2
  #define NUM_LUX_MANAGERS              2
  #include "Configuration_cicada.h"
#elif FIRMWARE_MODE == TEST_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        0
  #define NUM_NEO_GROUPS                2
  #define NUM_LUX_MANAGERS              2  
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// Debuggings ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

///////////////////////// Cicada ///////////////////////////////////////////
#define PRINT_LBS                       false
//
#define PRINT_LUX_DEBUG                 false
#define PRINT_LUX_READINGS              false
#define PRINT_BRIGHTNESS_SCALER_DEBUG   false

#define PRINT_SONG_DEBUG                false
#define PRINT_SONG_BRIGHTNESS           false
#define PRINT_SONG_COLOR                false

#define PRINT_CLICK_FEATURES            false
#define PRINT_CLICK_DEBUG               false

#define PRINT_LED_VALUES                false
#define PRINT_LED_DEBUG                 false
#define PRINT_LED_ON_RATIO_DEBUG        false
#define PRINT_COLOR_WIPE_DEBUG          false

#define PRINT_AUTO_GAIN                 false

#define PRINT_LOG_WRITE                 false
#define DLM_PRINT                       false
// if LOOP_LENGTH is set to true the program will keep track of how long it takes to go through
// the main loop, it will also store the min and max loop length values as well as calculate 
// what the average loop length is
#define PRINT_LOOP_LENGTH               false

// perform a write check on everything that is written to EEPROM
#define EEPROM_WRITE_CHECK              false

#define PRINT_RMS_VALS                  false
#define PRINT_RMS_DEBUG                 false

#define PRINT_PEAK_VALS                 false
#define PRINT_PEAK_DEBUG                false

#define PRINT_TONE_VALS                 false

#define PRINT_FREQ_VALS                 false

//////////////////////////// FFT Printing ///////////////////////////////////
#define PRINT_FFT_DEBUG                 false
// for printing raw FFT values
#define PRINT_FFT_VALS                  false
// will print spectral flux if flux_active
#define PRINT_FLUX_VALS                 false
#define PRINT_CLICK_FLUX                false
// will print centroid if centroid_active
#define PRINT_CENTROID_VALS             false
// will print highest energy bin in FFT

////////////////////////////////////////////////////////////////////////////
///////////////////////// Lux    Settings //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define LUX_SENSORS_ACTIVE              true
// how long the lux sensors need the LEDs to be 
// turned off in order to get an accurate reading
#define LUX_SHDN_LEN                    40

bool front_lux_active =                 true;
bool rear_lux_active  =                 true;

#define LUX_CALIBRATION_TIME            3000

// will the current lux reading be averaged with the past lux reading
// This is generally a bad idea as the lux sensor has internal integration which
// already takes the average of several readings
#define SMOOTH_LUX_READINGS             false

// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               150.0
#define HIGH_LUX_THRESHOLD              1200.0
#define EXTREME_LUX_THRESHOLD           4000.0

// on scale of 0-1.0 what is the min multiplier for lux sensor brightness adjustment
#define BRIGHTNESS_SCALER_MIN           0.125
#define BRIGHTNESS_SCALER_MAX           2.00

uint32_t lux_max_reading_delay =        1000 * 60 * 2;   // every two minutes
uint32_t lux_min_reading_delay =        1000 * 15;       // fifteen seconds

////////////////////////////////////////////////////////////////////////////
///////////////////////// NeoPixel Settings ////////////////////////////////
////////////////////////////////////////////////////////////////////////////
uint16_t  MIN_BRIGHTNESS =              0;
uint16_t  MAX_BRIGHTNESS =              255;

#define UPDATE_ON_OFF_RATIOS            true
byte LED_DRAWING_MEMORY[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte LED_DISPLAY_MEMORY[NUM_LED * 12]; // 12 bytes per LED

#define CLICK_RED                       200
#define CLICK_GREEN                     200
#define CLICK_BLUE                      200

#define SONG_RED_LOW                    0
#define SONG_GREEN_LOW                  255
#define SONG_BLUE_LOW                   0

#define SONG_RED_HIGH                   175
#define SONG_GREEN_HIGH                 100
#define SONG_BLUE_HIGH                  5

////////////////////////////////////////////////////////////////////////////
///////////////////////// Jumper Settings //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED               1


////////////////////////////////////////////////////////////////////////////
///////////////////////// Auto-Gain Settings ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// turn on/off auto gain. 0 is off, 1 is on
#define AUTOGAIN_ACTIVE                 0
#define MAX_GAIN_ADJUSTMENT             0.125

// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain  can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define USE_LED_ON_RATIO                (1)
#define MIN_LED_ON_RATIO                (0.01)
#define MAX_LED_ON_RATIO                (0.5)

#define AUTOGAIN_START_DELAY            (1000 * 60)
// how often to calculate auto-gain (in ms)
#define AUTOGAIN_FREQUENCY              (1000 * 60 * 5)

/////////////////////////////////////////////////////////////////////////
//////////////////////// Audio Settings /////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// how often does the feature collector update
//33 is 30 times a second
#define FC_UPDATE_RATE                  20
#define AUDIO_MEMORY                    16

// the scaler values are applied to the raw readings read from the audio objects
// TODO - in the future there needs to be a form of dynamic adjusting of these values according 
// to some logic
#if FIRMWARE_MODE == CICADA_MODE 
    double global_peak_scaler =         1.0   * ENC_ATTENUATION_FACTOR;
    double global_rms_scaler  =         3.5   * ENC_ATTENUATION_FACTOR;
    double global_fft_scaler  =         100.0 * ENC_ATTENUATION_FACTOR;
#elif FIRMWARE_MODE == CICADA_MODE_NEW 
    double global_peak_scaler =         1.0   * ENC_ATTENUATION_FACTOR;
    double global_rms_scaler  =         3.5   * ENC_ATTENUATION_FACTOR;
    double global_fft_scaler  =         100.0 * ENC_ATTENUATION_FACTOR;
#elif FIRMWARE_MODE == PITCH_MODE
    double global_peak_scaler =         100.0  * ENC_ATTENUATION_FACTOR;
    double global_rms_scaler  =         100.0  * ENC_ATTENUATION_FACTOR;
    double global_fft_scaler  =         1000.0 * ENC_ATTENUATION_FACTOR;
#endif

bool stereo_audio =                     true;
#if ENCLOSURE_TYPE == GROUND_ENCLOSURE
 uint8_t num_channels = 1;
#elif ENCLOSURE_TYPE == ORB_ENCLOSURE
 uint8_t num_channels =                  stereo_audio + 1;
#endif

// these are the default values which set front_mic_active
// if the microphone test is run and it is found that one of the microphones is
// not working properly, then the variables will be switched to false
bool front_mic_active =                 FRONT_MICROPHONE_INSTALLED;
bool rear_mic_active =                  REAR_MICROPHONE_INSTALLED;

// audio usage loggings
uint8_t audio_usage_max =               0;
elapsedMillis last_usage_print =        0;// for keeping track of audio memory usage

/////////////////////////////////////////////////////////////////////////
/////////////////////////      Feature Collector     ////////////////////
/////////////////////////////////////////////////////////////////////////
// minimum amount of time between peak-log resets  which is allowed.
#define PEAK_LOG_RESET_MIN              2000
#define RMS_LOG_RESET_MIN               2000

// used the scaled FFT readings or the normal FFT readings, the scaled readings will eensure that
// all the bins of intrest will have their magnitudes add up to 1, thus is best used for determining the centroid within a sub frequency range (for instance 8k - 14k or something
#define SCALE_FFT_BIN_RANGE             false


#endif // CONFIGURATION_H
