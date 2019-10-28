/*
   Test firmware for the Moth bot
*/
#include "Adafruit_VEML7700.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <elapsedMillis.h>
#include <WS2812Serial.h>

extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus scanning
}
///////////////////////// Debuggings ////////////////////////////////////
#define PRINT_LUX_READINGS true
// TODO
#define PRINT_SONG_FEATURES false
// TODO
#define PRINT_CLICK_FEATURES false
// TODO
#define PRINT_LED_VALUES false

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////

// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS true

// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED 0

// turns on/off debug printing
#define DEBUG 0

// data logging related
#define AUDIO_USAGE_POLL_RATE 200000

// Neo Pixels
#define MAX_BRIGHTNESS 250
#define MIN_BRIGHTNESS 5

// lux sensors
#define FRONT_LUX_INSTALLED true
#define REAR_LUX_INSTALLED true

unsigned long lux_max_reading_delay = 1000 * 60 * 5; // every 6 minutes
unsigned long lux_min_reading_delay = 1000 * 60 * 1; // 1 minute

#define SMOOTH_LUX_READINGS true

#define MIN_LUX_EXPECTED 10.0
#define MAX_LUX_EXPECTED 400.0
// on scale of 1-1000 what is the min multiplier for lux sensor brightness adjustment
// 500 would equate to a dimming of 50%
#define BRIGHTNESS_SCALER_MIN 500

// Audio
#define USB_OUTPUT 1
#define MAX_GAIN_ADJUSTMENT 0.10

const uint32_t auto_gain_frequency = 1000 * 60 * 0.5; // how often to calculate auto-gain (in ms)

///////// Click Settings
// what feature will be used to determine if a click has been found
// "rms_delta" will use that feature along with CLICK_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with CLICK_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
#define CLICK_FEATURE "rms_delta"
#define CLICK_RMS_DELTA_THRESH 0.03
#define CLICK_PEAK_DELTA_THRESH 0.03

// One click per ten minutes
#define MIN_CLICKS_PER_MINUTE 0.5
#define MAX_CLICKS_PER_MINUTE 40.0

#define STARTING_CLICK_GAIN 8.0

#define MIN_CLICK_GAIN 0.5
#define MAX_CLICK_GAIN 24.0

#define MIN_SONG_GAIN 0.5
#define MAX_SONG_GAIN 20

#define CLICK_BQ1_THRESH 1200
#define CLICK_BQ1_Q 0.95
#define CLICK_BQ1_DB -24
#define CLICK_BQ2_THRESH 2500
#define CLICK_BQ2_Q 0.95
#define CLICK_BQ2_DB -24

// how high the click flash timer will go up to
#define MAX_CLICK_LENGTH 40
// where the click flash timer wil
#define MIN_CLICK_LENGTH 5

//////// Song Settings

// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
#define SONG_FEATURE "peak"

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.005
#define MAX_SONG_PEAK_AVG 0.20

#define STARTING_SONG_GAIN 8.0

#define SONG_BQ1_THRESH 13500
#define SONG_BQ1_Q 0.75
#define SONG_BQ1_DB -12
#define SONG_BQ2_THRESH 14000
#define SONG_BQ2_Q 0.75
#define SONG_BQ2_DB -12

////////////////////////////////////////////////////////////////////////////
//////////////////////// Hardware Controls /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// cicada (-) or bird song (+) mode
#define JMP1_PIN 12
bool cicada_mode = true;

// mono audio (-) or stereo (+)
#define JMP2_PIN 11
bool stereo_audio = true;

// one front lux (-) or two lux (+)
#define JMP3_PIN 14
bool both_lux_sensors = true;

// independent lux readings (-) or average/combine lux readings (+)
#define JMP4_PIN 15
bool combine_lux_readings = true;

// auto gain adjust (-) or not (+)
#define JMP5_PIN 16
bool gain_adjust_active = true;

// data log not active (-) or active (+)
#define JMP6_PIN 17
bool data_logging_active = false;
#include <EEPROM.h>

// this is the max storage that the Teensy Has is 2048
#define MAX_ADDR 2000
// alternatively we can use EEPROM.length() to give us the max length that the microcontroller has
// EEPROM.write(addr, val) will write a value to the given location

/*  Below is a map of what will be stored in the data logger
   /////////////////////////////////////////////////////////
   General Configuration Settings 0 - 24
   0-5 the jumper configurations

   Runtime Values 50 - 99
   //////////////
   50 max_memory usage
   51 the total audio memory allocated
   60 front_lux min reading
   61 rear_lux min reading
   62 front_lux_max_reading
   63 rear_lux_max_reading

   Lux Sensor Readings 100 - 200

   Click Readings

   Song Readings

*/

// note that each address can store a single byte (value from 0 - 255)

unsigned int addr = 0;

// EEPROM MEMORY ADDRESSES
#define EEPROM_JMP1 0
#define EEPROM_JMP2 1
#define EEPROM_JMP3 2
#define EEPROM_JMP4 3
#define EEPROM_JMP5 4
#define EEPROM_JMP6 5
#define EEPROM_AUDIO_MEM_USAGE 50
#define EEPROM_AUDIO_MEM_MAX 51

// lux
#define EEPROM_MIN_LUX_READING_FRONT 60
#define EEPROM_MIN_LUX_READING_REAR 62
#define EEPROM_MIN_LUX_READING_COMBINED 64
#define EEPROM_MAX_LUX_READING_FRONT 66
#define EEPROM_MAX_LUX_READING_REAR 68
#define EEPROM_MAX_LUX_READING_COMBINED 70

// audio
#define EEPROM_CLICK_GAIN_MIN_FRONT 200
#define EEPROM_CLICK_GAIN_MAX_FRONT 202
#define EEPROM_CLICK_GAIN_MIN_REAR 204
#define EEPROM_CLICK_GAIN_MAX_REAR 206

#define EEPROM_SONG_GAIN_MIN_FRONT 208
#define EEPROM_SONG_GAIN_MAX_FRONT 210
#define EEPROM_SONG_GAIN_MIN_REAR 212
#define EEPROM_SONG_GAIN_MAX_REAR 214

// test addresses
#define EEPROM_TEST1 1900
#define EEPROM_TEST2 1902

/////////////////////////////// NeoP ////////////////////////////////////
#define NUM_LED 10
#define LED_PIN 5

#define RED    0xff0000
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0xf0f0f0

// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define LED_SHDN_LEN 40

byte drawingMemory[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LED * 12]; // 12 bytes per LED

int16_t flash_delay = 0;           // to keep track of the flash
bool flash_on = false;              // to keep track of the flash
elapsedMillis last_click_flash = 0; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
elapsedMillis led_shdn_timer = 0;
bool leds_on = false;               // set to false if the LEDs are turned off and true if the leds are on...
elapsedMillis led_off_len;          // keeps track of how long the LEDs are turned off, it will reset when colorWipe is called with a color of 0

int16_t flash_delay_r = 0;           // to keep track of the flash
bool flash_on_r = false;              // to keep track of the flash
elapsedMillis last_click_flash_r = 0; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
elapsedMillis led_shdn_timer_r = 0;
bool leds_on_r = false;               // set to false if the LEDs are turned off and true if the leds are on...
elapsedMillis led_off_len_r;          // keeps track of how long the LEDs are turned off, it will reset when colorWipe is called with a color of 0


WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// LUX ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

