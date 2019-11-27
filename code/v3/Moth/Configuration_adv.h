#ifndef CONFIGURATION_ADV_H
#define CONFIGURATION_ADV_H

#include "Datalog_Configuration.h"
#include "Hardware_Configuration.h"
////////////////////////// Lux Sensors /////////////////////////////////////////
// TODO these need to change into variables which are set with the jumpers?

// turn this flag off if no I2C multiplexer is present
#define I2C_MULTI 1

// how long the lux sensors need the LEDs to be turned off in order to get an accurate reading
#define LUX_SHDN_LEN 40

///////////////////////// Audio //////////////////////////////////////////////////
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
#define AUDIO_MEMORY 24

#define TCA_CHANNELS 2

#endif // CONFIGURATION_ADV_H
