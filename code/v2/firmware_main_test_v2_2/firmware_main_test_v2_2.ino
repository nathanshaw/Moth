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

#define SERIAL_ID 7

// record the run time // last value is number of minutes
#define RUNTIME_POLL_DELAY (1000*60*5)
// when should the value log start // last value is number of minutes
#define LOG_START_DELAY (1000*60*30)
// how long should the logging last? // last value is number of hours
#define LOG_TIME_FRAME (1000*60*60*50)

// turn on/off reading jumpers in setup (if off take the default "true" values for jumper bools
#define JUMPERS_POPULATED 0

// turn on/off auto gain. 0 is off, 1 is on
#define AUTO_GAIN 1

#define CICADA_MODE_ACTIVE false
#define PITCH_MODE_ACTIVE true

elapsedMillis last_runtime_update;

extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus scanning
}
///////////////////////// Debuggings ////////////////////////////////////
#define PRINT_LUX_DEBUG       false
#define PRINT_LUX_READINGS    false
// TODO
#define PRINT_SONG_DATA       false
// TODO
#define PRINT_CLICK_FEATURES  false
#define PRINT_CLICK_DEBUG     false
// TODO
#define PRINT_LED_VALUES      false
#define PRINT_AUTO_GAIN       false

#define PRINT_LOG_WRITE       true

#define EEPROM_WRITE_CHECK    false

/////////////////////////////////////////////////////////////////////////
//////////////////////// Firmware Controls //////////////////////////////
/////////////////////////////////////////////////////////////////////////
// set to true if you want to print out data stored in EEPROM on boot
#define PRINT_EEPROM_CONTENTS true

// turns on/off debug printing
#define DEBUG 0

// data logging related
#define AUDIO_USAGE_POLL_RATE 200000

// Neo Pixels
#define MAX_BRIGHTNESS 250
#define MIN_BRIGHTNESS 20

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
#define MIN_CLICKS_PER_MINUTE 0.1
#define MAX_CLICKS_PER_MINUTE 40.0

#define STARTING_CLICK_GAIN 6.0

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
#define MAX_CLICK_LENGTH 100
// where the click flash timer will start
#define MIN_CLICK_LENGTH 60


//////// Song Settings

// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
#define SONG_FEATURE "peak"

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.005
#define MAX_SONG_PEAK_AVG 0.20

#define STARTING_SONG_GAIN 12.0

#define SONG_BQ1_THRESH 13500
#define SONG_BQ1_Q 0.85
#define SONG_BQ1_DB -12
#define SONG_BQ2_THRESH 14000
#define SONG_BQ2_Q 0.85
#define SONG_BQ2_DB -12

////////////////////////////////////////////////////////////////////////////
//////////////////////// Hardware Controls /////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// cicada (-) or bird song (+) mode
#define JMP1_PIN 12
bool cicada_mode = false;
bool pitch_mode = true;

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
#define EEPROM_JMP1                       0
#define EEPROM_JMP2                       1
#define EEPROM_JMP3                       2
#define EEPROM_JMP4                       3
#define EEPROM_JMP5                       4
#define EEPROM_JMP6                       5

#define EEPROM_SERIAL_ID                  40
#define EEPROM_RUN_TIME                   41

#define EEPROM_AUDIO_MEM_USAGE            45
#define EEPROM_AUDIO_MEM_MAX              46

#define EEPROM_LOG_POLLING_RATE           50
#define EEPROM_LOG_START_TIME             54
#define EEPROM_LOG_END_TIME               58
#define EEPROM_LOG_ACTIVE                 63

// lux readings, each value takes up 4 spots
#define EEPROM_MIN_LUX_READINGS           100
#define EEPROM_MIN_LUX_READING_COMBINED   108
#define EEPROM_MAX_LUX_READINGS           112
#define EEPROM_MAX_LUX_READING_COMBINED   120

// audio
#define EEPROM_CLICK_GAIN_MIN_FRONT       200
#define EEPROM_CLICK_GAIN_MAX_FRONT       204
#define EEPROM_CLICK_GAIN_MIN_REAR        208
#define EEPROM_CLICK_GAIN_MAX_REAR        212
#define EEPROM_CLICK_GAIN_START_FRONT     216
#define EEPROM_CLICK_GAIN_START_REAR      220

#define EEPROM_SONG_GAIN_MIN_FRONT        224
#define EEPROM_SONG_GAIN_MAX_FRONT        228
#define EEPROM_SONG_GAIN_MIN_REAR         232
#define EEPROM_SONG_GAIN_MAX_REAR         236
#define EEPROM_SONG_GAIN_START_FRONT      240
#define EEPROM_SONG_GAIN_START_REAR       244