elapsedMillis lux_reading_timer = 0;
// TODO perhaps declare in a global setting somehow through init function thrown by flag
elapsedMillis lux_reading_timer_r = 0;

double brightness_scaler_front    = 1.0;
double brightness_scaler_rear     = 1.0;
double brightness_scaler_combined = 1.0;

double front_lux;
double rear_lux;
double combined_lux;

uint16_t min_lux_reading = 999999;
uint16_t min_lux_reading_r = 999999;
uint16_t min_lux_reading_c = 999999;
uint16_t max_lux_reading = 0;
uint16_t max_lux_reading_r = 0;
uint16_t max_lux_reading_c = 0;


Adafruit_VEML7700 veml[] = Adafruit_VEML7700();
Adafruit_VEML7700 veml_rear = Adafruit_VEML7700();

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Audio Library /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_MEMORY 24

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=55,122.00000190734863
AudioAmplifier           click_input_amp;           //xy=216,116.00000190734863
AudioAmplifier           song_input_amp;           //xy=216.5999984741211,147.00000190734863
AudioAnalyzeRMS          rms_input;           //xy=230.00000381469727,258.99999046325684
AudioAnalyzePeak         peak_input;          //xy=234,290.99999141693115
AudioFilterBiquad        click_biquad;        //xy=381.00000762939453,116.00000190734863
AudioFilterBiquad        song_biquad;        //xy=381.00000762939453,146.00000190734863
AudioAmplifier           click_mid_amp;           //xy=464,50
AudioAmplifier           song_mid_amp;           //xy=470.00000762939453,204.00000286102295
AudioFilterBiquad        click_biquad2;        //xy=568.0000228881836,116.00000190734863
AudioFilterBiquad        song_biquad2;        //xy=570.0000076293945,147.00000190734863
AudioAmplifier           song_post_amp;           //xy=668.0000076293945,204.00000381469727
AudioAmplifier           click_post_amp;           //xy=677.2000198364258,50
AudioAnalyzeRMS          song_rms;           //xy=862.0000152587891,201.00000286102295
AudioAnalyzeRMS          click_rms;           //xy=865.200023651123,28.000003814697266
AudioAnalyzePeak         song_peak;          //xy=866.0000152587891,233.00000381469727
AudioAnalyzePeak         click_peak;          //xy=869.2000198364258,60.000003814697266

AudioConnection          patchCord1(i2s1, 0, rms_input, 0);
AudioConnection          patchCord2(i2s1, 0, peak_input, 0);
AudioConnection          patchCord3(i2s1, 0, click_input_amp, 0);
AudioConnection          patchCord4(i2s1, 0, song_input_amp, 0);
AudioConnection          patchCord5(click_input_amp, click_biquad);
AudioConnection          patchCord6(song_input_amp, song_biquad);
AudioConnection          patchCord7(click_biquad, click_mid_amp);
AudioConnection          patchCord8(song_biquad, song_mid_amp);
AudioConnection          patchCord9(click_mid_amp, click_biquad2);
AudioConnection          patchCord10(song_mid_amp, song_biquad2);
AudioConnection          patchCord11(click_biquad2, click_post_amp);
AudioConnection          patchCord12(song_biquad2, song_post_amp);
AudioConnection          patchCord14(song_post_amp, song_rms);
AudioConnection          patchCord15(song_post_amp, song_peak);
AudioConnection          patchCord16(click_post_amp, click_rms);
AudioConnection          patchCord17(click_post_amp, click_peak);

#ifdef USB_OUTPUT
AudioOutputUSB           usb1;           //xy=1024.000015258789,129.00000190734863
AudioConnection          patchCord13(song_post_amp, 0, usb1, 1);
AudioConnection          patchCord18(click_post_amp, 0, usb1, 0);
#endif

// for now just going to keep the
AudioAmplifier           click_input_amp_r;           //xy=216,116.00000190734863
AudioAmplifier           song_input_amp_r;           //xy=216.5999984741211,147.00000190734863
AudioAnalyzeRMS          rms_input_r;           //xy=230.00000381469727,258.99999046325684
AudioAnalyzePeak         peak_input_r;          //xy=234,290.99999141693115
AudioFilterBiquad        click_biquad_r;        //xy=381.00000762939453,116.00000190734863
AudioFilterBiquad        song_biquad_r;        //xy=381.00000762939453,146.00000190734863
AudioAmplifier           click_mid_amp_r;           //xy=464,50
AudioAmplifier           song_mid_amp_r;           //xy=470.00000762939453,204.00000286102295
AudioFilterBiquad        click_biquad2_r;        //xy=568.0000228881836,116.00000190734863
AudioFilterBiquad        song_biquad2_r;        //xy=570.0000076293945,147.00000190734863
AudioAmplifier           song_post_amp_r;           //xy=668.0000076293945,204.00000381469727
AudioAmplifier           click_post_amp_r;           //xy=677.2000198364258,50
AudioAnalyzeRMS          song_rms_r;           //xy=862.0000152587891,201.00000286102295
AudioAnalyzeRMS          click_rms_r;           //xy=865.200023651123,28.000003814697266
AudioAnalyzePeak         song_peak_r;          //xy=866.0000152587891,233.00000381469727
AudioAnalyzePeak         click_peak_r;          //xy=869.2000198364258,60.000003814697266

AudioConnection          patchCord1_r(i2s1, 0, rms_input_r, 0);
AudioConnection          patchCord2_r(i2s1, 0, peak_input_r, 0);
AudioConnection          patchCord3_r(i2s1, 0, click_input_amp_r, 0);
AudioConnection          patchCord4_r(i2s1, 0, song_input_amp_r, 0);
AudioConnection          patchCord5_r(click_input_amp_r, click_biquad_r);
AudioConnection          patchCord6_r(song_input_amp_r, song_biquad_r);
AudioConnection          patchCord7_r(click_biquad_r, click_mid_amp_r);
AudioConnection          patchCord8_r(song_biquad_r, song_mid_amp_r);
AudioConnection          patchCord9_r(click_mid_amp_r, click_biquad2_r);
AudioConnection          patchCord10_r(song_mid_amp_r, song_biquad2_r);
AudioConnection          patchCord11_r(click_biquad2_r, click_post_amp_r);
AudioConnection          patchCord12_r(song_biquad2_r, song_post_amp_r);
AudioConnection          patchCord14_r(song_post_amp_r, song_rms_r);
AudioConnection          patchCord15_r(song_post_amp_r, song_peak_r);
AudioConnection          patchCord16_r(click_post_amp_r, click_rms_r);
AudioConnection          patchCord17_r(click_post_amp_r, click_peak_r);

// to keep track of when to write audio_usage to EEPROM
uint8_t audio_usage_max = 0;

// TODO make it so the routing is different if there is only one channel
double click_gain = STARTING_CLICK_GAIN; // starting click gain level
double click_gain_low = 100.0;
double click_gain_high = 0.0;

double click_rms_val = 0.0;
double last_click_rms_val = 0.0;
double click_rms_delta = 0.0;

double click_peak_val = 0.0;
double last_click_peak_val = 0.0;
double click_peak_delta = 0.0;

double song_gain = STARTING_SONG_GAIN;   // starting song gain level
uint8_t song_rms_weighted = 0;  // 0 -255 depending on the RMS of the song band...
uint8_t song_peak_weighted = 0; // 0 -255 depending on the peak of the song band...

// for the rear
double click_gain_r = STARTING_CLICK_GAIN; // starting click gain level
double click_gain_low_r = 100.0;
double click_gain_high_r = 0.0;

