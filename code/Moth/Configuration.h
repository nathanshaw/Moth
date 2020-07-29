/* This is the main configuration file for the Moth Framework
 * Using this file, along with the other configuration files you
 * Can cistomise how the firmware performs.
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H 

#include "Configuration_hardware.h"
#include <Audio.h>
#include "Macros.h"
#include <PrintUtils.h>

////////////////////////////////////////////////////////////////////////////
/////////////////////////// MOST IMPORTANT /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define LUX_ADJUSTS_BS                  0
#define LUX_ADJUSTS_MIN_MAX             1
uint8_t LUX_MAPPING_SCHEMA =            LUX_ADJUSTS_MIN_MAX;
// will autogain based on the LED ON/OFF time be active?
#define AUTOGAIN_ACTIVE                 false
// should correspond to the serial number on the PCB
#define SERIAL_ID                       13

// will a onset_detector be active?
#define ONSET_ACTIVE                    false
// if stereo feedback is set to true than only audio from both channels will be used to calculate visual feedback brightness and color
// not generally recommended...
#define STEREO_FEEDBACK                 false
// the local brightness scaler will adjust the brightness that would normally be displayed
// to utalise the entire dynamic range available
bool LBS_ACTIVE          =              false;
// if false, a onset detected on either side results in a LED flash on both sides
// if true, a onset detected on one side will only result in a flash on that side
bool INDEPENDENT_FLASHES =              false; // WARNING NOT IMPLEMENTED - TODO

////////////////////////////////////////////////////////////////////////////
///////////////////////// Debug Printing ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define P_LBS                           true

#define P_LEDS_ON                       false
// print lux debug mostly prints info about when extreme lux is entered and 
// other things in the lux manager, it is reccomended to leave this printing on
#define P_LED_ON_RATIO                  false
#define P_COLOR_WIPE                    false

#define P_SMOOTH_HSB                    false
#define P_HSB                           false
#define P_HUE                           false
#define P_BRIGHTNESS                    false
#define P_BRIGHTNESS_SCALER             true

#define P_NEO_COLORS                    false

#define P_EXTREME_LUX                   true
#define P_LUX                           true
#define P_LUX_READINGS                  true

#define P_CALCULATE_BRIGHTNESS_LENGTH   false 

// this is where the final brightness scaler is applied
#define P_PACK_COLORS                   false

#define P_SONG                          false
#define P_SONG_COLOR                    false

// basically do you want to print the number of song updates which occur every second?
#define P_NUM_SONG_UPDATES              false
#if P_NUM_SONG_UPDATES == 1
uint16_t song_updates = 0;
elapsedMillis song_update_timer = 0;
#endif // P_NUM_SONG_UPDATES

#define P_UPDATE_SONG_LENGTH            false

#define P_ONSET_FEATURES                false
#define P_ONSET                         false

#define P_AUTO_GAIN                     false

#define P_LOG_WRITE                     false
#define DLM_PRINT                       false
// if LOOP_LENGTH is set to true the program will keep track of how long it takes to go through
// the main loop, it will also store the min and max loop length values as well as calculate 
// what the average loop length is
#define P_LOOP_LENGTH                   false

// perform a write check on everything that is written to EEPROM
#define EEPROM_WRITE_CHECK              false

///////////////////////// Audio Stuff ///////////////////////////////////////
#define P_RMS_VALS                      false
#define P_RMS                           false
#define P_PEAK_VALS                     false
#define P_PEAK                          false
#define P_TONE_VALS                     false
#define P_FREQ_VALS                     false
#define P_AUDIO_USAGE_MAX               false

//////////////////////////// FFT Printing ///////////////////////////////////
#define P_FFT                           false
// for printing raw FFT values
#define P_FFT_VALS                      false
// will print spectral flux if flux_active
#define P_FLUX_VALS                     false
#define P_ONSET_FLUX                    false
// will print centroid if centroid_active
#define P_CENTROID_VALS                 false
// will print highest energy bin in FFT

//////////////////////////// User Controls ///////////////////////////////////
#define P_USER_CONTROLS                 true
#define P_POT_VALS                      true

//////////////////////////// EEPROM ///////////////////////////////////
// set to true if you want to print out data stored in EEPROM on boot
#define P_EEPROM_CONTENTS               false

////////////////////////////////////////////////////////////////////////////
/////////////////////////// User Controls //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// if set to true an audio USB object will be created so the audio can be debuged via Audacity
#define AUDIO_USB                       true

//////////////////////////// Pots ////////////////////////////////////////////
// this wll allow the POT to overide the brightness at the end of color wipe
#define USER_BRIGHTNESS_OVERDRIVE       true

///////////////////////////// P1 - Brightness Overide ////////////////////////
// which pot will be used to control thebrightness overide
// if USER_BS_ACTIVE is set to true the user will scale the natural 
// brightness levels (in pitch mode only) before being sent to the neopixel
// manager (which then might constrain according to Lux levels)
double user_brightness_scaler               = 1.0;

// this will determine if the USER will have control over the brightness scaler
// at this point v2.1 does this via jumpers and v3.0 does this via a pot
// so the default value is true
#define USER_BS_ACTIVE                        true

#if HV_MAJOR > 2
#define BS_POT_NUM                            0
#define POT_BS_MAX                            5.0
#define POT_BS_MIN                            0.05

#endif//HV_MAJOR

///////////////////////////// P3 - Onset Threshold ///////////////////////////
// if USER_ONSET_THRESH_OVERRIDE is set to true the user will determine the 
// threshold level for onsets this is used to determine the threshold for onset 
// events and is controlled by pot #3
#if HV_MAJOR > 2
#define USER_ONSET_THRESH_OVERRIDE            true
#else
#define USER_ONSET_THRESH_OVERRIDE            false
#endif//HV_MAJOR
// the third pot will be used for changing the onset threshold
#define OT_POT_NUM                            2
#define USER_OT_MIN                           0.1
#define USER_OT_MAX                           2.5
double ONSET_THRESH =                         1.0;

///////////////////////////// P4 - Brightness Threshold ////////////////////////
// the fourth pot will be used to change the song thresehold
// 0.0 - 1.0 any target brightness calculated below this threshold will be discarded
// in favor of a brightness of 0.0, this should be controled using pins or a POT if available...
// if USER_ONSET_THRESH_OVERRIDE is set to true the user will determine the threshold level for onsets
// this is used to determine the threshold for onset events and is controlled by pot #3
#if HV_MAJOR > 2
#define USER_BRIGHT_THRESH_OVERRIDE            true
#else
#define USER_BRIGHT_THRESH_OVERRIDE            false
#endif//HV_MAJOR

#if FIRMWARE_MODE == CICADA_MODE
double BRIGHTNESS_CUTTOFF_THRESHOLD = 0.15;
#elif FIRMWARE_MODE == PITCH_MODE
double BRIGHTNESS_CUTTOFF_THRESHOLD = 0.1;
#endif//FIRMWARE_MODE

// if > 0 then the brightness will be smoothed with a previous value
// thee higher the value the more it is smoothed
double SMOOTH_HSB   =              0.125;

#define BT_POT_NUM                            3   
#define USER_CUTTOFF_MIN                      0.0   
#define USER_CUTTOFF_MAX                      0.5

bool SATURATED_COLORS                =        true;
////////////////////////////////////////////////////////////////////////////
/////////////////////////// System Performance /////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MAX_FPS                         60.0
elapsedMillis loop_tmr = 0; 
uint32_t loop_length = (uint32_t)((double)1000.0 / (double)MAX_FPS);

/////////////////////////// jumper Pins ////////////////////////////////////
#if HV_NUMBER == 3
#define USER_CONTROL_POLL_RATE         1000
#elif HV_NUMBER < 3
#define USER_CONTROL_POLL_RATE         8000
#endif // HV_NUMBER

////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Brightness Scalers////////////////////////
////////////////////////////////////////////////////////////////////////////

// how often should the LBS be recalculated?
#define LBS_TIME_FRAME                 (1000 * 60 * 1)
// once the local min and max have been overwritten how long to collect readings for
// a new min and max before using the new values?
#define LBS_OVERLAP_TIME               (1000 * 30)

elapsedMillis lbs_timer;
// what percent from the low and high will be truncated to the lowest and highest value
#define LBS_LOW_TRUNCATE_THRESH       0.1
#define LBS_HIGH_TRUNCATE_THRESH      0.75
// currently the cicada mode will use integers to determine the LBS
#if FIRMWARE_MODE == CICADA_MODE
uint8_t lbs_min =                     255;
uint8_t lbs_max =                     0;
// to keep track of 
double lbs_min_temp =                 999999999.9; 
double lbs_max_temp =                 0.0;
// this is what the LBS will map the lowest feature results and highest feature results 
// TODO will perhaps need to make a 16bit version of this?, or change all my brightnesses to be stored using 16 bits instead of 8?
// uint8_t lbs_brightness_low =          0;
// uint8_t lbs_brightness_high =         255;
uint8_t lbs_scaler_min_thresh =       0;
uint8_t lbs_scaler_max_thresh =       255;
/////////////////////////////////
#elif FIRMWARE_MODE == PITCH_MODE
double lbs_min =                     1.0;
double lbs_max =                     0.0;
// to keep track of 
double lbs_min_temp =                 1.0; 
double lbs_max_temp =                 0.0;
// this is what the LBS will map the lowest feature results and highest feature results 
// TODO will perhaps need to make a 16bit version of this?, or change all my brightnesses to be stored using 16 bits instead of 8?
// double lbs_brightness_low =          0.0;
// double lbs_brightness_high =         1.0;

double lbs_scaler_min_thresh =       0.0;
double lbs_scaler_max_thresh =       1.0;

#endif // FIRMWARE_MODE
////////////////////////////////////////////////////////////////////////////
///////////////////////// General Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// how long will the bot wait until starting the main loop
// this is useful for when it neeeds to bee put into an enclosure and then installed in the environment
uint32_t  BOOT_DELAY      =           (1000 * 60 * 2);
double MASTER_GAIN_SCALER =           1.0;

bool gain_adjust_active =                false;

// WARNING NOT IMPLEMENTED - TODO
#define DEACTIVATE_UNDER_EXTREME_LUX     false

// this needs to be included after the firmware_mode line so everything loads properly
#if FIRMWARE_MODE == PITCH_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        1
  #define NUM_NEO_GROUPS                2
#elif FIRMWARE_MODE == CICADA_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        1
  #define NUM_NEO_GROUPS                2
#elif FIRMWARE_MODE == TEST_MODE
  #define NUM_AUTOGAINS                 0
  #define NUM_FEATURE_COLLECTORS        0
  #define NUM_NEO_GROUPS                2  
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// Lux    Settings //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
bool LUX_SENSORS_ACTIVE =               false;
// WARNING NOT IMPLEMENTED - TODO
#if ENCLOSURE_TYPE == ORB_ENCLOSURE
#define   COMBINE_LUX_READINGS           true
#elif ENCLOSURE_TYPE == GROUND_ENCLOSURE
#define   COMBINE_LUX_READINGS           true
#endif // enclosure type

// THESE should be determined by the LUX_READINGS
uint8_t  MIN_BRIGHTNESS =              0;
uint8_t  MAX_BRIGHTNESS =              255;

// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               10.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               350.0
#define HIGH_LUX_THRESHOLD              1200.0
#if HV_MAJOR < 3
#define EXTREME_LUX_THRESHOLD           4000.0
#else
#define EXTREME_LUX_THRESHOLD           6000.0
#endif
// on scale of 0-1.0 what is the min multiplier for lux sensor brightness adjustment
#define BRIGHTNESS_SCALER_MIN           0.05
#define BRIGHTNESS_SCALER_MAX           3.00

uint32_t lux_max_reading_delay =        1000 * 60 * 1;   // every minute
uint32_t lux_min_reading_delay =        1000 * 10;       // ten seconds

////////////////////////////////////////////////////////////////////////////
///////////////////////// NeoPixel Settings ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// this is needed for the forced lux 
#define UPDATE_ON_OFF_RATIOS            true

#define ONSET_RED                       200
#define ONSET_GREEN                     200
#define ONSET_BLUE                      255

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
    double global_peak_scaler =         1.0  * ENC_ATTENUATION_FACTOR;
    double global_rms_scaler  =         3.5  * ENC_ATTENUATION_FACTOR;
    double global_fft_scaler  =         100.0 * ENC_ATTENUATION_FACTOR;
#endif

bool stereo_audio =                     true;

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
////////////////////// Neopixel Stuff  /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////// LED General Settings /////////////
byte LED_DRAWING_MEMORY[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte LED_DISPLAY_MEMORY[NUM_LED * 12]; // 12 bytes per LED
// if this is true the flash messages will erase other brightness values
// if it is false then the flash values will be added to existing values
bool FLASH_DOMINATES =                  false;

// if this is true then the brightness will b = (b + b) * b; in order to reduce its value, and make loud events even more noticable
bool SCALE_DOWN_BRIGHTNESS =            true;

// how high the onset flash timer will go up to
#define MAX_FLASH_TIME                  60
// where the onset flash timer will start
#define MIN_FLASH_TIME                  40
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME             80

/*//////////////////////////// LED MAPPING MODE //////////////////////////////////////////////
 * The options which are being considered are 
 * LED_MAPPING_STANDARD
 *  All leds display the same brightness in tandum
 * LED_MAPPING_BOTTOM_UP
 *  The LEDs facing the ground are mapped first while the LEDs on the TOP
 *  are filled last
 * LED_MAPPING_ROUND
 *  The LEDs are displayed like around a clock face moving in a counter clockwise or a clockwise direction
 * LED_MAPPING_CENTER_OUT        
 *  The LEDS in the center of the PCB are mapped first while the outside LEDs are mapped last
