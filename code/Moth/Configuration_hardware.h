#ifndef __CONFIGURATION_ADV_H__
#define __CONFIGURATION_ADV_H__
// contains all the hardware specific variables
#define HARDWARE_NAME "Moth"
#include "PrintUtils.h"
#include "Macros.h"

#define HV_MAJOR                  2
#define HV_MINOR                  1
// FIRMWARE MODE should be set to  CICADA_MODE, PITCH_MODE, or TEST_MODE
// depending on what functionality you want
#define FIRMWARE_MODE             PITCH_MODE

// TODO - need  to move some of this to the EEPROM storage, and add a flag in the standard configuratui file to  either read that information or to write it
// how long does the microphone test routine last for in the feature collector testMicrophone() function
#define MICROPHONE_TEST_DURATION  1500
#define LUX_TEST_DURATION         1000

////////////////////////////////////////////////////////////////////////////
////////////////////// Hardware Configurations /////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Currently there are three options for the enclosure : GROUND_ENCLOSURE, ORB_ENCLOSURE, and NO_ENCLOSURE
// different enclosures result in a different amount of attenuation from environmental sounds.
// the orb enclosure forms the base-line for this, it is thin and dones attneuate sounds but not nearly as much as the ground enclosure.
uint8_t ENCLOSURE_TYPE =          ORB_ENCLOSURE;

#if ENCLOSURE_TYPE == ORB_ENCLOSURE
#define ENC_ATTENUATION_FACTOR         1.0
#elif ENCLOSURE_TYPE == GROUND_ENCLOSURE
#define ENC_ATTENUATION_FACTOR         3.0
#elif ENCLOSURE_TYPE == NO_ENCLOSURE
#define ENC_ATTENUATION_FACTOR         0.75
#endif

// different microcontrollers which can be used for the system
#define TEENSY30                  0
#define TEENSY32                  1
#define TEENSY35                  2
#define TEENSY36                  3
#define TEENSY40                  4

#define MICROCONTROLLER           TEENSY32

#if HV_MAJOR > 1
#define NUM_LUX_SENSORS           2
#endif

// todo add logic to change these if needed...
#if (HV_MAJOR == 3)
#define NUM_LED                   40
#endif

#if (HV_MAJOR == 2 && HV_MINOR == 0)
#define NUM_LED                   12
#endif

#if (HV_MAJOR == 2 && HV_MINOR == 1)
#define NUM_LED                   10
#endif 

#define FRONT_MICROPHONE_INSTALLED      true
#define REAR_MICROPHONE_INSTALLED       true

////////////// TCA Bus Expanders     /////
// I2C_MULTI should be 0 if no TCA I2C bus expander is present on the PCB
// I2C MULTI should be 1 if a TCA I2C bus expander is present
#if HV_MAJOR < 3
#define I2C_MULTI                 1
// the number of active channels on the TCA (can in theory support 8 sensors, etc.)
#define TCA_CHANNELS              2
#else
#define I2C_MULTI                 0
#define TCA_CHANNELS              0 
#endif

//////////// MICROCONTROLLER PIN OUTS ////
#define LED_PIN                   5

//////////// User Controls /////////////////
#if HV_MAJOR < 3
#define NUM_JUMPERS               6
#define JMP1_PIN                  12
#define JMP2_PIN                  11
#define JMP3_PIN                  14
#define JMP4_PIN                  15
#define JMP5_PIN                  16
#define JMP6_PIN                  17
#define NUM_POTS                  0
int jmp_pins[NUM_JUMPERS] = {JMP1_PIN, JMP2_PIN, JMP3_PIN, JMP4_PIN, JMP5_PIN,
                             JMP6_PIN};

#elif HV_MAJOR == 3
#define NUM_JUMPERS               10

#define JMP1_PIN                  14
#define JMP2_PIN                  12
#define JMP3_PIN                  11
#define JMP4_PIN                  10
#define JMP5_PIN                  8
#define JMP6_PIN                  7
#define JMP7_PIN                  6
#define JMP8_PIN                  4
#define JMP9_PIN                  3
#define JMP10_PIN                 2

int jmp_pins[NUM_JUMPERS] = {JMP1_PIN, JMP2_PIN, JMP3_PIN, JMP4_PIN, JMP5_PIN,
                             JMP6_PIN, JMP7_PIN, JMP8_PIN, JMP9_PIN, JMP10_PIN};

#define NUM_POTS                  4
#define POT1_PIN                  22
#define POT2_PIN                  21
#define POT3_PIN                  20
#define POT4_PIN                  17

int pot_pins[NUM_POTS] = {POT1_PIN, POT2_PIN, POT3_PIN, POT4_PIN};
uint16_t pot_vals[NUM_POTS] = {0, 0, 0, 0};

#endif

////////////////////////////////////////////////////////////////////////////
///////////////////////// Jumper Settings //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED               1

#endif // __HARDWARE_CONFIGURATION_H__
