#ifndef CONFIGURATION_H
#define CONFIGURATION_H 
/* This is the main configuration file for the Moth Framework
 * Using this file, along with the other configuration files you
 * Can cistomise how the firmware performs.
 */
#include "Datalog_Configuration.h"
#include "Hardware_Configuration.h"
#include <Audio.h>
#include "Macros.h"
#include <PrintUtils.h>

#if FIRMWARE_MODE == CICADA_MODE
  #include "Configuration_cicadas.h"
#endif

#if FIRMWARE_MODE == PITCH_MODE
  #include "Configuration_pitch.h"
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// General Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define SERIAL_ID                       5

////////////////////////////////////////////////////////////////////////////
///////////////////////// Operating Modes //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// the current modes, or software driven functionality in which 
// the firmware should use.
#define CICADA_MODE                     0
#define PITCH_MODE                      1
// FIRMWARE MODE should be set to one  of of the modes defined above...
// options are CICADA_MODE and PITCH_MODE
#define FIRMWARE_MODE                   (CICADA_MODE)

// this needs to be included after the firmware_mode line so everything loads properly
#if FIRMWARE_MODE == PITCH_MODE
  #include "Configuration_pitch.h"
#elif FIRMWARE_MODE == CICADA_MODE
  #include "Configuration_cicadas.h"
#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// Debuggings ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS           true

///////////////////////// Cicada ///////////////////////////////////////////
#define PRINT_LUX_DEBUG                 false
#define PRINT_LUX_READINGS              false
#define PRINT_BRIGHTNESS_SCALER_DEBUG   false
#define PRINT_SONG_DATA                 false
#define PRINT_CLICK_FEATURES            false
#define PRINT_CLICK_DEBUG               false
#define PRINT_LED_VALUES                false
#define PRINT_AUTO_GAIN                 false
#define PRINT_LED_DEBUG                 false
#define PRINT_LOG_WRITE                 true
#define EEPROM_WRITE_CHECK              false

///////////////////////// Feature Collector ///////////////////////////////
// feature collector related
#define PRINT_RMS_VALS                  false
#define PRINT_PEAK_VALS                 false
#define PRINT_TONE_VALS                 false
#define PRINT_FREQ_VALS                 false
#define PRINT_FFT_VALS                  false

////////////////////////////////////////////////////////////////////////////
///////////////////////// Lux    Settings //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// how long the lux sensors need the LEDs to be 
// turned off in order to get an accurate reading
#define LUX_SHDN_LEN 40
double combined_lux;
double combined_min_lux_reading;
double combined_max_lux_reading;
bool front_lux_active = true;
bool rear_lux_active = true;

#define LUX_SENSORS_ACTIVE              true
#define LUX_CALIBRATION_TIME            4000
#define SMOOTH_LUX_READINGS             true

// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD               16.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD               100
#define HIGH_LUX_THRESHOLD              450.0

// on scale of 0-1.0 what is the min multiplier for lux sensor brightness adjustment
#define BRIGHTNESS_SCALER_MIN           0.5
#define BRIGHTNESS_SCALER_MAX           1.5

uint32_t lux_max_reading_delay = long(1000.0 * 60.0 * 0.05); // every 6 minutes
uint32_t lux_min_reading_delay = long(1000.0 * 60.0 * 0.01); // one minute

////////////////////////////////////////////////////////////////////////////
///////////////////////// NeoPixel Settings ////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MIN_BRIGHTNESS                  10
#define MAX_BRIGHTNESS                  255
#define UPDATE_ON_OFF_RATIOS true
byte drawingMemory[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LED * 12]; // 12 bytes per LED

////////////////////////////////////////////////////////////////////////////
///////////////////////// Datalog Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
elapsedMillis runtime;
bool data_logging_active = true;

// does the autolog get written over each time?
#define CLEAR_EEPROM_CONTENTS           0

// how long will each of the four different auto-log options be? 
// // a -1 means that the log will keep updating forever
#define DATALOG_1_LENGTH                40
#define DATALOG_2_LENGTH                40
#define DATALOG_3_LENGTH                -1
#define DATALOG_4_LENGTH                -1

// record the run time // last value is number of minutes
#define DATALOG_TIMER_1                 (1000*60*1)
#define DATALOG_TIMER_2                 (1000*60*1)
#define DATALOG_TIMER_3                 (1000*60*1)
#define DATALOG_TIMER_4                 (1000*60*1)

#define DATALOG_START_DELAY_1           (1000*60*0.05)
#define DATALOG_START_DELAY_2           (1000*60*0.05)
#define DATALOG_START_DELAY_3           (1000*60*0.05)
#define DATALOG_START_DELAY_4           (1000*60*0.05)