*/
#if HV_MAJOR > 2
uint8_t LED_MAPPING_MODE = LED_MAPPING_CENTER_OUT;
#else
uint8_t LED_MAPPING_MODE = LED_MAPPING_STANDARD;
#endif //HV_MAJOR
///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Feature Collector /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Which Audio features will be activated?
#define FFT_FEATURE_ACTIVE                  1
#define PEAK_FEATURE_ACTIVE                 1
#define RMS_FEATURE_ACTIVE                  1
// these features are not currently implemented
// #define TONE_FEATURE_ACTIVE                 1
// #define FREQ_FEATURE_ACTIVE                 1

/////////////////////////////// Audio Features ////////////////////////////////////
// all the different features that are available to use through the feature collector for
// varioud mappings.
#define FEATURE_RMS                         (1)
#define FEATURE_RMS_POS_DELTA               (11)
#define FEATURE_RMS_AVG                     (12)
#define FEATURE_PEAK                        (2)
#define FEATURE_PEAK_POS_DELTA              (22)
#define FEATURE_PEAK_AVG                    (23)
#define FEATURE_FREQ                        (3)
#define FEATURE_TONE                        (4)
#define FEATURE_FFT                         (5)
#define FEATURE_FFT_ENERGY                  (6)
#define FEATURE_FFT_RELATIVE_ENERGY         (7)
#define FEATURE_FFT_MAX_BIN                 (8)
#define FEATURE_FFT_BIN_RANGE               (9)
#define FEATURE_STRONG_FFT                  (10)