#define EEPROM_TOTAL_CLICKS_REAR          248
#define EEPROM_TOTAL_CLICKS_FRONT         252

// current values
#define EEPROM_SONG_GAIN_CURRENT_FRONT    256
#define EEPROM_SONG_GAIN_CURRENT_REAR     260
#define EEPROM_CLICK_GAIN_CURRENT_FRONT   264
#define EEPROM_CLICK_GAIN_CURRENT_REAR    268

// clicks per second log (100 spots, double)
#define EEPROM_CPM_LOG_START              300
// #define EEPROM_CPM_INT                 400
#define EEPROM_CPM_LOG_LENGTH             40

// Lux Log
#define EEPROM_LUX_LOG_START              1000
#define EEPROM_LUX_LOG_LENGTH             40

// Led logs/info
// how often are the LEDs on instead of off?
// what is the average brightness scaler?
#define EEPROM_LED_ON_RATIO               1800
#define EEPROM_AVG_BRIGHTNESS_SCALER      1808

unsigned int cpm_eeprom_idx = EEPROM_CPM_LOG_START;
const long EEPROM_CPM_LOG_END = EEPROM_CPM_LOG_START + (4 * 2 * EEPROM_CPM_LOG_LENGTH);//4 bits to double, front and rear, log length

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

// all these are declared as an array just in case dual-sided lights are used

int16_t remaining_flash_delay[2];           // to keep track of the flash
bool flash_on[2] = {false, false};              // to keep track of the flash
elapsedMillis last_click_flash[2]; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
elapsedMillis led_shdn_timer[2];
bool leds_on[2] = {false, false};               // set to false if the LEDs are turned off and true if the leds are on...
elapsedMillis led_off_len[2];          // keeps track of how long the LEDs are turned off, it will reset when colorWipe is called with a color of 0

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

// to keep track of how often the leds are on/off
elapsedMillis led_switch_timer[2];// each time color wipe is called this value is reset
long led_on_times[2] = {1, 1};
long led_off_times[2] = {1, 1};
double led_on_ratio[2];

////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// LUX ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// TODO these need to change into variables which are set with the jumpers?
#define FRONT_LUX_INSTALLED true
#define REAR_LUX_INSTALLED true

const long LOG_POLLING_RATE = (long)((double)LOG_TIME_FRAME / (double)EEPROM_LUX_LOG_LENGTH * 2.0);
const long EEPROM_LUX_LOG_END = EEPROM_LUX_LOG_START + (4 * 2 * EEPROM_LUX_LOG_LENGTH);//4 bits to double, front and rear, log length
elapsedMillis log_timer;
unsigned int lux_eeprom_idx = EEPROM_LUX_LOG_START;
int lux_total[2];
int lux_readings[2];

unsigned long lux_max_reading_delay = long(1000 * 60 * 6); // every 6 minutes
unsigned long lux_min_reading_delay = long(1000 * 60 * 0.05); // 3 seconds

#define SMOOTH_LUX_READINGS true

#define MIN_LUX_EXPECTED 1.0
#define MAX_LUX_EXPECTED 400.0
// on scale of 1-1000 what is the min multiplier for lux sensor brightness adjustment
// 500 would equate to a dimming of 50%
#define BRIGHTNESS_SCALER_MIN 800

// turn this flag off if no I2C multiplexer is present
#define I2C_MULTI 1

elapsedMillis lux_reading_timer[2];

// front front, rear rear, combined, combined
double brightness_scaler[3]    = {1.0, 1.0, 1.0};// , 1.0, 1.0, 1.0};
double total_brightness_scalers[3];
double total_brightness_readings[3];
double avg_brightness_readings[3];

void updateBrightnessScalerTotals() {
  for (int i = 0; i < 3; i++) {
    total_brightness_scalers[i] += brightness_scaler[i];
    total_brightness_readings[i]++;
  }
}

void updateBrightnessScalerAverages() {
  for (int i = 0; i < 3; i++) {
    avg_brightness_readings[i] = total_brightness_scalers[i] / total_brightness_readings[i];
  }
}

// front / rear and combined
double lux[3];

double min_lux_reading[3] = {9999.9, 9999.9, 9999.9};
double max_lux_reading[3] = {0.0, 0.0, 0.0};

Adafruit_VEML7700 veml[2] = Adafruit_VEML7700();