double click_rms_val_r = 0.0;
double last_click_rms_val_r = 0.0;
double click_rms_delta_r = 0.0;

double click_peak_val_r = 0.0;
double last_click_peak_val_r = 0.0;
double click_peak_delta_r = 0.0;

double song_gain_r = STARTING_SONG_GAIN;   // starting song gain level
uint8_t song_rms_weighted_r = 0;  // 0 -255 depending on the RMS of the song band...
uint8_t song_peak_weighted_r = 0; // 0 -255 depending on the peak of the song band...

// for keeping track of audio memory usage
elapsedMillis last_usage_print = 0;

///////////////////////////////////////////////////////////////////////
/////////////// Auto-Gain Variables ///////////////////////////////////
///////////////////////////////////////////////////////////////////////
double total_song_peaks;
unsigned long num_song_peaks;
uint16_t num_past_clicks; // number of clicks which has occurred since boot

double total_song_peaks_r;
unsigned long num_song_peaks_r;
uint16_t num_past_clicks_r; // number of clicks which has occurred since boot

elapsedMillis last_auto_gain_adjustment; // the time in which the last auto_gain_was_calculated
// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.

///////////////////////////////////////////////////////////////////////
//                       TCA9532A I2C bus expander
///////////////////////////////////////////////////////////////////////
// TODO add code so if the TCA is not available things are cool... also add firmware #define to control this
#define TCAADDR 0x70
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setupVEMLthroughTCA() {
  Wire.begin();
  tcaselect(0);
  if (!veml_front.begin()) {
    Serial.println("VEML Front not found");
    while (1);
  }
  Serial.println("VEML Front found");
  veml_front.setGain(VEML7700_GAIN_1);
  veml_front.setIntegrationTime(VEML7700_IT_25MS);// 800ms was default

  if (both_lux_sensors) {
    tcaselect(1);
    if (!veml_rear.begin()) {
      Serial.println("VEML Rear not found");
      while (1);
    }
    Serial.println("VEML Rear found");
    veml_rear.setGain(VEML7700_GAIN_1);
    veml_rear.setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
  }
}

///////////////////////////////////////////////////////////////////////
//                    Neopixel Related Functions
///////////////////////////////////////////////////////////////////////

void colorWipe(uint8_t red, uint8_t green, uint8_t blue) {
  int color = scaleBrightness(red, green, blue, brightness_scaler_combined);
  if (led_shdn_timer < LED_SHDN_LEN) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    return;
  }
  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
  }
  leds.show();
  // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
  // also reset led_off_len if the leds  were just turned off
  if (color == 0) {
    if (leds_on == true) {
      led_off_len = 0;
    }
    leds_on = false;
  } else {
    leds_on = true;
  }
}

void colorWipeFront(uint8_t red, uint8_t green, uint8_t blue) {
  // TODO needs to be fully implemented
  int color = scaleBrightness(red, green, blue, brightness_scaler_front);
  if (led_shdn_timer < LED_SHDN_LEN) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    return;
  }
  for (int i = 0; i < leds.numPixels() * 0.5; i++) {
    leds.setPixel(i, color);
  }
  leds.show();
  // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
  // also reset led_off_len if the leds  were just turned off
  if (color == 0) {
    if (leds_on == true) {
      led_off_len = 0;
    }
    leds_on = false;
  } else {
    leds_on = true;
  }
}

void colorWipeRear(uint8_t red, uint8_t green, uint8_t blue) {
  // TODO needs to be fully implemented
  int color = scaleBrightness(red, green, blue, brightness_scaler_rear);
  if (led_shdn_timer < LED_SHDN_LEN) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    return;
  }
  for (int i = (leds.numPixels() * 0.5) - 1; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
  }
  leds.show();
  // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
  // also reset led_off_len if the leds  were just turned off
  if (color == 0) {
    if (leds_on == true) {
      led_off_len = 0;
    }
    leds_on = false;
  } else {
    leds_on = true;
  }
}

uint32_t scaleBrightness(uint8_t red, uint8_t green, uint8_t blue, double scaler) {
  // TODO change code to account for front and back sides
  uint32_t color = 0;
  red = red * scaler;
  green = green * scaler;
  blue = blue * scaler;
  color = (red << 16) + (green << 8) + (blue);
  return color;
}

uint8_t checkLuxSensors() {
  // Return a 1 if the lux sensors are read and a 0 if they are not
  // if the LEDs have been off, on their own regard, for 40ms or longer...
  // and it has been long-enough to warrent a new reading
  bool readings = false;

  if (leds_on == false && led_off_len >= LED_SHDN_LEN && lux_reading_timer > lux_min_reading_delay) {
    _readFrontLuxSensor();
    readings = true;
  }
  if (both_lux_sensors == true) {
    if (leds_on == false && led_off_len >= LED_SHDN_LEN && lux_reading_timer_r > lux_min_reading_delay) {
      _readRearLuxSensor();
      readings = true;
    }
  }
  if (readings) {
    return 1;
  };

  // if it has been longer than the "lux_max_reading_delay" force a new reading...
  // TODO update this code to handle reading on each side if both_lux_sensors active
  if (both_lux_sensors == true) {
    if (lux_reading_timer > lux_max_reading_delay && led_shdn_timer > LED_SHDN_LEN) {
      // turn the LEDs off
      colorWipeFront(0, 0, 0);
      //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
      led_shdn_timer = 0;
    }
    if (lux_reading_timer_r > lux_max_reading_delay && led_shdn_timer_r > LED_SHDN_LEN) {
      // turn the LEDs off
      colorWipeRear(0, 0, 0);
      //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
      led_shdn_timer_r = 0;
    }
  }
  else {
    if (lux_reading_timer > lux_max_reading_delay && led_shdn_timer > LED_SHDN_LEN) {
      // turn the LEDs off
      colorWipe(0, 0, 0);
      //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
      led_shdn_timer = 0;
    }
  }
  return 0;
}

void updateLuxMinMax() {
  // give the program some time to settle
  if (data_logging_active && millis() > 20000) {
    // front
    if ((int)front_lux > max_lux_reading) {
      max_lux_reading = front_lux;
      writeTwoBytesToEEPROM(EEPROM_MAX_LUX_READING_FRONT, max_lux_reading);
      Serial.print("logged new front max_lux_reading to EEPROM\t"); Serial.println(max_lux_reading);
    } else if ((int)front_lux < min_lux_reading && front_lux != 0) {
      min_lux_reading = front_lux;
      writeTwoBytesToEEPROM(EEPROM_MIN_LUX_READING_FRONT , min_lux_reading);
      Serial.print("logged new front min_lux_reading to EEPROM\t"); Serial.println(min_lux_reading);
    }
    // rear
    if ((int)rear_lux > max_lux_reading_r) {
      max_lux_reading_r = rear_lux;
      writeTwoBytesToEEPROM(EEPROM_MAX_LUX_READING_REAR, max_lux_reading_r);
      Serial.print("logged new rear max_lux_reading to EEPROM\t"); Serial.println(max_lux_reading_r);
    } else if ((int)rear_lux < min_lux_reading_r && rear_lux != 0) {
      min_lux_reading_r = rear_lux;
      writeTwoBytesToEEPROM(EEPROM_MIN_LUX_READING_REAR , min_lux_reading_r);
      Serial.print("logged new rear min_lux_reading to EEPROM\t"); Serial.println(min_lux_reading_r);
    }
    // combined
    if ((int)combined_lux > max_lux_reading_c) {
      max_lux_reading_c = combined_lux;
      writeTwoBytesToEEPROM(EEPROM_MAX_LUX_READING_COMBINED, max_lux_reading_c);
      Serial.print("logged new combined max_lux_reading to EEPROM\t"); Serial.println(max_lux_reading_c);
    } else if ((int)combined_lux < min_lux_reading_c) {
      min_lux_reading_c = combined_lux;
      writeTwoBytesToEEPROM(EEPROM_MIN_LUX_READING_COMBINED , min_lux_reading_c);
      Serial.print("logged new combined min_lux_reading to EEPROM\t"); Serial.println(max_lux_reading_c);
    }
  }
}

