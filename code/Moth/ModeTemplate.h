/*
 * 
 * This is a template file which contains a base for the creation of a new mode
 * 
 * It contains all of the expected files and functionality
 */

/*
#ifndef TEST_MODE_H
#define TEST_MODE_H

// if you are not using any of these libraries then remove from the include statements
#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include "Configuration_pitch.h"
#include "NeopixelManager/NeopixelManager.h"
#include "LuxManager/LuxManager.h"
#include "AudioEngine/AudioEngine.h"
#include <Audio.h>
#include <SerialFlash.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

///////////////// Globals /////////////////////////////////////
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);
NeoGroup neos[NUM_NEO_GROUPS] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 9, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_SENSORS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

// FeatureCollector fc[2] = {FeatureCollector("front"), FeatureCollector("rear")};

DLManager datalog_manager = DLManager((String)"Datalog Manager");

// AutoGain auto_gain[2] = {AutoGain("Song", &fc[0], &fc[1], MIN_SONG_GAIN, MAX_SONG_GAIN, MAX_GAIN_ADJUSTMENT),
//                          AutoGain("Onset", &fc[2], &fc[3], MIN_ONSET_GAIN, MAX_ONSET_GAIN, MAX_GAIN_ADJUSTMENT)
//                         };

///////////////////////////////////////////////////////////////////////////////////////////

///////////////// Audio code should appear here (created from the Teensy Audio Design Tool)

///////////////////////////////////////////////////////////////////////////////////////////

void setupAudio() {
  Serial.println("WARNING - setupAudio() is not implemented yet");
}

 void linkFeatureCollector() {
  Serial.println("WARNING - linkFeatureCollector() is not implemented yet");
 }

void setupDLManager() {
 Serial.println("WARNING - setupDLManager() is not implemented yet");
}

void updateMode() {
 Serial.println("WARNING - setupMode() is not implemented yet for TEST_MODE");
}

void updateAutogain() {
 Serial.println("WARNING - updateAutogain() is not implemented yet for TEST_MODE");
}

#endif
*/
