#ifndef __HARDWARE_CONFIGURATION_H__
#define __HARDWARE_CONFIGURATION_H__
// contains all the hardware specific variables
#define HARDWARE_NAME "Moth"

#define H_VERSION_MAJOR           2
#define H_VERSION_MINOR           1

#define S_VERSION_MAJOR           3
#define S_VERSION_MINOR           1
#define S_SUBVERSION              2

#define NUM_LUX_SENSORS           2

// todo add logic to change these if needed...
#define NUM_LED 10
#define LED_PIN 5

///////////////////////// Jumper Settings /////////////////////////////////
// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED 0

#endif // __HARDWARE_CONFIGURATION_H__