//////////////////////////// FFT ////////////////////////////////////////////////
// When calculating things such as which bin has the most energy and so on,
// what bin is considered the "1st?" and which one is the last?
// todo what frequency does this correspond to?
#define FFT_LOWEST_BIN              1
// todo this needs to be calculated better?
#define FFT_NUM_BINS                511
#define FFT_HIGHEST_BIN             80
// when using the Freq function generator, what amount of uncertanity is allowed?
// #define FREQ_UNCERTANITY_ALLOWED    0.15

/////////////////////////////// Color Mapping /////////////////////////////////////
// when calculating the hue for the NeoPixel leds, what feature do you want to use?
// look under Audio Features for the Available Features
#define HUE_FEATURE                         (FEATURE_FFT_MAX_BIN)
#define BRIGHTNESS_FEATURE                  (FEATURE_PEAK_AVG)
#define SATURATION_FEATURE                  (FEATURE_PEAK_AVG)

// These are different color mapping modes
#define COLOR_MAPPING_RGB                     0
#define COLOR_MAPPING_HSB                     1

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#define COLOR_MAP_MODE              (COLOR_MAPPING_HSB)


#define  MODE_SINGLE_RANGE                    0
#define  MODE_ALL_BINS                        1
#define  MODE_BIN_ENERGY                      2

