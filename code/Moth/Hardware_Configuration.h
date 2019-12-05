#ifndef __HARDWARE_CONFIGURATION_H__
#define __HARDWARE_CONFIGURATION_H__
// contains all the hardware specific variables
#define HARDWARE_NAME "Moth"

#define H_VERSION_MAJOR           2
#define H_VERSION_MINOR           1

#define S_VERSION_MAJOR           3
#define S_VERSION_MINOR           2
#define S_SUBVERSION              0

#if H_VERSION_MAJOR > 1
#define NUM_LUX_SENSORS           2

// todo add logic to change these if needed...
#define NUM_LED 10
#define LED_PIN 5

////////////////////////////////////////////////////////////////////////////
//////////////////////////// Jumper Pins ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define JMP1_PIN 12
#define JMP2_PIN 11
#define JMP3_PIN 14
#define JMP4_PIN 15
#define JMP5_PIN 16
#define JMP6_PIN 17

////////////////////////////////////////////////////////////////////////////
////////////////////////// TCA Bus Expanders ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// I2C_MULTI should be 0 if no TCA I2C bus expander is present on the PCB
// I2C MULTI should be 1 if a TCA I2C bus expander is present
#define I2C_MULTI 1
// the number of active channels on the TCA (can in theory support 8 sensors, etc.)
#define TCA_CHANNELS 2
#endif // H_HARDWARE_VERSION
#endif // __HARDWARE_CONFIGURATION_H__