#define DATALOG_TIME_FRAME_1            (1000*60*60*0.25)
#define DATALOG_TIME_FRAME_2            (1000*60*60*5)
#define DATALOG_TIME_FRAME_3            (1000*60*60*0.25)
#define DATALOG_TIME_FRAME_4            (1000*60*60*5)

// refresh rates for the static logs
#define STATICLOG_RATE_FAST             (1000*60*3)
#define STATICLOG_RATE_SLOW             (1000*60*12)

/////////////////// for the auto logging ////////////////////////////////////
// will the lux readings be logged?
#define AUTOLOG_LUX_F                   1
#define AUTOLOG_LUX_R                   1
#define AUTOLOG_LUX_TIMER               0
// the ratio of on vs off time for the neopixels
#define AUTOLOG_LED_ON_OFF_F            1
#define AUTOLOG_LED_ON_OFF_R            1
#define AUTOLOG_LED_ON_OFF_TIMER        0
// the number of values to store in the logging process
#define AUTOLOG_FLASHES_F               1
#define AUTOLOG_FLASHES_R               1
#define AUTOLOG_FLASHES_TIMER           0
// the brightness scaler avg log
#define AUTOLOG_BRIGHTNESS_SCALER_F     1
#define AUTOLOG_BRIGHTNESS_SCALER_R     1
#define AUTOLOG_BRIGHTNESS_SCALER_TIMER 0

/////////////////// for the static logging /////////////////////////////////
#define STATICLOG_CLICK_GAIN            1
#define STATICLOG_SONG_GAIN             1
#define STATICLOG_LUX_VALUES            1
#define STATICLOG_FLASHES               1
#define STATICLOG_RUNTIME               1

#define STATICLOG_LUX_MIN_MAX_TIMER     3
#define STATICLOG_CLICK_GAIN_TIMER      3
#define STATICLOG_SONG_GAIN_TIMER       3
#define STATICLOG_FLASHES_TIMER         3
#define STATICLOG_RUNTIME_TIMER         3

////////////////////////////////////////////////////////////////////////////
///////////////////////// Jumper Settings //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED 0

////////////////////////////////////////////////////////////////////////////
/////////////////////////// Datalog Manager ////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// will be written once at the setup loop then will never write again
#define DATALOG_TYPE_INIT               0
// will write to the same addr over and over again when commanded to do so
#define DATALOG_TYPE_UPDATE             1
// log consists of several memory locations for its values and will increment its index
// with each read until the space runs out then it will stop logging
// Note that the timing of the updates are determined by the datalogmanager class
#define DATALOG_TYPE_AUTO               2
#define UPDATING_LOG                    0
#define ONE_TIME_LOG                    1
#define DATALOG_MANAGER_MAX_LOGS        25
#define DATALOG_MANAGER_TIMER_NUM       4
uint8_t datalog_timer_num = 4;
uint32_t datalog_timer_lens[4] = {DATALOG_TIMER_1, DATALOG_TIMER_2, DATALOG_TIMER_3, DATALOG_TIMER_4};

////////////////////////////////////////////////////////////////////////////
///////////////////////// Auto-Gain Settings ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// turn on/off auto gain. 0 is off, 1 is on
#define AUTO_GAIN_ACTIVE                1
#define MAX_GAIN_ADJUSTMENT             0.10

// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define USE_LED_ON_RATIO                (1)
#define MIN_LED_ON_RATIO                (0.3)
#define MAX_LED_ON_RATIO                (0.95)

// the time in which the last auto_gain_was_calculated
elapsedMillis last_auto_gain_adjustment; 
const uint32_t autogain_frequency = 1000 * 60 * 10; // how often to calculate auto-gain (in ms)

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// will there be a USB audio output object created?
#define USB_OUTPUT                    1

/////////////////////////////////////////////////////////////////////////
//////////////////////// Audio Settings /////////////////////////////////
/////////////////////////////////////////////////////////////////////////
#define AUDIO_MEMORY                  40
// for scaling the peak readings in the Audio Engine
// to make it easier to debug things, etc.
#define PEAK_SCALER                   10.0
#define RMS_SCALER                    10.0

bool stereo_audio = true;
uint8_t num_channels = stereo_audio + 1;
// these are the default values which set front_mic_active
// if the microphone test is run and it is found that one of the microphones is
// not working properly, then the variables will be switched to false
#define FRONT_MICROPHONE_INSTALLED    true
#define REAR_MICROPHONE_INSTALLED     true
bool front_mic_active = FRONT_MICROPHONE_INSTALLED;
bool rear_mic_active = REAR_MICROPHONE_INSTALLED;
// audio usage loggings
uint8_t audio_usage_max = 0;
elapsedMillis last_usage_print = 0;// for keeping track of audio memory usage

/////////////////////////////////////////////////////////////////////////
/////////////////////////      Leds     /////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#endif // CONFIGURATION_H
