#ifndef CONFIGURATION_ADV_H
#define CONFIGURATION__ADV_H

// TODO these need to change into variables which are set with the jumpers?
#define FRONT_LUX_INSTALLED true
#define REAR_LUX_INSTALLED true
// turn this flag off if no I2C multiplexer is present
#define I2C_MULTI 1


// this file assigns pin values to different variables depending on the version of the board
/*
#ifndef __PINS_H__
#define __PINS_H__
*/
////////////////////////////////////////////////////////////////////////////
//////////////////////// Hardware Controls /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
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
bool both_lux_sensors = true;
uint8_t num_lux_sensors = 2;
// if false, a click detected on either side results in a LED flash on both sides
// if true, a click detected on one side will only result in a flash on that side
bool INDEPENDENT_CLICKS = true;

// independent lux readings (-) or average/combine lux readings (+)
#define JMP4_PIN 15
bool combine_lux_readings = true;

// auto gain adjust (-) or not (+)
#define JMP5_PIN 16
bool gain_adjust_active = true;

// data log not active (-) or active (+)
#define JMP6_PIN 17
bool data_logging_active = true;

// #endif // __PINS_H__

#endif // CONFIGURATION_ADV_H