void _readRearLuxSensor() {
  tcaselect(1);
  if (SMOOTH_LUX_READINGS && rear_lux != 0) {
    rear_lux = (rear_lux + veml_rear.readLux()) * 0.5;
  } else {
    rear_lux = veml_rear.readLux();
  }
  if (rear_lux > 10000000) {
    Serial.print("rear lux reading error: ");
    Serial.println(rear_lux);
    if (SMOOTH_LUX_READINGS) {
      rear_lux = (rear_lux + veml_rear.readLux()) * 0.5;
    } else {
      rear_lux = veml_rear.readLux();
    }

  }
  combined_lux = (front_lux + rear_lux) * 0.5;
  brightness_scaler_rear = map(constrain(rear_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  brightness_scaler_combined = map(constrain(combined_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  updateLuxMinMax();
  lux_reading_timer_r = 0;
}

void _readFrontLuxSensor() {
  tcaselect(0);
  if (SMOOTH_LUX_READINGS && front_lux != 0) {
    front_lux = (front_lux + veml_front.readLux()) * 0.5;
  } else {
    front_lux = veml_front.readLux();
  }
  if (front_lux > 10000000) {
    Serial.print("front lux reading error: ");
    Serial.println(front_lux);
    if (SMOOTH_LUX_READINGS) {
      front_lux = (front_lux + veml_front.readLux()) * 0.5;
    } else {
      front_lux = veml_front.readLux();
    }
  }
  combined_lux = (front_lux + rear_lux) * 0.5;
  // TODO souble check that all these numbers should be hard coded, same for the rear below
  brightness_scaler_front = map(constrain(front_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  brightness_scaler_combined = map(constrain(combined_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  updateLuxMinMax();
  lux_reading_timer = 0;
}

void _readLuxSensors() {
  tcaselect(0);
  if (SMOOTH_LUX_READINGS && front_lux != 0) {
    front_lux = (front_lux + veml_front.readLux()) * 0.5;
  } else {
    front_lux = veml_front.readLux();
  }
  if (front_lux > 10000000) {
    Serial.print("front lux reading error: ");
    Serial.println(front_lux);
    if (SMOOTH_LUX_READINGS && front_lux != 0) {
      front_lux = (front_lux + veml_front.readLux()) * 0.5;
    } else {
      front_lux = veml_front.readLux();
    }
  }
  // TODO souble check that all these numbers should be hard coded, same for the rear below
  brightness_scaler_front = map(constrain(front_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;

  // if there is also a rear lux sensor
  if (both_lux_sensors) {
    tcaselect(1);
    if (SMOOTH_LUX_READINGS && rear_lux != 0) {
      rear_lux = (rear_lux + veml_rear.readLux()) * 0.5;
    } else {
      rear_lux = veml_rear.readLux();
    }
    if (rear_lux > 10000000) {
      Serial.print("rear lux reading error: ");
      Serial.println(rear_lux);
      if (SMOOTH_LUX_READINGS && rear_lux != 0) {
        rear_lux = (rear_lux + veml_rear.readLux()) * 0.5;
      } else {
        rear_lux = veml_rear.readLux();
      }
    }
    combined_lux = (front_lux + rear_lux) * 0.5;
    brightness_scaler_rear = map(constrain(rear_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
    brightness_scaler_combined = map(constrain(combined_lux, MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  } else {
    combined_lux = front_lux;
    brightness_scaler_combined = brightness_scaler_front;
  }
  // reset polling timer
  updateLuxMinMax();
  lux_reading_timer = 0;
}

void printLuxReadings() {
  if (PRINT_LUX_READINGS) {
    Serial.print("Lux: "); Serial.print(combined_lux); Serial.print(" = ");
    Serial.print(front_lux); Serial.print(" ("); Serial.print(brightness_scaler_front); Serial.print(")");
    Serial.print(" + "); Serial.print(rear_lux); Serial.print(" ("); Serial.print(brightness_scaler_rear); Serial.println(")");
  }
}

void clickFlashes() {
  // check to see if light is already on
  if (flash_delay > 0) { // if there is time remaining in the flash
    Serial.print("flash delay : ");
    Serial.println(flash_delay);
    if (flash_on == false) { //and the light is not currently on
      flash_on = true; // turn the light on along with the flag
      // TODO try scaling the click brightness according to peak or rms instead of hard value? (user flag descides)
      if (stereo_audio) {
        colorWipeFront(0, 0, 255);
      } else {
        colorWipe(0, 0, 255);
      }
      last_click_flash = 0; // reset the elapsed millis variable as the light was just turned on
    } else {
      // if the light is already on
      // subtract the number of ms which have gone by since the last check
      // TODO optimize below lines
      flash_delay = max(flash_delay - last_click_flash, 0);
      last_click_flash = 0;
      if (flash_delay == 0) {
        // if there is no more time left then turn off the light and turn flag to false
        flash_on = false;
        colorWipe(0, 0, 0);
        Serial.println("Turned front flash off");
      }
    }
  } else {
    if (flash_on == true) {
      // if for some reason the flash is still on turn it off
      flash_on = false;
      if (stereo_audio) {
        colorWipeFront(0, 0, 0);
        Serial.println("Turned front flash off");
      } else {
        colorWipe(0, 0, 0);
        Serial.println("Turned combined flash off");
      }
    }
    last_click_flash = 0;
  }
  if (stereo_audio) {
    clickFlashesRear();
  }
}

void clickFlashesRear() {
  // check to see if light is already on
  if (flash_delay_r > 0) { // if there is time remaining in the flash
    Serial.print("flash delay_r : ");
    Serial.println(flash_delay_r);
    if (flash_on_r == false) { //and the light is not currently on
      flash_on_r = true; // turn the light on along with the flag
      // TODO try scaling the click brightness according to peak or rms instead of hard value? (user flag descides)
      colorWipeRear(0, 0, 255);
      last_click_flash_r = 0; // reset the elapsed millis variable as the light was just turned on
    } else {
      // if the light is already on
      // subtract the number of ms which have gone by since the last check
      // TODO optimize below lines
      flash_delay_r = max(flash_delay_r - last_click_flash_r, 0);
      last_click_flash_r = 0;
      if (flash_delay_r == 0) {
        // if there is no more time left then turn off the light and turn flag to false
        flash_on_r = false;
        colorWipeRear(0, 0, 0);
        Serial.println("Turned rear flash off");
      }
    }
  } else {
    if (flash_on_r == true) {
      flash_on_r = false;
      colorWipeRear(0, 0, 0);
      Serial.println("Turned rear flash off");
    }
    last_click_flash_r = 0;
  }
}

///////////////////////////////////////////////////////////////////////
//                    Click Audio Functions
///////////////////////////////////////////////////////////////////////
void printClickStats() {
  // TODO update this to have a stereo option
  Serial.print("1000* Click | rms: ");
  Serial.print(click_rms_val * 1000);
  Serial.print(" delta: ");
  Serial.print(click_rms_delta * 1000);
  Serial.print(" peak: ");
  Serial.print(click_peak_val * 1000);
  if (stereo_audio) {
    Serial.print(" R: ");
    Serial.print(click_rms_val_r * 1000);
    Serial.print(" delta: ");
    Serial.print(click_rms_delta_r * 1000);
    Serial.print(" peak: ");
    Serial.print(click_peak_val_r * 1000);
  }
  Serial.println();
}

void calculateClickAudioFeatures() {
  if (CLICK_FEATURE == "rms_delta" || CLICK_FEATURE == "all") {
    if (click_rms.available()) {
      last_click_rms_val = click_rms_val;
      click_rms_val = click_rms.read();
      click_rms_delta = last_click_rms_val - click_rms_val;
      if (click_rms_delta > CLICK_RMS_DELTA_THRESH) {
        // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
        // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
        if (flash_delay <= 0) {
          flash_delay = MIN_CLICK_LENGTH;
          num_past_clicks++;
          Serial.print("-------------- FRONT CLICK DETECTED --------------------- ");
          Serial.print("rms_delta | ");
          Serial.println(num_past_clicks);
        } else if (flash_delay < MAX_CLICK_LENGTH) {
          flash_delay++;
        } else if (flash_delay > MAX_CLICK_LENGTH) {
          flash_delay = MAX_CLICK_LENGTH;
        }
      }
    }
  }
  if (CLICK_FEATURE == "peak_delta" || CLICK_FEATURE == "all") {
    if ( click_peak.available() ) {
      last_click_peak_val = click_peak_val;
      click_peak_val = click_peak.read();
      click_peak_delta = last_click_peak_val  - click_peak_val;
      if (click_peak_delta > CLICK_PEAK_DELTA_THRESH) {
        // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
        // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
        if (flash_delay <= 0) {
          flash_delay = MIN_CLICK_LENGTH;
          num_past_clicks++;
          Serial.print("-------------- FRONT fCLICK DETECTED --------------------- ");
          Serial.print("peak_delta | ");
          Serial.println(num_past_clicks);
        } else if (flash_delay < MAX_CLICK_LENGTH) {
          flash_delay++;
        } else if (flash_delay > MAX_CLICK_LENGTH) {
          flash_delay = MAX_CLICK_LENGTH;
        }
      }
    }
  }
  if (CLICK_FEATURE != "peak_delta" && CLICK_FEATURE != "rms_delta") {
    Serial.print("sorry the CLICK_FEATURE ");
    Serial.print(CLICK_FEATURE);
    Serial.println(" is not implemented/available");
  }
  // if stereo mode is active also calculate based on rear
  if (stereo_audio == true) {
    calculateClickAudioFeaturesRear();
  }
}

void calculateClickAudioFeaturesRear() {
  if (CLICK_FEATURE == "rms_delta" || CLICK_FEATURE == "all") {
    if (click_rms_r.available()) {
      last_click_rms_val_r = click_rms_val_r;
      click_rms_val_r = click_rms_r.read();
      click_rms_delta_r = last_click_rms_val_r - click_rms_val_r;
      if (click_rms_delta_r > CLICK_RMS_DELTA_THRESH) {
        // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
        // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
        if (flash_delay_r <= 0) {
          flash_delay_r = MIN_CLICK_LENGTH;
          num_past_clicks_r++;
          Serial.print("-------------- REAR CLICK DETECTED --------------------- ");
          Serial.print("rms_delta | ");
          Serial.println(num_past_clicks_r);
        } else if (flash_delay_r < MAX_CLICK_LENGTH) {
          flash_delay_r++;
        } else if (flash_delay_r > MAX_CLICK_LENGTH) {
          flash_delay_r = MAX_CLICK_LENGTH;
        }
      }
    }
  }
  if (CLICK_FEATURE == "peak_delta" || CLICK_FEATURE == "all") {
    if ( click_peak_r.available() ) {
      last_click_peak_val_r = click_peak_val_r;
      click_peak_val_r = click_peak_r.read();
      click_peak_delta_r = last_click_peak_val_r  - click_peak_val_r;
      if (click_peak_delta_r > CLICK_PEAK_DELTA_THRESH) {
        // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
        // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
        if (flash_delay_r <= 0) {
          flash_delay_r = MIN_CLICK_LENGTH;
          num_past_clicks_r++;
          Serial.print("-------------- REAR CLICK DETECTED --------------------- ");
          Serial.print("peak_delta | ");
          Serial.println(num_past_clicks_r);
        } else if (flash_delay_r < MAX_CLICK_LENGTH) {
          flash_delay_r++;
        } else if (flash_delay_r > MAX_CLICK_LENGTH) {
          flash_delay_r = MAX_CLICK_LENGTH;
        }
      }
    }
  }
  if (CLICK_FEATURE != "peak_delta" && CLICK_FEATURE != "rms_delta") {
    Serial.print("sorry the CLICK_FEATURE ");
    Serial.print(CLICK_FEATURE);
    Serial.println(" is not implemented/available");
  }
}
///////////////////////////////////////////////////////////////////////
//                    Song Audio Functions
///////////////////////////////////////////////////////////////////////

/* TODO need a watchdog program which will run adjust gain if certain conditions are met.
    One such condition is if too many click events are detected within a certain period of time,
    Another condition is if too few click everts are detected within a certain period of time,
    Another condition is if the red led brightness is too low, for an extended period of time
    Another condition is if the red led brightness is too high over an extended period of time.
*/

/* TODO - need a function which will poll the lux sensor and then adjust the brightness of the
    LEDs.

   Also there should be a watchdog which every 10 minutes (or so) will re-poll the lux sensor
   when the LEDs are turned off to readjst the brightness scales.

*/
void printDouble( double val, unsigned int precision) {
  // prints val with number of decimal places determine by precision
  // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
  // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  Serial.print("."); // print the decimal point
  unsigned int frac;
  if (val >= 0)
    frac = (val - int(val)) * precision;
  else
    frac = (int(val) - val ) * precision;
  int frac1 = frac;
  while ( frac1 /= 10 )
    precision /= 10;
  precision /= 10;
  while (  precision /= 10)
    Serial.print("0");

  Serial.println(frac, DEC) ;
}

void autoGainAdjustFront() {
  // TODO update this for both front and rear
  /* The purpose of this function is to determine the gain level for both the "song" and the
       "click" bands. The function will run for a number of milliseconds equal to the variable len
      passed into it. (usually around 5 or 10 seconds)

     It will poll the RMS and Peak for each band for this period of time and determine what the max
     and average values are.

     It will then adjust the master gain levels, click_gain and song_gain to meet target values
     target_click_rms and target_song_rms.

      This function should be run in the setup loop when the teensy is booting as well as a few times an
      hour to adjust the gain levels.

  */
  if (millis() < 60000) {
    return;
  }
  // if it has not been long enough since the last check then exit now
  if (last_auto_gain_adjustment < auto_gain_frequency) {
    return;
  };
  Serial.println("-------------------- Auto Gain Start ---------------------------");
  Serial.print("num_past_clicks: ");
  Serial.println(num_past_clicks);
  // first check is to see if there has been too many/few clicks detected
  double clicks_per_minute = ((double)num_past_clicks * 60000) / (double)last_auto_gain_adjustment;
  double cost; // our cost variable
  Serial.print("clicks_per_minute: ");
  // printDouble(clicks_per_minute, 1000000);
  Serial.println(clicks_per_minute);
  // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
  if (clicks_per_minute == 0) {
    click_gain += click_gain * MAX_GAIN_ADJUSTMENT;
    click_gain = min(click_gain, MAX_CLICK_GAIN);
    // update the click gain in the three gain stages
    updateClickGain();
  }
  // then check if there are too few clicks_per_minute
  else if (clicks_per_minute < MIN_CLICKS_PER_MINUTE) {
    // there are too few clicks, need to increase the gain to compensate
    // first we calculate the factor by which the clicks are off
    // the higher the number the more it is off
    cost = 1.0 - (clicks_per_minute / MIN_CLICKS_PER_MINUTE);

    // take the max amount that can be adjusted based off the current_gain and the max_gain_adj
    // then multiply that by the inverted cost
    click_gain += click_gain * MAX_GAIN_ADJUSTMENT * cost;

    // make sure that the click_gain is not more than the max_click_gain
    click_gain = min(click_gain, MAX_CLICK_GAIN);

    // update the click gain in the three gain stages
    updateClickGain();
  }
  // then check to see if there are too many clicks per-minute
  else if (clicks_per_minute > MAX_CLICKS_PER_MINUTE) {
    // determine a cost function... the higher the value the more the cost
    cost = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute);

    // adjust the click_gain
    click_gain -= click_gain * MAX_GAIN_ADJUSTMENT * cost;

    // make sure that the gain is not set too low
    click_gain = max(click_gain, MIN_CLICK_GAIN);

    // update the click gain in the three gain stages
    updateClickGain();
  }

  ///////////////////////////////////////////////////////////////
  // second check is to see if the song gain needs to be adjusted
  ///////////////////////////////////////////////////////////////

  // calculate the average peak values since the last auto-gain adjust
  double avg_song_peak = total_song_peaks / num_song_peaks;
  Serial.println("\n--------- song -------------");
  Serial.print("total_song_peaks ");
  Serial.println(total_song_peaks);
  Serial.print("num_song_peaks ");
  Serial.println(num_song_peaks);
  // if the avg value is more than the max...
  if (avg_song_peak > MAX_SONG_PEAK_AVG) {
    // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
    cost = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak);
    // calculate what the new song_gain will be
    double change = song_gain * MAX_GAIN_ADJUSTMENT * cost;
    song_gain -= change;
    // ensure that what we have is not less than the min
    song_gain = max(song_gain, MIN_SONG_GAIN);
    Serial.print("song gain decreased by ");
    Serial.print(change);
    Serial.print(" ");
    updateSongGain();
  }
  // if the average value is less than the min....
  else if (avg_song_peak < MIN_SONG_PEAK_AVG) {
    Serial.println();
    Serial.print("avg_song_peak lower than MIN_SONG_PEAK_AVG ");
    Serial.println(avg_song_peak);
    // calculate cost between 0 and 1 with higher cost resulting in higher gain attenuation
    cost = 1.0 - (MIN_SONG_PEAK_AVG / avg_song_peak);
    Serial.print("cost : ");
    Serial.println(cost);
    // calculate the new song gain
    double change = song_gain * MAX_GAIN_ADJUSTMENT * cost;
    song_gain += change;
    // ensure what we have is not less than the max...
    song_gain = min(song_gain, MAX_SONG_GAIN);
    Serial.print("song gain increased by ");
    Serial.print(change);
    Serial.print(" ");
    Serial.println();
    updateSongGain();
  }

  ///////////////////////////////////////////////////////////////
  // last thing to do is reset the last_auto_gain_adjustment timer
  ///////////////////////////////////////////////////////////////

  if (stereo_audio == true) {
    autoGainAdjustRear();
  }
  total_song_peaks = 0;
  num_song_peaks = 0;
  num_past_clicks = 0;
  last_auto_gain_adjustment = 0;
  Serial.println(" ------------------------------------------------- ");
}


void autoGainAdjustRear() {
  /* The purpose of this function is to determine the gain level for both the "song" and the
      "click" bands. The function will run for a number of milliseconds equal to the variable len
     passed into it. (usually around 5 or 10 seconds)

    It will poll the RMS and Peak for each band for this period of time and determine what the max
    and average values are.

    It will then adjust the master gain levels, click_gain and song_gain to meet target values
    target_click_rms and target_song_rms.

     This function should be run in the setup loop when the teensy is booting as well as a few times an
     hour to adjust the gain levels.

  */
  // if it has not been long enough since the last check then exit now
  Serial.print("num_past_clicks_r: ");
  Serial.println(num_past_clicks_r);
  // first check is to see if there has been too many/few clicks detected
  double clicks_per_minute_r = ((double)num_past_clicks_r * 60000) / (double)last_auto_gain_adjustment;
  double cost_r; // our cost variable
  Serial.print("clicks_per_minute_r: ");
  // printDouble(clicks_per_minute, 1000000);
  Serial.println(clicks_per_minute_r);
  // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
  if (clicks_per_minute_r == 0) {
    click_gain_r += click_gain_r * MAX_GAIN_ADJUSTMENT;
    click_gain_r = min(click_gain_r, MAX_CLICK_GAIN);
    // update the click gain in the three gain stages
    updateClickGain();
  }
  // then check if there are too few clicks_per_minute
  else if (clicks_per_minute_r < MIN_CLICKS_PER_MINUTE) {
    // there are too few clicks, need to increase the gain to compensate
    // first we calculate the factor by which the clicks are off
    // the higher the number the more it is off
    cost_r = 1.0 - (clicks_per_minute_r / MIN_CLICKS_PER_MINUTE);

    // take the max amount that can be adjusted based off the current_gain and the max_gain_adj
    // then multiply that by the inverted cost
    click_gain_r += click_gain_r * MAX_GAIN_ADJUSTMENT * cost_r;

    // make sure that the click_gain is not more than the max_click_gain
    click_gain_r = min(click_gain_r, MAX_CLICK_GAIN);

    // update the click gain in the three gain stages
    updateClickGain();
  }
  // then check to see if there are too many clicks per-minute
  else if (clicks_per_minute_r > MAX_CLICKS_PER_MINUTE) {
    // determine a cost function... the higher the value the more the cost
    cost_r = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute_r);

    // adjust the click_gain
    click_gain_r -= click_gain_r * MAX_GAIN_ADJUSTMENT * cost_r;

    // make sure that the gain is not set too low
    click_gain_r = max(click_gain_r, MIN_CLICK_GAIN);

    // update the click gain in the three gain stages
    updateClickGain();
  }

  ///////////////////////////////////////////////////////////////
  // second check is to see if the song gain needs to be adjusted
  ///////////////////////////////////////////////////////////////

  // calculate the average peak values since the last auto-gain adjust
  double avg_song_peak_r = total_song_peaks_r / num_song_peaks_r;
  // if the avg value is more than the max...
  if (avg_song_peak_r > MAX_SONG_PEAK_AVG) {
    // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
    cost_r = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak_r);
    // calculate what the new song_gain will be
    double change_r = song_gain_r * MAX_GAIN_ADJUSTMENT * cost_r;
    song_gain_r -= change_r;
    // ensure that what we have is not less than the min
    song_gain_r = max(song_gain_r, MIN_SONG_GAIN);
    Serial.print("song gain_r decreased by ");
    Serial.print(change_r);
    Serial.print(" ");
    updateSongGain();
  }
  // if the average value is less than the min....
  else if (avg_song_peak_r < MIN_SONG_PEAK_AVG) {
    // calculate cost between 0 and 1 with higher cost resulting in higher gain attenuation
    cost_r = 1.0 - (MIN_SONG_PEAK_AVG / avg_song_peak_r);
    // calculate the new song gain
    double change_r = song_gain_r * MAX_GAIN_ADJUSTMENT * cost_r;
    song_gain_r += change_r;
    // ensure what we have is not less than the max...
    song_gain_r = min(song_gain_r, MAX_SONG_GAIN);
    Serial.print("song gain_r increased by ");
    Serial.print(change_r);
    Serial.print(" ");
    updateSongGain();
  }

  ///////////////////////////////////////////////////////////////
  // last thing to do is reset the last_auto_gain_adjustment timer
  ///////////////////////////////////////////////////////////////
  total_song_peaks = 0.0;
  num_song_peaks = 0.0;
  num_past_clicks = 0;
  last_auto_gain_adjustment = 0;
}

void updateSongGain() {
  song_input_amp.gain(song_gain);
  song_mid_amp.gain(song_gain);
  song_post_amp.gain(song_gain);
  Serial.print("updated song gain : ");
  Serial.print(song_gain);
  if (stereo_audio) {
    song_input_amp_r.gain(song_gain_r);
    song_mid_amp_r.gain(song_gain_r);
    song_post_amp_r.gain(song_gain_r);
    Serial.print(" - ");
    Serial.print(song_gain_r);
  }
  Serial.println();
}

void updateClickGain() {
  if (click_gain > click_gain_high) {
    click_gain_high = click_gain;
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT, click_gain_high);
  }
  if (click_gain < click_gain_low) {
    click_gain_low = click_gain;
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT, click_gain_low);
  }
  click_input_amp.gain(click_gain);
  click_mid_amp.gain(click_gain);
  click_post_amp.gain(click_gain);
  Serial.print("updated click gain : ");
  Serial.print(click_gain);
  if (stereo_audio) {
    if (click_gain_r > click_gain_high_r) {
      click_gain_high_r = click_gain_r;
      writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_REAR, click_gain_high_r);
    }
    if (click_gain_r < click_gain_low_r) {
      click_gain_low_r = click_gain_r;
      writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_REAR, click_gain_low_r);
    }
    click_input_amp_r.gain(click_gain_r);
    click_mid_amp_r.gain(click_gain_r);
    click_post_amp_r.gain(click_gain_r);
    Serial.print(" - ");
    Serial.print(click_gain_r);
  }
  Serial.println();
}

void printSongStats() {
  if (PRINT_SONG_FEATURES) {
    Serial.print("Song -- F| rms_weighted: ");
    Serial.print(song_rms_weighted);
    Serial.print("\t peak: ");
    Serial.print(song_peak_weighted);
    if (stereo_audio) {
      Serial.print(" R| rms_weighted: ");
      Serial.print(song_rms_weighted_r);
      Serial.print("\t peak: ");
      Serial.print(song_peak_weighted_r);
    }
    Serial.println();
  }
}

void calculateSongAudioFeatures() {
  // TODO, rework the whole calculate weighted song brigtness to something that makes more sense
  // TODO rework to only calculate the features which are the features used
  if (song_rms.available()) {
    double song_rms_val = song_rms.read();
    song_rms_weighted = map(constrain((song_rms_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
  }

  if (song_peak.available()) {
    // random magiv 5 varaible needs to be better defined/tested TODO
    double song_peak_val = song_peak.read();
    song_peak_weighted = map(constrain((song_peak_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
    num_song_peaks++;
    total_song_peaks += song_peak_val * 100;
    // will only print if flag is set
    printSongStats();
  }

  if (stereo_audio) {
    if (song_rms_r.available()) {
      double song_rms_val_r = song_rms_r.read();
      song_rms_weighted_r = map(constrain((song_rms_val_r * 5), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
    }

    if (song_peak_r.available()) {
      // random magiv 5 varaible needs to be better defined/tested TODO
      double song_peak_val_r = song_peak_r.read();
      song_peak_weighted_r = map(constrain((song_peak_val_r * 5), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
      num_song_peaks_r++;
      total_song_peaks_r += song_peak_val_r * 100;
    }
  }
  // TODO, perhaps add another feature or two, perhaps an option for combining the two readings?
}

void songDisplay() {
  // if the click WHITE light is on, then ignore the song rms
  if (flash_on == true) {
    return;
  }
  // TODO make a user control which allows for selection between RMs and peak
  if (SONG_FEATURE == "peak") {
    if (stereo_audio == false) {
      colorWipe(song_peak_weighted, 0, 0);
    } else {
      colorWipeFront(song_peak_weighted, 0, 0);
      colorWipeRear(song_peak_weighted_r, 0, 0);
    }
  } else if (SONG_FEATURE == "rms") {
    if (stereo_audio == false) {
      colorWipe(song_rms_weighted, 0, 0);
    } else  {
      colorWipeFront(song_rms_weighted, 0, 0);
      colorWipeRear(song_rms_weighted_r, 0, 0);
    }
  } else {
    Serial.print("ERROR: the SONG_FEATURE ");
    Serial.print(SONG_FEATURE);
    Serial.println(" is not a valid/implemented SONG_FEATURE");
  }
}

///////////////////////////////////////////////////////////////////////
//                    General Purpose Audio Functions
///////////////////////////////////////////////////////////////////////

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
///////////////////////////////////////////////////////////////////////
//                            Datalogging
///////////////////////////////////////////////////////////////////////

void writeTwoBytesToEEPROM(unsigned int addr, uint16_t data) {
  // store the dat with least significant bytes in lower index
  uint8_t lsb = data & 0x00FF;
  uint8_t msb = data >> 8;
  EEPROM.write(addr, lsb);
  EEPROM.write(addr + 1, msb);
}

uint16_t read16BitsFromEEPROM(unsigned int addr) {
  uint16_t data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return data;
}

void writeDoubleToEEPROM(unsigned int addr, double data) {
  // store the dat with least significant bytes in lower index
  uint16_t d = data * 100;
  uint8_t lsb = d & 0x00FF;
  uint8_t msb = d >> 8;
  EEPROM.write(addr, lsb);
  EEPROM.write(addr + 1, msb);
}

double readDoubleFromEEPROM(unsigned int addr) {
  uint16_t data = EEPROM.read(addr + 1);
  data = (data << 8) + EEPROM.read(addr);
  return (double)data / 100;
}

void writeSetupConfigsToEEPROM() {
  if (data_logging_active) {

    writeTwoBytesToEEPROM(EEPROM_TEST1, 99);
    writeTwoBytesToEEPROM(EEPROM_TEST2, 9999);
    Serial.println("tested functionality");

    EEPROM.write(EEPROM_JMP1, cicada_mode);
    EEPROM.write(EEPROM_JMP2, stereo_audio);
    EEPROM.write(EEPROM_JMP3, both_lux_sensors);
    EEPROM.write(EEPROM_JMP4, combine_lux_readings);
    EEPROM.write(EEPROM_JMP5, gain_adjust_active);
    EEPROM.write(EEPROM_JMP6, data_logging_active);
    Serial.println("logged jumper values to EEPROM");

    EEPROM.write(EEPROM_AUDIO_MEM_MAX, AUDIO_MEMORY);
    Serial.println("logged AUDIO_MEMORY to EEPROM");
  }
}

void writeAudioUsageToEEPROM(uint8_t used) {
  if (data_logging_active) {
    EEPROM.write(EEPROM_AUDIO_MEM_USAGE, used);
    Serial.println("logged audio memory usage to EEPROM");
  }
}

void printEEPROMContents() {
  Serial.println("-------------------------------------");
  Serial.println("Printing EEPROM CONTENTS ...");
  Serial.println("-------------------------------------");
  Serial.println("onboard jumper settings");
  Serial.print("jumpers :\t");
  Serial.print(EEPROM.read(EEPROM_JMP1));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP2));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP3));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP4));
  Serial.print("\t");
  Serial.print(EEPROM.read(EEPROM_JMP5));
  Serial.print("\t");
  Serial.println(EEPROM.read(EEPROM_JMP6));
  Serial.println("-------------------------------------");
  Serial.println("\nFirmware Setup Configurations");

  Serial.println("\nAudio Settings\n||||||||||||||||||||||||||||||||||");
  Serial.print("Audio memory usage max   :\t");
  Serial.println(EEPROM.read(EEPROM_AUDIO_MEM_USAGE));
  Serial.print("Audio memory allocated   :\t");
  Serial.println(EEPROM.read(EEPROM_AUDIO_MEM_MAX));
  Serial.print("min/max front click gain :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT));
  Serial.print("min/max rear click gain  :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN_REAR)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX_REAR));

  Serial.println("\nLux Settings\n||||||||||||||||||||||||||||||||||");
  Serial.print("min/max front lux reading     : \t");
  Serial.print(read16BitsFromEEPROM(EEPROM_MIN_LUX_READING_FRONT)); Serial.print("\t");
  Serial.println(read16BitsFromEEPROM(EEPROM_MAX_LUX_READING_FRONT));
  Serial.print("min/max rear lux reading      : \t");
  Serial.print(read16BitsFromEEPROM(EEPROM_MIN_LUX_READING_REAR)); Serial.print("\t");
  Serial.println(read16BitsFromEEPROM(EEPROM_MAX_LUX_READING_REAR));
  Serial.print("min/max combined lux reading  : \t");
  Serial.print(read16BitsFromEEPROM(EEPROM_MIN_LUX_READING_COMBINED)); Serial.print("\t");
  Serial.println(read16BitsFromEEPROM(EEPROM_MAX_LUX_READING_COMBINED));

  Serial.println("\nTest Values\n||||||||||||||||||||||||||||||||||");
  Serial.print("should be equal to 99 = ");
  Serial.println(read16BitsFromEEPROM(EEPROM_TEST1));
  Serial.print("should be equal to 9999 = ");
  Serial.println(read16BitsFromEEPROM(EEPROM_TEST2));
  Serial.println("-------------------------------------");
  Serial.println("Finished Printing EEPROM Datalog");
  Serial.println("-------------------------------------\n");
}

///////////////////////////////////////////////////////////////////////
//                    Setup and Main Loops
///////////////////////////////////////////////////////////////////////

void readJumpers() {
  Serial.println("reading jumpers");
  pinMode(JMP1_PIN, INPUT);
  pinMode(JMP2_PIN, INPUT);
  pinMode(JMP3_PIN, INPUT);
  pinMode(JMP4_PIN, INPUT);
  pinMode(JMP5_PIN, INPUT);
  pinMode(JMP6_PIN, INPUT);

  delay(100);

  cicada_mode = digitalRead(JMP1_PIN);
  stereo_audio = digitalRead(JMP2_PIN);
  both_lux_sensors = digitalRead(JMP3_PIN);
  combine_lux_readings = digitalRead(JMP4_PIN);
  gain_adjust_active = digitalRead(JMP5_PIN);
  data_logging_active = digitalRead(JMP6_PIN);
  Serial.println("Finished reading jumpers\n");
}

void cicadaLoop() {
  // SONG /////////////////
  calculateSongAudioFeatures();
  songDisplay();

  // Click ////////////////
  calculateClickAudioFeatures();
  // printClickStats();
  clickFlashes();
}

void cicadaSetup() {
  /////////////////////////////////
  // Start Serial and let things settle
  /////////////////////////////////
  Serial.begin(57600);
  Serial.println("Starting Cicada Mode Setup Loop");

  ////////////// Audio ////////////
  AudioMemory(AUDIO_MEMORY);
  // Audio for the click channel...

  click_biquad.setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad.setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad.setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad.setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);

  click_biquad2.setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad2.setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad2.setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad2.setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);

  click_input_amp.gain(click_gain);
  click_mid_amp.gain(click_gain);
  click_post_amp.gain(click_gain);

  // Audio for the song channel...
  song_biquad.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);

  song_biquad2.setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad2.setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad2.setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad2.setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);

  song_input_amp.gain(song_gain);
  song_mid_amp.gain(song_gain);
  song_post_amp.gain(song_gain);

  if (stereo_audio) {
    Serial.println("setting up stereo audio analysis chain");
    setupRearAudio();
  }

  /////////////////////////////////
  // Start the LEDs ///////////////
  /////////////////////////////////
  Serial.println("setting up LEDs");
  leds.begin();
  delay(250);
  colorWipe(0, 0, 0); // turn off the LEDs

  /////////////////////////////////
  // VEML sensors through TCA9543A
  /////////////////////////////////
  setupVEMLthroughTCA();
  delay(200);
  Serial.print("starting click gain: ");
  Serial.println(click_gain);
  Serial.print("starting song gain: ");
  Serial.println(song_gain);
  Serial.println("Finshed cicada Setup Loop");
  Serial.println("------------------");
  delay(200);
}


void setupRearAudio() {
  Serial.println("Setting up audio chain for rear microphone");

  click_biquad_r.setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad_r.setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad_r.setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad_r.setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);

  click_biquad2_r.setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad2_r.setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad2_r.setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad2_r.setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);

  click_input_amp_r.gain(click_gain_r);
  click_mid_amp_r.gain(click_gain_r);
  click_post_amp_r.gain(click_gain_r);

  // Audio for the song channel...
  song_biquad_r.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad_r.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad_r.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad_r.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);

  song_biquad2_r.setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad2_r.setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad2_r.setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad2_r.setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);

  song_input_amp_r.gain(song_gain_r);
  song_mid_amp_r.gain(song_gain_r);
  song_post_amp_r.gain(song_gain_r);
}

void setup() {
  if (PRINT_EEPROM_CONTENTS == true) {
    Serial.begin(57600);
    delay(1000);
    Serial.println("preparing to print EEPROM contents");
    delay(4000);
    printEEPROMContents();
  }
  if (JUMPERS_POPULATED) {
    readJumpers();
  }
  if (cicada_mode) {
    cicadaSetup();
  } else {
    while (1) {
      Serial.println("sorry the bird mode is not yet implemented");
      delay(1000);
    }
  }
  writeSetupConfigsToEEPROM();
}

void loop() {
  if (cicada_mode) {
    cicadaLoop();
  }
  // Lux //////////////////
  if (checkLuxSensors()) {
    printLuxReadings();
  }
  checkAudioUsage();
  autoGainAdjustFront(); // will call rear as well if in stereo mode
}