// should the centroid value be smoothed?
#define SMOOTH_CENTROID                       true

// what factor will the new values be scaled by compared to the old values 
// when calculating the song brightness target vs current levels a 1.0 will turn off
// the low filtering so only the new values will be used while 0.5 will result in the
// average of the old and new value to be used, a higher value will be a quicker responce
// the max value is 1.0 and the min value is 0.0
#define BRIGHTNESS_LP_LEVEL                   (double)0.85
#define COLOR_LP_LEVEL                        (double)0.65

///////////////////////////////////////////////////////////// SONG /////////////////////////////////////////////////////////////////
// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
int SONG_FEATURE =                            PEAK_RAW;
int SONG_COLOR_FEATURE =                      SPECTRAL_CENTROID;

// these were 0.25
#define SONG_PEAK_THRESH                      0.15
#define SONG_RMS_THRESH                       0.15

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG                     0.002
#define MAX_SONG_PEAK_AVG                     0.2

// this is the gain that the input amps will be set at at the start of the installation
// #define STARTING_SONG_GAIN              8.0
// #define STARTING_ONSET_GAIN             16.0

#if HV_MAJOR < 3
#define STARTING_GAIN                         80.0
#elif HV_MAJOR == 3
// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
#define STARTING_GAIN                         30.0
#endif

