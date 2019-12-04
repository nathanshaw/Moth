#ifndef CONFIGURATION_ADV_H
#define CONFIGURATION_ADV_H

#include "Datalog_Configuration.h"
#include "Hardware_Configuration.h"
#include <Audio.h>

////////////////////////// Lux Sensors /////////////////////////////////////////
// how long the lux sensors need the LEDs to be turned off in order to get an accurate reading
#define LUX_SHDN_LEN 40
double combined_lux;
double combined_min_lux_reading;
double combined_max_lux_reading;

////////////////////////// TCA Bus Expanders ///////////////////////////////////
// I2C_MULTI should be 0 if no TCA I2C bus expander is present on the PCB
// I2C MULTI should be 1 if a TCA I2C bus expander is present
#define I2C_MULTI 1

///////////////////////// Audio //////////////////////////////////////////////////
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

////////////////////////////// Auto Gain ////////////////////////////////////


elapsedMillis last_auto_gain_adjustment; // the time in which the last auto_gain_was_calculated

////////////////////////// Neo Pixels ///////////////////////////////////////////
#define UPDATE_ON_OFF_RATIOS true

byte drawingMemory[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LED * 12]; // 12 bytes per LED

////////////////////////////////////////////////////////////////////////////
//////////////////////// Hardware Controls /////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// how high the click flash timer will go up to
#define MAX_FLASH_TIME 200
// where the click flash timer will start
#define MIN_FLASH_TIME 40
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME 200

// cicada (-) or bird song (+) mode
#define JMP1_PIN 12
bool cicada_mode = true;

// mono audio (-) or stereo (+)
#define JMP2_PIN 11
bool stereo_audio = true;
uint8_t num_channels = stereo_audio + 1;
uint8_t active_channels[2];

#define FRONT_MIC true
#define REAR_MIC true

// one front lux (-) or two lux (+)
#define JMP3_PIN 14

// if false, a click detected on either side results in a LED flash on both sides
// if true, a click detected on one side will only result in a flash on that side
bool INDEPENDENT_CLICKS = true;

// independent lux readings (-) or average/combine lux readings (+)
#define JMP4_PIN 15
bool combine_lux_readings = false;

// auto gain adjust (-) or not (+)
#define JMP5_PIN 16
bool gain_adjust_active = true;

// data log not active (-) or active (+)
#define JMP6_PIN 17

// #endif // __PINS_H__
#define AUDIO_MEMORY 40

// the number of active channels on the TCA (can in theory support 8 sensors, etc.)
#define TCA_CHANNELS 2

// for scaling the peak readings in the Audio Engine, to make it easier to debug things, etc.
#define PEAK_SCALER 10.0
#define RMS_SCALER 10.0

///////////////////////////////// Datalogging shizzzz /////////////////
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

#define UPDATING_LOG 0
#define ONE_TIME_LOG 1
#define DATALOG_MANAGER_MAX_LOGS  10
#define DATALOG_MANAGER_TIMER_NUM 4
#endif // CONFIGURATION_ADV_H
