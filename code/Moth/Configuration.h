#ifndef CONFIGURATION_H
#define CONFIGURATION_H 

#include "Configuration_hardware.h"
#include <Audio.h>
#include "Macros.h"
#include <PrintUtils.h>


////////////////////////////////////////////////////////////////////////////
/////////////////////////// MOST IMPORTANT /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define AUTOGAIN_ACTIVE                 0
////////////////////////////////////////////////////////////////////////////
/////////////////////////// User Controls //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// should correspond to the serial number on the PCB
#define SERIAL_ID            4


#define CLICK_ACTIVE                    false

// if set to true an audio USB object will be created so the audio can be debuged via Audacity
#define AUDIO_USB_DEBUG                 true

// if stereo feedback is set to true than only audio from both channels will be used to calculate visual feedback brightness and color
// not generally recommended...
#define STEREO_FEEDBACK                 false

#define P_USER_CONTROLS             true

// this wll allow the POT to overide the brightness at the end of color wipe
#define USER_BRIGHTNESS_OVERDRIVE   true
// which pot will be used to control thebrightness overide
#define BS_POT_NUM                  0
#define POT_BS_MAX                  5.0
#define POT_BS_MIN                  0.05

#define MAX_FPS                         60.0
elapsedMillis loop_tmr = 0; 
uint32_t loop_length = (uint32_t)((double)1000.0 / (double)MAX_FPS);
////////////////////////////////////////////////////////////////////////////
/////////////////////////// Pots ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MAX_POT_BS       3.0
#define MIN_POT_BS       0.1

////////////////////////////////////////////////////////////////////////////
/////////////////////////// User Controls //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/* This is the main configuration file for the Moth Framework
 * Using this file, along with the other configuration files you
 * Can cistomise how the firmware performs.
 * 
 * ----------------------- JUMPER PINS --------------------------
 * 
 */

#if H_VERSION_NUMBER == 3
#define USER_CONTROL_POLL_RATE         1000
#elif H_VERSION_NUMBER < 3
#define USER_CONTROL_POLL_RATE         8000
#endif // H_VERSION_NUMBER

///////////////////////////// Brightness Thresholds ////////////////////////
// 0.0 - 1.0 any target brightness calculated below this threshold will be discarded
// in favor of a brightness of 0.0
#define BRIGHTNESS_CUTTOFF_THRESHOLD  0.2

////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Brightness Scalers////////////////////////
////////////////////////////////////////////////////////////////////////////
// the local brightness scaler will adjust the brightness that would normally be displayed
// to utalise the entire dynamic range available
#define LBS_ACTIVE                     true
// how often should the LBS be recalculated?
#define LBS_TIME_FRAME                 (1000 * 60 * 10)
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

// how long will the bot wait until starting the main loop
// this is useful for when it neeeds to bee put into an enclosure and then installed in the environment
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
#define DEACTIVATE_UNDER_EXTREME_LUX     false

// this needs to be included after the firmware_mode line so everything loads properly
#if FIRMWARE_MODE == PITCH_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        2
  #define NUM_NEO_GROUPS                2
  #define NUM_LUX_MANAGERS              1
  #include "Configuration_pitch.h"
#elif FIRMWARE_MODE == CICADA_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        2
  #define NUM_NEO_GROUPS                2
  #define NUM_LUX_MANAGERS              1
  #include "Configuration_cicada.h"
#elif FIRMWARE_MODE == TEST_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        0
  #define NUM_NEO_GROUPS                2
  #define NUM_LUX_MANAGERS              1  
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// Debuggings ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

///////////////////////// Cicada ///////////////////////////////////////////
#define P_LBS                       false

// print lux debug mostly prints info about when extreme lux is entered and 
// other things in the lux manager, it is reccomended to leave this printing on
#define P_LUX_DEBUG                 true
#define P_LUX_READINGS              true
#define P_BRIGHTNESS_SCALER_DEBUG   true
#define P_CALCULATE_BRIGHTNESS_LENGTH false 

#define P_SONG_DEBUG                false
#define P_SONG_BRIGHTNESS           false
#define P_SONG_COLOR                false