///////////////////////////////////////////////////////////////////////
//                    Neopixel Related Functions
///////////////////////////////////////////////////////////////////////

void resetOnOffRatioCounters() {
  for (int i = 0; i < 2; i++) {
    led_on_ratio[i] = (double)led_on_times[i] / (double)(led_on_times[i] + led_off_times[i]);
    led_on_times[i] = 0;
    led_off_times[i] = 0;
    led_switch_timer[i] = 0;
  }
  Serial.println("reset the led on/off ratio counters");
}
// mode 0 is just front, mode 1 is just rear, mode 2 is both (using combined values?), mode 3 is both using independent values
void updateOnOffRatios(uint32_t color, uint8_t i) {
  // to keep track of on/off times
  if (color > 0) {
    led_on_times[i] += led_switch_timer[i];
  } else {
    led_off_times[i] += led_switch_timer[i];
  }
  if (led_on_times[i] > 0 && led_off_times[i] > 0) {
    led_on_ratio[i] = (double)led_on_times[i] / (double)(led_on_times[i] + led_off_times[i]);
    led_switch_timer[i] = 0;
  }
  // Serial.print("updated led on/off ratio ");Serial.print(i);Serial.print(" :\t");
  // Serial.print(led_on_ratio[i]);Serial.print("\t=\t");Serial.print(led_on_times[i]);
  // Serial.print("\t");Serial.println(led_off_times[i]);
}

void colorWipe(uint8_t red, uint8_t green, uint8_t blue, uint8_t mode = 2) {
  // TODO this logic is broken...
  // Serial.print("colorWipe : ");Serial.print(mode);Serial.print("\t");
  if (mode < 2) {
    int color = combineColors(red, green, blue, brightness_scaler[mode]);
    updateOnOffRatios(color, mode);
    if (led_shdn_timer[mode] < LED_SHDN_LEN) {
      // if the LEDs are in shutdown mode than simply exit without changing the LEDs
      return;
    }
    if (mode == 0) {
      for (int i = 0; i < leds.numPixels() / 2; i++) {
        leds.setPixel(i, color);
        // Serial.print(i);
      }
    } else {
      for (int i = leds.numPixels() / 2; i < leds.numPixels(); i++) {
        leds.setPixel(i, color);
        // Serial.print(i);
      }
    }
    leds.show();
    // TODO how toa dapt this to new array variables?
    // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
    // also reset led_off_len if the leds  were just turned off
    if (color == 0) {
      if (leds_on[mode] > 0) {
        led_off_len[mode] = 0;
      }
      leds_on[mode] = false;
    } else {
      leds_on[mode] = true;
    }
  }
  // both using combined values
  if (mode == 2) {
    Serial.println("WARNING COLOR WIPE MODE 2 IS NOT IMPLEMENTED");
  }
  // using independent values but calling both
  if (mode == 3) {
    Serial.println("WARNING COLOR WIPE MODE 3 IS NOT IMPLEMENTED");
  }
}

uint32_t combineColors(uint8_t red, uint8_t green, uint8_t blue, double scaler) {
  // TODO change code to account for front and back sides
  uint32_t color = 0;
  red = red * scaler;
  green = green * scaler;
  blue = blue * scaler;
  color = (red << 16) + (green << 8) + (blue);
  return color;
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
  Serial.print("cicada_mode set to : "); Serial.println(cicada_mode);
  stereo_audio = digitalRead(JMP2_PIN);
  num_channels = stereo_audio + 1; // the number of channels we will be using
  Serial.print("stereo_audio set to : "); Serial.println(stereo_audio);
  INDEPENDENT_CLICKS = digitalRead(JMP3_PIN);
  // both_lux_sensors = digitalRead(JMP3_PIN);
  // num_lux_sensors = both_lux_sensors + 1;
  Serial.print("independent clicks set to : "); Serial.println(INDEPENDENT_CLICKS);
  combine_lux_readings = digitalRead(JMP4_PIN);
  Serial.print("combine_lux_readings set to : "); Serial.println(combine_lux_readings);
  gain_adjust_active = digitalRead(JMP5_PIN);
  Serial.print("gain_adjust_active set to : "); Serial.println(gain_adjust_active);

  data_logging_active = digitalRead(JMP6_PIN);
  Serial.print("data_logging_active set to : "); Serial.println(data_logging_active);
  Serial.println("\n------------------------------------");
}

#define TEN_SECONDS 10000
elapsedMillis ten_second_timer;
