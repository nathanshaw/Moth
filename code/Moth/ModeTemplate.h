/*
 * 
 * This is a template file which contains a base for the creation of a new mode
 * 
 * It contains all of the expected files and functionality
 */


///////////////// Globals /////////////////////////////////////
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);
NeoGroup neos[NUM_NEO_GROUPS] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 10, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_SENSORS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

FeatureCollector fc[2] = {FeatureCollector("front"), FeatureCollector("rear")};

RGBConverter converter;


DLManager datalog_manager = DLManager((String)"Datalog Manager");

AutoGain auto_gain[2] = {AutoGain("Song", &fc[0], &fc[1], MIN_SONG_GAIN, MAX_SONG_GAIN, MAX_GAIN_ADJUSTMENT),
                         AutoGain("Click", &fc[2], &fc[3], MIN_CLICK_GAIN, MAX_CLICK_GAIN, MAX_GAIN_ADJUSTMENT)
                        };

/*
 * Audio code should appear here (created from the Teensy Audio Design Tool)
 */
/*
 void linkFeatureCollector() {
  Serial.println("WARNING - linkFeatureCollector() is not implemented yet");
 }

void mainLoop() {
   Serial.println("WARNING - mainLoop() is not implemented yet");
}

void mainSetup() {
 Serial.println("WARNING - mainSetup() is not implemented yet");
}
*/
