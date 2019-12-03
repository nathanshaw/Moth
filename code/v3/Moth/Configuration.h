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
// the current modes, or software driven functionality in which the firmware should use.
#define CICADA_MODE 0
#define PITCH_MODE  1

// FIRMWARE MODE should be set to one  of of the modes defined above...
#define FIRMWARE_MODE (CICADA_MODE)

// this needs to be included after the firmware_mode line so everything loads properly
#if FIRMWARE_MODE == PITCH_MODE
  #include "Configuration_pitch.h"
#elif FIRMWARE_MODE == CICADA_MODE
  #include "Configuration_cicadas.h"
#endif

///////////////////////// Lux    Settings /////////////////////////////////
bool front_lux_active = true;
bool rear_lux_active = true;

#define LUX_SENSORS_ACTIVE        true
#define LUX_CALIBRATION_TIME      4000
#define SMOOTH_LUX_READINGS       true

// this is the threshold in which anything below will just be treated as the lowest reading
#define LOW_LUX_THRESHOLD         16.0
// when a lux of this level is detected the LEDs will be driven with a brightness scaler of 1.0
#define MID_LUX_THRESHOLD         100
#define HIGH_LUX_THRESHOLD        450.0

// on scale of 0-1.0 what is the min multiplier for lux sensor brightness adjustment
#define BRIGHTNESS_SCALER_MIN     0.5
#define BRIGHTNESS_SCALER_MAX     1.5

uint32_t lux_max_reading_delay = long(1000.0 * 60.0 * 6); // every 6 minutes
uint32_t lux_min_reading_delay = long(1000.0 * 60.0 * 1); // one minute

///////////////////////// NeoPixel Settings ///////////////////////////////
#define MIN_BRIGHTNESS            10
#define MAX_BRIGHTNESS            255

////////////////////////////////////////////////////////////////////////////
///////////////////////// Datalog Settings /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// record the run time // last value is number of minutes
#define DATALOG_TIMER_1       (1000*60*1)
#define DATALOG_TIMER_2       (1000*60*10)
#define DATALOG_TIMER_3       (1000*60*30)
#define DATALOG_TIMER_4       (1000*60*60)

#define DATALOG_START_DELAY_1 (1000*60*0)
#define DATALOG_START_DELAY_2 (1000*60*1)
#define DATALOG_START_DELAY_3 (1000*60*10)
#define DATALOG_START_DELAY_4 (1000*60*60)

#define DATALOG_TIME_FRAME_1  (1000*60*60*0.1)
#define DATALOG_TIME_FRAME_2  (1000*60*60*0.25)
#define DATALOG_TIME_FRAME_3  (1000*60*60*0.5)
#define DATALOG_TIME_FRAME_4  (1000*60*60*50)

uint8_t datalog_timer_num = 4;
uint32_t datalog_timer_lens[4] = {DATALOG_TIMER_1, DATALOG_TIMER_2, DATALOG_TIMER_3, DATALOG_TIMER_4};

// will the lux readings be logged?
#define AUTOLOG_LUX        1
// the number of values to store in the logging process
#define LUX_LOG_LENGTH     40

#define AUTOLOG_FLASHES    0
#define SLASHES_LOG_LENGTH 40
///////////////////////// Auto-Gain Settings /////////////////////////////////
// turn on/off auto gain. 0 is off, 1 is on
// #define AUTO_GAIN 1

// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define USE_LED_ON_RATIO              (1)
#define MIN_LED_ON_RATIO              (0.3)
#define MAX_LED_ON_RATIO              (0.95)

///////////////////////// Debuggings ////////////////////////////////////
//
///////////////////////// Cicada ////////////////////////////////////////
#define PRINT_LUX_DEBUG               true
#define PRINT_LUX_READINGS            true
#define PRINT_BRIGHTNESS_SCALER_DEBUG false
// TODO
#define PRINT_SONG_DATA               false
// TODO
#define PRINT_CLICK_FEATURES          false
#define PRINT_CLICK_DEBUG             false
// TODO
#define PRINT_LED_VALUES              false
#define PRINT_AUTO_GAIN               false

#define PRINT_LED_DEBUG               false

#define PRINT_LOG_WRITE               true

#define EEPROM_WRITE_CHECK            false

///////////////////////// Feature Collector ///////////////////////////////
// feature collector related
#define PRINT_RMS_VALS        false
#define PRINT_PEAK_VALS       false
#define PRINT_TONE_VALS       false
#define PRINT_FREQ_VALS       false
#define PRINT_FFT_VALS        false

// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS true

// minimum amount of time between peak-log resets  which is allowed.
#define PEAK_LOG_RESET_MIN    2000
#define USE_SCALED_FFT 1

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// will there be a USB audio output object created?
#define USB_OUTPUT 1

/////////////////////////////////////////////////////////////////////////
//////////////////////// Auto-gain Settings /////////////////////////////
/////////////////////////////////////////////////////////////////////////
#define MAX_GAIN_ADJUSTMENT 0.10
const uint32_t auto_gain_frequency = 1000 * 60 * 10; // how often to calculate auto-gain (in ms)

/////////////////////////////////////////////////////////////////////////
//////////////////////// Audio Settings /////////////////////////////////
/////////////////////////////////////////////////////////////////////////


//////////////////// Leds
// for the update to EEPROM on how long the program has been running for
elapsedMillis last_runtime_update;

#endif // CONFIGURATION_H
