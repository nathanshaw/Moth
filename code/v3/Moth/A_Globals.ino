//////////////////////////// lux and stuff /////////////////////////

elapsedMillis lux_reading_timer[2];

// #include "MothConfig.h"


// extern "C" {
// #include "utility/twi.h" // from Wire library, so we can do bus scanning
// }


// front / rear and combined
double lux[3];

double min_lux_reading[3] = {9999.9, 9999.9, 9999.9};
double max_lux_reading[3] = {0.0, 0.0, 0.0};

/////////////////////////////// NeoP ////////////////////////////////////
#define NUM_LED 10
#define LED_PIN 5

byte drawingMemory[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LED * 12]; // 12 bytes per LED

/*void colorWipe(uint8_t, uint8_t, uint8_t, uint8_t mode = 2);

uint32_t packColors(uint8_t red, uint8_t green, uint8_t blue, double scaler);

void updateOnOffRatios(uint32_t color, uint8_t i);

void resetOnOffRatioCounters();*/
#include <WS2812Serial.h>
WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);
NeoGroup neos[2] = {
  NeoGroup(&leds, 0, 5, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 6, 12, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// audio usage loggings
uint8_t audio_usage_max = 0;
elapsedMillis last_usage_print = 0;// for keeping track of audio memory usage
#define AUDIO_MEMORY 24

elapsedMillis last_runtime_update;
elapsedMillis ten_second_timer;

void tenSecondUpdate() {
  if (ten_second_timer > TEN_SECONDS) {
    checkAudioUsage();
#if (AUTO_GAIN)
    autoGainAdjust(); // will call rear as well if in stereo mode
#endif
    updateEEPROMLogs();
    updateRuntimeAndClicks();
    ten_second_timer = 0;
  }
}

void checkAudioUsage() {
  // TODO instead perhaps log the audio usage...
  if (last_usage_print > AUDIO_USAGE_POLL_RATE) {
    uint8_t use = AudioMemoryUsageMax();
    if (use > audio_usage_max) {
      audio_usage_max = use;
      writeAudioUsageToEEPROM(use);
      Serial.print("memory usage: ");
      Serial.print(use);
      Serial.print(" out of ");
      Serial.println(AUDIO_MEMORY);
    }
    last_usage_print = 0;
  }
}
