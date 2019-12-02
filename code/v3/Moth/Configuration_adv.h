#ifndef CONFIGURATION_ADV_H
#define CONFIGURATION_ADV_H

#include "Datalog_Configuration.h"
#include "Hardware_Configuration.h"
#include <Audio.h>

////////////////////////// Lux Sensors /////////////////////////////////////////
// how long the lux sensors need the LEDs to be turned off in order to get an accurate reading
#define LUX_SHDN_LEN 40

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
bool data_logging_active = true;

// #endif // __PINS_H__
#define AUDIO_MEMORY 40

// the number of active channels on the TCA (can in theory support 8 sensors, etc.)
#define TCA_CHANNELS 2

// for scaling the peak readings in the Audio Engine, to make it easier to debug things, etc.
#define PEAK_SCALER 10.0
#define RMS_SCALER 10.0

#endif // CONFIGURATION_ADV_H
