#ifndef __CONFIGURATION_ADV_H__
#define __CONFIGURATION_ADV_H__
// contains all the hardware specific variables
#define HARDWARE_NAME "Moth"
#include "PrintUtils.h"

// how long does the microphone test routine last for in the feature collector testMicrophone() function
#define MICROPHONE_TEST_DURATION  2000

////////////////////////////////////////////////////////////////////////////
////////////////////// Hardware Configurations /////////////////////////////
////////////////////////////////////////////////////////////////////////////

// different microcontrollers which can be used for the system
#define TEENSY30                  0
#define TEENSY32                  1
#define TEENSY35                  2
#define TEENSY36                  3
#define TEENSY40                  4

#define MICROCONTROLLER           TEENSY32
#define H_VERSION_MAJOR           2
#define H_VERSION_MINOR           1

#if H_VERSION_MAJOR > 1
#define NUM_LUX_SENSORS           2
#endif

// todo add logic to change these if needed...
#if (H_VERSION_MAJOR == 2 && H_VERSION_MINOR == 0)
#define NUM_LED                   12   
#endif
#if (H_VERSION_MAJOR == 2 && H_VERSION_MINOR == 1)
#define NUM_LED                   10
#endif 

////////////// TCA Bus Expanders     /////
// I2C_MULTI should be 0 if no TCA I2C bus expander is present on the PCB
// I2C MULTI should be 1 if a TCA I2C bus expander is present
#define I2C_MULTI 1
// the number of active channels on the TCA (can in theory support 8 sensors, etc.)
#define TCA_CHANNELS 2

//////////// MICROCONTROLLER PIN OUTS ////
#define LED_PIN 3

//////////// Jumper Pins /////////////////
#define JMP1_PIN 12
#define JMP2_PIN 11
#define JMP3_PIN 14
#define JMP4_PIN 15
#define JMP5_PIN 16
#define JMP6_PIN 17

////////////////////////////////////////////////////////////////////////////
////////////////////// Software Configurations /////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define S_VERSION_MAJOR           0
#define S_VERSION_MINOR           0
#define S_SUBVERSION              0

////////////////////////////////////////////////////////////////////////////
////////////////////// Neopixel Managers  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// how high the click flash timer will go up to
#define MAX_FLASH_TIME        300
// where the click flash timer will start
#define MIN_FLASH_TIME        100
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define FLASH_DEBOUNCE_TIME 400

///////////////////////////////// General Purpose Functions //////////////////////////////////
#define SERIAL_BAUD_RATE          57600

void readJumpers(bool &v1, bool &v2, bool &v3, bool &v4, bool &v5, bool &v6) {
  printMajorDivide("reading jumpers");
  pinMode(JMP1_PIN, INPUT);
  pinMode(JMP2_PIN, INPUT);
  pinMode(JMP3_PIN, INPUT);
  pinMode(JMP4_PIN, INPUT);
  pinMode(JMP5_PIN, INPUT);
  pinMode(JMP6_PIN, INPUT);
  delay(100);
  v1 = digitalRead(JMP1_PIN);
  Serial.print(v1); printTab();
  v2 = digitalRead(JMP2_PIN);
  Serial.print(v2); printTab();
  v3 = digitalRead(JMP3_PIN);
  Serial.print(v3); printTab();
  v4 = digitalRead(JMP4_PIN);
  Serial.print(v4); printTab();
  v5 = digitalRead(JMP5_PIN);
  Serial.print(v5); printTab();
  v6 = digitalRead(JMP6_PIN);
  Serial.print(v6); printTab();
  printDivide();
}

#endif // __HARDWARE_CONFIGURATION_H__