// basically do you want to print the number of song updates which occur every second?
#define P_NUM_SONG_UPDATES          false
#if P_NUM_SONG_UPDATES == 1
uint16_t song_updates = 0;
elapsedMillis song_update_timer = 0;
#endif // P_NUM_SONG_UPDATES

#define P_UPDATE_SONG_LENGTH        false

#define P_CLICK_FEATURES            false
#define P_CLICK_DEBUG               false

#define P_LED_ON_RATIO_DEBUG        false
#define P_COLOR_WIPE_DEBUG          false

#define P_AUTO_GAIN                 false

#define P_LOG_WRITE                 false
#define DLM_PRINT                   false
// if LOOP_LENGTH is set to true the program will keep track of how long it takes to go through
// the main loop, it will also store the min and max loop length values as well as calculate 
// what the average loop length is
#define P_LOOP_LENGTH               false

// perform a write check on everything that is written to EEPROM
#define EEPROM_WRITE_CHECK          false

#define P_RMS_VALS                  false
#define P_RMS_DEBUG                 false

#define P_PEAK_VALS                 false
#define P_PEAK_DEBUG                false

#define P_TONE_VALS                 false

#define P_FREQ_VALS                 false

#define P_POT_VALS                  true

#define P_AUDIO_USAGE_MAX           false

//////////////////////////// FFT Printing ///////////////////////////////////
#define P_FFT_DEBUG                 false
// for printing raw FFT values
#define P_FFT_VALS                  false
// will print spectral flux if flux_active
#define P_FLUX_VALS                 false
#define P_CLICK_FLUX                false
// will print centroid if centroid_active
#define P_CENTROID_VALS             false
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
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#if H_VERSION_MAJOR , 3
#define EXTREME_LUX_THRESHOLD           4000.0
#else
#define EXTREME_LUX_THRESHOLD           6000.0
#endif
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
#define CLICK_BLUE                      255

#define SONG_RED_LOW                    50
#define SONG_GREEN_LOW                  255
#define SONG_BLUE_LOW                   50

#define SONG_RED_HIGH                   255
#define SONG_GREEN_HIGH                 60
#define SONG_BLUE_HIGH                  60

////////////////////////////////////////////////////////////////////////////
///////////////////////// Auto-Gain Settings ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// turn on/off auto gain. 0 is off, 1 is on
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

////////////////////////////////////////////////////////////////////////////
////////////////////// Software Configurations /////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define S_VERSION_MAJOR           0
#define S_VERSION_MINOR           2
#define S_SUBVERSION              1
// version 0.2.1 was creaeted on 29.06.20 and got a working version of the code working for PCB v2.1 in Cicada Mode as well as
//   improving the frame rate from 8 to 29, and adding support for PCB v3 in Cicada mode (minus the click)
// version 0.2.0 was created on 07/05/20 as the first attempt to get everything workin on PCB v3
// version 0.1.0 was created on 25.02.20 to address issues with the ground enclosure not being as responsive
// as well as the autobrightness calibration routine being very visible and disruptive when resetting
// version 0.1.0 was created on 24.02.20 as it served as the first viable working song implementation
// version 0.0.5 was created on 20.02.20 in the field at Kiatoke Cicada Grove as a first reaction to 
// how version 0.0.4 was performing, the changes included higher starting gains, a brighter flash,
// and the addition of a flag to differentiate between adding the flash brightness of just displaying
// the brightness

////////////////////////////////////////////////////////////////////////////
////////////////////// Neopixel Stuff  /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// if this is true the flash messages will erase other brightness values
// if it is false then the flash values will be added to existing values
bool FLASH_DOMINATES = false;

// how high the click flash timer will go up to
#define MAX_FLASH_TIME            60
// where the click flash timer will start
#define MIN_FLASH_TIME            40
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME       80

///////////////////////////////// General Purpose Functions //////////////////////////////////
#define SERIAL_BAUD_RATE          115200

#define LED_MAPPING_STANDARD 0
#define LED_MAPPING_BOTTOM_UP 1
#define LED_MAPPING_ROUND 2
#define LED_MAPPING_CENTER_OUT 3

// TODO need to add this to 
uint8_t LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;

#endif // CONFIGURATION_H