#define SONG_BQ1_THRESH                       4000
#define SONG_BQ1_Q                            0.85
#define SONG_BQ1_DB                           -12
#define SONG_BQ2_THRESH                       16000
#define SONG_BQ2_Q                            0.85
#define SONG_BQ2_DB                     -12

// #define MIN_SONG_GAIN                        (STARTING_SONG_GAIN * 0.5)
// #define MAX_SONG_GAIN                        (STARTING_SONG_GAIN * 1.5)

///////// Onset Settings
// what feature will be used to determine if a onset has been found
// "rms_delta" will use that feature along with ONSET_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with ONSET_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
#define CALCULATE_FLUX                        true
#define CALCULATE_CENTROID                    true
#define CALCULATE_ROFF                        false

int ONSET_FEATURE =                           PEAK_DELTA;
#define ONSET_RMS_DELTA_THRESH                0.05
#define ONSET_PEAK_DELTA_THRESH               0.17
#define ONSET_SPECTRAL_FLUX_THRESH            100.0
#define SPECTRAL_FLUX_SCALER                  1

// these values are used to ensure that the light do not turn on when there are low 
// amplitudes in the sonic environment
#define RMS_LOW_THRESH                        0.1
#define PEAK_LOW_THRESH                       0.15
/*
 * TODO
 */
// one onset per minute
#define MIN_FPM_THRESH                  1 
#define LOW_FPM_THRESH                  1.0
#define HIGH_FPM_THRESH                 40
#define MAX_FPM_THRESH                  60.0

/*
 *  TODO
 */
#define ON_RATIO_FEEDBACK               1
#define MIN_ON_RATIO_THRESH             0.05
#define LOW_ON_RATIO_THRESH             0.40
#define HIGH_ON_RATIO_THRESH            0.60
#define MAX_ON_RATIO_THRESH             0.90

/*
 * TODO
 */
#define ONSET_BQ1_THRESH                1200
#define ONSET_BQ1_Q                     0.85
#define ONSET_BQ1_DB                    -12
#define ONSET_BQ2_THRESH                3000
#define ONSET_BQ2_Q                     0.85
#define ONSET_BQ2_DB                    -12

//////////////////////////////// Global Variables /////////////////////////
double color_feature_min = 1.00;
double color_feature_max = 0.0;

elapsedMillis feature_reset_tmr;
const unsigned long feature_reset_time = (1000 * 2.5);// every 2.5 minute?

double brightness_feature_min = 1.0;
double brightness_feature_max = 0.0;

double current_brightness = 1.0;
double last_brightness = 1.0;

double last_hue = 0.0;
double last_saturation = 0.0;
double hue = 0.0;

double current_color = 0.5;
double last_color = 0.5;

double last_feature;
double current_feature;

//////////////////////////////// Onset ////////////////////////////////////
elapsedMillis onset_feature_reset_tmr;
const unsigned long onset_feature_reset_time = (1000 * 60 * 1);// every 5 minute?

double last_range_rms = 0.0;
double min_rrms = 1.0;
double max_rrms = 0.1;

double last_flux = 0.0;
double min_flux = 1.0;
double max_flux = 0.1;

double last_cent_degd = 0.0;
double min_cent_negd = 1.0;
double max_cent_negd = 0.0;

double min_onset_feature = 1.0;
double max_onset_feature = 0.0;

double onset_flux = 0.0;
double onset_rrms = 0.0;
double onset_cent = 0.0;

#endif // CONFIGURATION_H
