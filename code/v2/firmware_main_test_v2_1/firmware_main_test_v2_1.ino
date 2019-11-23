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

#define SERIAL_ID 15
#define I2C_MULTI 1
// record the run time every three minutes for now
#define RUNTIME_POLL_DELAY 180000
elapsedMillis last_runtime_update;

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

// lux sensors // TODO these need to change into variables which are set with the jumpers?
#define FRONT_LUX_INSTALLED true
#define REAR_LUX_INSTALLED true

unsigned long lux_max_reading_delay = 1000 * 60 * 5; // every 6 minutes
unsigned long lux_min_reading_delay = 1000 * 60 * 0.25; // 15 seconds

#define SMOOTH_LUX_READINGS true

#define MIN_LUX_EXPECTED 1.0
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
uint8_t num_channels = stereo_audio + 1;
#define FRONT_MIC true
#define REAR_MIC true

// one front lux (-) or two lux (+)
#define JMP3_PIN 14
bool both_lux_sensors = true;
uint8_t num_lux_sensors = 2;

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
#define EEPROM_JMP1 0
#define EEPROM_JMP2 1
#define EEPROM_JMP3 2
#define EEPROM_JMP4 3
#define EEPROM_JMP5 4
#define EEPROM_JMP6 5
#define EEPROM_AUDIO_MEM_USAGE 50
#define EEPROM_AUDIO_MEM_MAX 51

#define EEPROM_SERIAL_ID 52
#define EEPROM_RUN_TIME 54

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

// all these are declared as an array just in case dual-sided lights are used

int16_t flash_delay[2];           // to keep track of the flash
bool flash_on[2] = {false, false};              // to keep track of the flash
elapsedMillis last_click_flash[2]; // ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
elapsedMillis led_shdn_timer[2];
bool leds_on[2] = {false, false};               // set to false if the LEDs are turned off and true if the leds are on...
elapsedMillis led_off_len[2];          // keeps track of how long the LEDs are turned off, it will reset when colorWipe is called with a color of 0

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// LUX ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

elapsedMillis lux_reading_timer[2];

// front front, rear rear, combined, combined
double brightness_scaler[6]    = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

// front / rear and combined
double lux[3];

double min_lux_reading[3] = {9999.9, 9999.9, 9999.9};
double max_lux_reading[3] = {0.0, 0.0, 0.0};

Adafruit_VEML7700 veml[2] = Adafruit_VEML7700();

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Audio Library /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_MEMORY 24

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=76.66667938232422,1245.6664371490479
AudioAnalyzeRMS          rms_input1;     //xy=260.00000762939453,1316.6666345596313
AudioAnalyzeRMS          rms_input2;     //xy=260.00000762939453,1316.6666345596313
AudioAnalyzePeak         peak_input1;    //xy=264.00000762939453,1348.6666345596313
AudioAnalyzePeak         peak_input2;    //xy=264.00000762939453,1348.6666345596313
AudioAmplifier           click_input_amp1; //xy=282.6666793823242,1217.9999284744263
AudioAmplifier           song_input_amp1; //xy=282.6666717529297,1250.6665239334106
AudioAmplifier           song_input_amp2; //xy=282.66666412353516,1283.9998378753662
AudioAmplifier           click_input_amp2; //xy=286.00001525878906,1184.6665925979614
AudioFilterBiquad        click_biquad2;  //xy=460.9999542236328,1187.9998712539673
AudioFilterBiquad        song_biquad2;   //xy=462.6666946411133,1282.999761581421
AudioFilterBiquad        click_biquad1;  //xy=464.33331298828125,1219.6665210723877
AudioFilterBiquad        song_biquad1;   //xy=464.33331298828125,1249.6665210723877
AudioAmplifier           click_mid_amp2; //xy=630.6666412353516,1185.333086013794
AudioAmplifier           click_mid_amp1; //xy=633.9999542236328,1216.9998998641968
AudioAmplifier           song_mid_amp2;  //xy=634.9999160766602,1284.3331317901611
AudioAmplifier           song_mid_amp1;  //xy=636.6666259765625,1252.6665229797363
AudioFilterBiquad        click_biquad11; //xy=813.0000114440918,1216.333209991455
AudioFilterBiquad        click_biquad21; //xy=814.6666412353516,1187.9998941421509
AudioFilterBiquad        song_biquad11;  //xy=814.9999961853027,1247.3331470489502
AudioFilterBiquad        song_biquad21;  //xy=814.9999771118164,1280.6664805412292
AudioAmplifier           song_post_amp2; //xy=991.333381652832,1295.9998598098755
AudioAmplifier           click_post_amp1; //xy=991.9999771118164,1221.9999532699585
AudioAmplifier           song_post_amp1; //xy=994.6666030883789,1260.9998998641968
AudioAmplifier           click_post_amp2; //xy=995.3332748413086,1185.3333415985107
AudioAnalyzeRMS          song_rms1;      //xy=1257.0000228881836,1259.6665334701538
AudioAnalyzeRMS          song_rms2;      //xy=1258.6666717529297,1224.6665143966675
AudioAnalyzeRMS          click_rms1;     //xy=1260.0000228881836,1086.6665334701538
AudioAnalyzeRMS          click_rms2;     //xy=1261.666648864746,1051.666464805603
AudioAnalyzePeak         song_peak1;     //xy=1261.0000228881836,1291.6665334701538
AudioAnalyzePeak         song_peak2;     //xy=1262.6666717529297,1323.3332242965698
AudioAnalyzePeak         click_peak1;    //xy=1264.0000228881836,1118.6665334701538
AudioAnalyzePeak         click_peak2;    //xy=1265.6666717529297,1151.9998006820679
AudioOutputUSB           usb1;           //xy=1307.33353805542,1409.3331747055054
AudioConnection          patchCord1(i2s1, 0, rms_input1, 0);
AudioConnection          patchCord2(i2s1, 0, peak_input1, 0);
AudioConnection          patchCord3(i2s1, 0, click_input_amp1, 0);
AudioConnection          patchCord4(i2s1, 0, song_input_amp1, 0);
AudioConnection          patchCord5(i2s1, 0, peak_input2, 0);
AudioConnection          patchCord6(i2s1, 0, rms_input2, 0);
AudioConnection          patchCord7(i2s1, 1, click_input_amp2, 0);
AudioConnection          patchCord8(i2s1, 1, song_input_amp2, 0);
AudioConnection          patchCord9(click_input_amp1, click_biquad1);
AudioConnection          patchCord10(song_input_amp1, song_biquad1);
AudioConnection          patchCord11(song_input_amp2, song_biquad2);
AudioConnection          patchCord12(click_input_amp2, click_biquad2);
AudioConnection          patchCord13(click_biquad2, click_mid_amp2);
AudioConnection          patchCord14(song_biquad2, song_mid_amp2);
AudioConnection          patchCord15(click_biquad1, click_mid_amp1);
AudioConnection          patchCord16(song_biquad1, song_mid_amp1);
AudioConnection          patchCord17(click_mid_amp2, click_biquad21);
AudioConnection          patchCord18(click_mid_amp1, click_biquad11);
AudioConnection          patchCord19(song_mid_amp2, song_biquad21);
AudioConnection          patchCord20(song_mid_amp1, song_biquad11);
AudioConnection          patchCord21(click_biquad11, click_post_amp1);
AudioConnection          patchCord22(click_biquad21, click_post_amp2);
AudioConnection          patchCord23(song_biquad11, song_post_amp1);
AudioConnection          patchCord24(song_biquad21, song_post_amp2);
AudioConnection          patchCord25(song_post_amp2, song_rms2);
AudioConnection          patchCord26(song_post_amp2, song_peak2);
AudioConnection          patchCord27(click_post_amp1, click_rms1);
AudioConnection          patchCord28(click_post_amp1, click_peak1);
AudioConnection          patchCord29(click_post_amp1, 0, usb1, 0);
AudioConnection          patchCord30(song_post_amp1, song_rms1);
AudioConnection          patchCord31(song_post_amp1, song_peak1);
AudioConnection          patchCord32(song_post_amp1, 0, usb1, 1);
AudioConnection          patchCord33(click_post_amp2, click_rms2);
AudioConnection          patchCord34(click_post_amp2, click_peak2);
// GUItool: end automatically generated code

// to keep track of when to write audio_usage to EEPROM
uint8_t audio_usage_max = 0;
// for keeping track of audio memory usage
elapsedMillis last_usage_print = 0;

// TODO make it so the routing is different if there is only one channel
double click_gain[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN}; // starting click gain level
double click_gain_low[2] = {100.0, 100.0};
double click_gain_high[2] = {0.0, 0.0};
// rms
double click_rms_val[2] = {0.0, 0.0};
double last_click_rms_val[2] = {0.0, 0.0};
double click_rms_delta[2] = {0.0, 0.0};
// peak
double click_peak_val[2] = {0.0, 0.0};
double last_click_peak_val[2] = {0.0, 0.0};
double click_peak_delta[2] = {0.0, 0.0};

double song_gain[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};   // starting song gain level
uint8_t song_rms_weighted[2] = {0, 0};  // 0 -255 depending on the RMS of the song band...
uint8_t song_peak_weighted[2] = {0, 0}; // 0 -255 depending on the peak of the song band...

///////////////////////////////////////////////////////////////////////
/////////////// Auto-Gain Variables ///////////////////////////////////
///////////////////////////////////////////////////////////////////////
double total_song_peaks[2];
unsigned long num_song_peaks[2];
uint16_t num_past_clicks[2]; // number of clicks which has occurred since boot

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
  if (!I2C_MULTI) {
    if (!veml[0].begin()) {
      Serial.print("VEML "); Serial.print(0); Serial.println(" not found");
    }
    else {
      Serial.print("VEML "); Serial.print(0); Serial.println(" found");
      veml[0].setGain(VEML7700_GAIN_1);
      veml[0].setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
    }
  } else {
    for (int i = 0; i < both_lux_sensors + 1; i++) {
      tcaselect(i);
      if (!veml[i].begin()) {
        Serial.print("VEML "); Serial.print(i); Serial.println(" not found");
      } else {
        Serial.print("VEML "); Serial.print(i); Serial.println(" found");
        veml[i].setGain(VEML7700_GAIN_1);
        veml[i].setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
//                    Neopixel Related Functions
///////////////////////////////////////////////////////////////////////
// mode 0 is just front, mode 1 is just rear, mode 2 is both (using combined values?), mode 3 is both using independent values
void colorWipe(uint8_t red, uint8_t green, uint8_t blue, uint8_t mode = 2) {
  // TODO this logic is broken...
  if (mode < 2) {
    int color = scaleBrightness(red, green, blue, brightness_scaler[mode]);
    if (led_shdn_timer[mode] < LED_SHDN_LEN) {
      // if the LEDs are in shutdown mode than simply exit without changing the LEDs
      return;
    }
    for (int i = 0; i < leds.numPixels(); i++) {
      leds.setPixel(i, color);
    }
    leds.show();
    // TODO how toa dapt this to new array variables?
    // if the LEDs are on set "led_on" to true, otherwise turn "led_on" to false
    // also reset led_off_len if the leds  were just turned off
    if (color == 0) {
      if (leds_on[mode] == true) {
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
  for (int i = 0; i < num_lux_sensors; i++) {
    // for each lux sensor, if the LEDs are off, have been off for longer than the LED_SHDN_LEN
    // and it has been longer than the
    // min reading delay then read the sensors
    if (leds_on[i] == false && led_off_len[i] >= LED_SHDN_LEN && lux_reading_timer[i] > lux_min_reading_delay) {
      if (I2C_MULTI) {
        readLuxSensors(i);
      } else {
        Serial.print("readLuxSensorsWithout()");
        readLuxSensorsWithout();
      }
    }
  }


  // if it has been longer than the "lux_max_reading_delay" force a new reading...
  // TODO update this code to handle reading on each side if both_lux_sensors active
  if (both_lux_sensors == true && combine_lux_readings == false) {
    for (int i = 0; i < num_lux_sensors; i++) {
      if (lux_reading_timer[i] > lux_max_reading_delay && led_shdn_timer[i] > LED_SHDN_LEN) {
        colorWipe(0, 0, 0, i); // TODO make sure this should not be called with a mode
        led_shdn_timer[i] = 0;
      }
    }
  } else if (both_lux_sensors == true && combine_lux_readings == true) {
    if (lux_reading_timer[0] > lux_max_reading_delay && led_shdn_timer[0] > LED_SHDN_LEN) {
      colorWipe(0, 0, 0, 0); // TODO I dont think this uses combined lux ATM
      colorWipe(0, 0, 0, 1);
      led_shdn_timer[0] = 0;
      led_shdn_timer[1] = 0;
    }
  }
  else if (both_lux_sensors == false) {
    if (lux_reading_timer[0] > lux_max_reading_delay && led_shdn_timer[0] > LED_SHDN_LEN) {
      // if it is over the max amount of time for a reading, and a shutdown is not active,
      // then force a reading
      // turn the LEDs off
      colorWipe(0, 0, 0);
      //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
      led_shdn_timer[0] = 0;
    }
  }
}

void updateLuxMinMax() {
  // give the program some time to settle
  if (data_logging_active && millis() > 20000) {
    // front
    for (int i = 0; i < num_lux_sensors; i++) {
      if ((int)lux[i] > (int)max_lux_reading[i]) {
        max_lux_reading[i] = lux[i];
        writeShortToEEPROM(EEPROM_MAX_LUX_READING_FRONT + i , max_lux_reading[i]);
        Serial.print("logged new "); Serial.print(i);
        Serial.print(" max_lux_reading to EEPROM\t"); Serial.println(min_lux_reading[i]);
      } else if ((int)lux[0] < (int)min_lux_reading[i] && lux[i] != 0) {
        min_lux_reading[i] = lux[i];
        writeShortToEEPROM(EEPROM_MIN_LUX_READING_FRONT + i , min_lux_reading[i]);
        Serial.print("logged new "); Serial.print(i);
        Serial.print(" min_lux_reading to EEPROM\t"); Serial.println(min_lux_reading[i]);
      }
    }
    // combined
    if ((int)lux[2] > (int)max_lux_reading[2]) {
      max_lux_reading[2] = lux[2];
      writeShortToEEPROM(EEPROM_MAX_LUX_READING_COMBINED, max_lux_reading[2]);
      Serial.print("logged new combined max_lux_reading to EEPROM\t"); Serial.println(max_lux_reading[2]);
    } else if ((int)lux[2] < (int)min_lux_reading[2]) {
      min_lux_reading[2] = lux[2];
      writeShortToEEPROM(EEPROM_MIN_LUX_READING_COMBINED , min_lux_reading[2]);
      Serial.print("logged new combined min_lux_reading to EEPROM\t"); Serial.println(max_lux_reading[2]);
    }
  }
}

void readLuxSensorsWithout() {
  lux[0] = (lux[0] + veml[0].readLux()) * 0.5;
  // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
  if (lux[0] > 1000000) {
    Serial.print("lux "); Serial.print(0); Serial.print(" reading error: ");
    Serial.println(lux[0]);
    // take the reading again
    if (SMOOTH_LUX_READINGS && lux[0] != 0) {
      lux[0] = (lux[0] + veml[0].readLux()) * 0.5;
    } else {
      lux[0] = veml[0].readLux();
    }
  }
  // TODO souble check that all these numbers should be hard coded, same for the rear below
  // calculate the combined lux
  if (num_lux_sensors == 1) {
    // if there is only one lux sensor then make the rear lux and combined lux equal to the front lux
    lux[2] = lux[0];
    lux[1] = lux[0];
  } else {
    // otherwise set the combined lux to the average of the two lux readings
    lux[2] = (lux[0] + lux[1]) * 0.5;
  }
  // update the brightness scales
  for (int i = 0; i < num_lux_sensors + 1; i++) {
    brightness_scaler[0] = map(constrain(lux[0], MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  }
  updateLuxMinMax();
}

void readLuxSensors(uint8_t mode) {
  // mode 0 will read just the rear sensors, mode 1 will read just the rear sensors, mode 2 will read them both
  uint8_t _start;
  uint8_t _end;

  if (mode == 0 ) {
    _start = 0;
    _end = 1;
  } else if (mode == 1) {
    _start = 1;
    _end = 2;
  } else if (mode == 2) {
    _start = 0;
    _end = 2;
  } else {
    Serial.println("WARNING _start and _end are the same value");
    _start = 0;
    _end = 0;
  }

  for (int i = _start; i < _end; i++) {
    tcaselect(i);
    if (SMOOTH_LUX_READINGS && lux[i] != 0) {
      lux[i] = (lux[i] + veml[i].readLux()) * 0.5;
    } else {
      lux[i] = veml[i].readLux();
    }
    // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
    if (lux[i] > 1000000) {
      Serial.print("lux "); Serial.print(i); Serial.print(" reading error: ");
      Serial.println(lux[i]);
      // take the reading again
      if (SMOOTH_LUX_READINGS && lux[i] != 0) {
        lux[i] = (lux[i] + veml[i].readLux()) * 0.5;
      } else {
        lux[i] = veml[i].readLux();
      }
    }
    lux_reading_timer[i] = 0;
  }
  // TODO souble check that all these numbers should be hard coded, same for the rear below
  // calculate the combined lux
  if (num_lux_sensors == 1) {
    // if there is only one lux sensor then make the rear lux and combined lux equal to the front lux
    lux[2] = lux[0];
    lux[1] = lux[0];
  } else {
    // otherwise set the combined lux to the average of the two lux readings
    lux[2] = (lux[0] + lux[1]) * 0.5;
  }
  // update the brightness scales
  for (int i = 0; i < num_lux_sensors + 1; i++) {
    brightness_scaler[0] = map(constrain(lux[0], MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  }
  updateLuxMinMax();
}

void printLuxReadings() {
  if (PRINT_LUX_READINGS) {
    Serial.print("Lux: "); Serial.print(lux[2]); Serial.print(" = ");
    Serial.print(lux[0]); Serial.print(" ("); Serial.print(brightness_scaler[0]); Serial.print(")");
    Serial.print(" + "); Serial.print(lux[1]); Serial.print(" ("); Serial.print(brightness_scaler[1]); Serial.println(")");
  }
}

void clickFlashes() {
  for (int i = 0; i < num_channels; i++) {
    if (flash_delay[i] > 0) { // if there is time remaining in the flash it either needs to be turned on or the timer needs to increment
      // Serial.print("flash delay "); Serial.print(i); Serial.print(" : ");
      // Serial.println(flash_delay[i]);
      if (flash_on[i] == false) { //and the light is not currently on
        flash_on[i] = true; // turn the light on along with the flag
        if (stereo_audio && i == 0) {
          colorWipe(0, 0, 255, 0);
        } else if (stereo_audio && i == 1) {
          colorWipe(0, 0, 255, 1);
        } else {
          colorWipe(0, 0, 255, 2);
        }
        last_click_flash[i] = 0; // reset the elapsed millis variable as the light was just turned on
      } else {
        // if the light is already on
        // subtract the number of ms which have gone by since the last check
        // TODO optimize below lines
        flash_delay[i] = max(flash_delay[i] - last_click_flash[i], 0);
        last_click_flash[i] = 0;
        if (flash_delay[i] == 0) {
          // if there is no more time left then turn off the light and turn flag to false
          flash_on[i] = false;
          if (stereo_audio && i == 0) {
            colorWipe(0, 0, 0, 0);
            // Serial.println("Turned front flash off");
          } else if (stereo_audio && i == 1) {
            colorWipe(0, 0, 0, 1);
            // Serial.println("Turned rear flash off");
          } else {
            colorWipe(0, 0, 0, 2);
            // Serial.println("Turned both sides flash off");
          }
        }
      }
    } else { // there is no time in the flash delay timer
      if (flash_on[i] == true) {
        // if for some reason the flash is still on turn it off
        flash_on[i] = false;
        if (stereo_audio && i == 0) {
          colorWipe(0, 0, 0, 0);
          // Serial.println("Turned front flash off");
        }
        if (stereo_audio && i == 1) {
          colorWipe(0, 0, 0, 1);
          // Serial.println("Turned front flash off");
        } else {
          colorWipe(0, 0, 0, 0);
          colorWipe(0, 0, 0, 1);
          // Serial.println("Turned combined flash off");
        }
      }
      last_click_flash[i] = 0;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//                    Click Audio Functions
///////////////////////////////////////////////////////////////////////
void printClickStats() {
  // TODO update this to have a stereo option
  Serial.print("1000* Click | rms: ");
  Serial.print(click_rms_val[0] * 1000);
  Serial.print(" delta: ");
  Serial.print(click_rms_delta[0] * 1000);
  Serial.print(" peak: ");
  Serial.print(click_peak_val[0] * 1000);
  if (stereo_audio) {
    Serial.print(" R: ");
    Serial.print(click_rms_val[1] * 1000);
    Serial.print(" delta: ");
    Serial.print(click_rms_delta[1] * 1000);
    Serial.print(" peak: ");
    Serial.print(click_peak_val[1] * 1000);
  }
  Serial.println();
}

void calculateClickAudioFeatures() {
  for (int i = 0; i < num_channels; i++) {
    if (CLICK_FEATURE == "rms_delta" || CLICK_FEATURE == "all") {
      if (click_rms[i].available()) {
        last_click_rms_val[i] = click_rms_val[i];
        click_rms_val[i] = click_rms[i].read();
        click_rms_delta[i] = last_click_rms_val[i] - click_rms_val[i];
        if (click_rms_delta[i] > CLICK_RMS_DELTA_THRESH) {
          // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
          // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
          if (flash_delay[i] <= 0) {
            flash_delay[i] = MIN_CLICK_LENGTH;
            num_past_clicks[i]++;
            // TODO have a timeout for the clicks, perhaps 100ms or something like that, prevent the click from being detected if under that time...
            // Serial.print("-------------- CLICK "); Serial.print(i); Serial.print(" DETECTED --------------------- ");
            // Serial.print("rms_delta | ");
            // Serial.println(num_past_clicks[i]);
          } else if (flash_delay[i] < MAX_CLICK_LENGTH) {
            flash_delay[i]++;
          } else if (flash_delay[i] > MAX_CLICK_LENGTH) {
            flash_delay[i] = MAX_CLICK_LENGTH;
          }
        }
      }
    }
    if (CLICK_FEATURE == "peak_delta" || CLICK_FEATURE == "all") {
      if ( click_peak[i].available() ) {
        last_click_peak_val[i] = click_peak_val[i];
        click_peak_val[i] = click_peak[i].read();
        click_peak_delta[i] = last_click_peak_val[i]  - click_peak_val[i];
        if (click_peak_delta[i] > CLICK_PEAK_DELTA_THRESH) {
          // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
          // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
          if (flash_delay[i] <= 0) {
            flash_delay[i] = MIN_CLICK_LENGTH;
            num_past_clicks[i]++;
            // Serial.print("-------------- CLICK "); Serial.print(i); Serial.print(" DETECTED --------------------- ");
            // Serial.print("peak_delta | ");
            // Serial.println(num_past_clicks[i]);
          } else if (flash_delay[i] < MAX_CLICK_LENGTH) {
            flash_delay[i]++;
          } else if (flash_delay[i] > MAX_CLICK_LENGTH) {
            flash_delay[i] = MAX_CLICK_LENGTH;
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

void autoGainAdjust() {
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
  // dont run this logic unless the firmware has been running for one minute, any less time will result in erroneous values
  if (millis() < 60000) {
    return;
  }
  // if it has not been long enough since the last check then exit now
  if (last_auto_gain_adjustment < auto_gain_frequency) {
    return;
  };
  Serial.println("-------------------- Auto Gain Start ---------------------------");
  for (int i = 0; i < num_channels; i++) {
    Serial.print("num_past_clicks: ");
    Serial.println(num_past_clicks[i]);
    // first check is to see if there has been too many/few clicks detected
    double clicks_per_minute = ((double)num_past_clicks[i] * 60000) / (double)last_auto_gain_adjustment;
    double cost; // our cost variable
    Serial.print("clicks_per_minute: ");
    // printDouble(clicks_per_minute, 1000000);
    Serial.println(clicks_per_minute);
    // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
    if (clicks_per_minute == 0) {
      click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT;
      click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);
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
      click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;

      // make sure that the click_gain is not more than the max_click_gain
      click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);

      // update the click gain in the three gain stages
      updateClickGain();
    }
    // then check to see if there are too many clicks per-minute
    else if (clicks_per_minute > MAX_CLICKS_PER_MINUTE) {
      // determine a cost function... the higher the value the more the cost
      cost = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute);

      // adjust the click_gain
      click_gain[i] -= click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;

      // make sure that the gain is not set too low
      click_gain[i] = max(click_gain[i], MIN_CLICK_GAIN);

      // update the click gain in the three gain stages
      updateClickGain();
    }

    ///////////////////////////////////////////////////////////////
    // second check is to see if the song gain needs to be adjusted
    ///////////////////////////////////////////////////////////////

    // calculate the average peak values since the last auto-gain adjust
    double avg_song_peak = total_song_peaks[i] / num_song_peaks[i];
    Serial.print("\n--------- song "); Serial.print(i); Serial.println(" -------------");
    Serial.print("total_song_peaks ");
    Serial.println(total_song_peaks[i]);
    Serial.print("num_song_peaks ");
    Serial.println(num_song_peaks[i]);
    // if the avg value is more than the max...
    if (avg_song_peak > MAX_SONG_PEAK_AVG) {
      // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
      cost = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak);
      // calculate what the new song_gain will be
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] -= change;
      // ensure that what we have is not less than the min
      song_gain[i] = max(song_gain[i], MIN_SONG_GAIN);
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
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] += change;
      // ensure what we have is not less than the max...
      song_gain[i] = min(song_gain[i], MAX_SONG_GAIN);
      Serial.print("song gain increased by ");
      Serial.print(change);
      Serial.print(" ");
      Serial.println();
      updateSongGain();
    }

    ///////////////////////////////////////////////////////////////
    // last thing to do is reset the last_auto_gain_adjustment timer
    ///////////////////////////////////////////////////////////////
    total_song_peaks[i] = 0;
    num_song_peaks[i] = 0;
    num_past_clicks[i] = 0;
  }
  last_auto_gain_adjustment = 0;
  Serial.println(" ------------------------------------------------- ");
}

void updateSongGain() {
  for (int i = 0; i < num_channels; i++) {
    song_input_amp[i].gain(song_gain[i]);
    song_mid_amp[i].gain(song_gain[i]);
    song_post_amp[i].gain(song_gain[i]);
    Serial.print("updated song gain "); Serial.print(i);
    Serial.print(" : "); Serial.print(song_gain[i]);
    Serial.println();
  }
  Serial.println("------------------------");
}

void updateClickGain() {
  for (int i = 0; i < num_channels; i++) {
    if (click_gain[i] > click_gain_high[i]) {
      click_gain_high[i] = click_gain[i];
      writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT, click_gain_high[i]);
    }
    if (click_gain[i] < click_gain_low[i]) {
      click_gain_low[i] = click_gain[i];
      writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT, click_gain_low[i]);
    }
    click_input_amp[i].gain(click_gain[i]);
    click_mid_amp[i].gain(click_gain[i]);
    click_post_amp[i].gain(click_gain[i]);
    Serial.print("updated click "); Serial.print(i); Serial.print(" gain : ");
    Serial.print(click_gain[i]);
    Serial.println();
  }
}

void printSongStats() {
  if (PRINT_SONG_FEATURES) {
    for (int i = 0; i < num_channels; i++) {
      Serial.print("Song -- "); Serial.print(i); Serial.print(" | rms_weighted: ");
      Serial.print(song_rms_weighted[i]);
      Serial.print("\t peak: ");
      Serial.print(song_peak_weighted[i]);
      Serial.println();
    }
  }
}

void calculateSongAudioFeatures() {
  // TODO, rework the whole calculate weighted song brigtness to something that makes more sense
  // TODO rework to only calculate the features which are the features used
  for (int i = 0; i < num_channels; i++) {
    if (song_rms[i].available()) {
      double song_rms_val = song_rms[i].read();
      song_rms_weighted[i] = map(constrain((song_rms_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
    }

    if (song_peak[i].available()) {
      // random magiv 5 varaible needs to be better defined/tested TODO
      double song_peak_val = song_peak[i].read();
      song_peak_weighted[i] = map(constrain((song_peak_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
      num_song_peaks[i]++;
      total_song_peaks[i] += song_peak_val * 100;
      // will only print if flag is set
      printSongStats();
    }
  }
  // TODO, perhaps add another feature or two, perhaps an option for combining the two readings?
}

void songDisplay() {
  // if the click WHITE light is on, then ignore the song rms
  for (int i = 0; i < num_channels; i++) {
    if (flash_on[i] == false) {
      // TODO make a user control which allows for selection between RMs and peak
      if (SONG_FEATURE == "peak") {
        if (stereo_audio == false) {
          colorWipe(song_peak_weighted[i], 0, 0, 0);
          colorWipe(song_peak_weighted[i], 0, 0, 1);
        } else {
          colorWipe(song_peak_weighted[i], 0, 0, i);
          // colorWipeRear(song_peak_weighted_r, 0, 0);
        }
      } else if (SONG_FEATURE == "rms") {
        if (stereo_audio == false) {
          colorWipe(song_rms_weighted[i], 0, 0, 0);
          colorWipe(song_rms_weighted[i], 0, 0, 1);
        } else  {
          colorWipe(song_rms_weighted, 0, 0, i);
          // colorWipeRear(song_rms_weighted_r, 0, 0);
        }
      } else {
        Serial.print("ERROR: the SONG_FEATURE ");
        Serial.print(SONG_FEATURE);
        Serial.println(" is not a valid/implemented SONG_FEATURE");
      }
    }
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
void writeLongToEEPROM(int addr, uint32_t data) {
  Serial.print("\nWriting Long to EEPROM:\t");
  Serial.println(data);
  uint8_t b[4];
  for (int i = 0; i < 4; i++) {
    // EEPROM.write(addr + 1, 0);
    b[i] = data >> 8 * i;
    Serial.print("b ");
    Serial.print(i);
    Serial.print(" is : ");
    Serial.print(b[i], BIN);
    EEPROM.write(addr + i, b[i]);
    Serial.print( "\tread back from EEPROM: ");
    Serial.println(EEPROM.read(addr + i), BIN);
  }
  Serial.print("reading long from EEPROM : ");
  Serial.print(readLongFromEEPROM(addr));
  Serial.print("\t");
  Serial.println(readLongFromEEPROM(addr), BIN);
  Serial.println("- - - - - - - - - - - - - - - -");
}

uint32_t readLongFromEEPROM(int addr) {
  uint32_t data = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t n = EEPROM.read(addr + i) << 8 * i;
    data = n | data;
  }
  return data;
}

void writeShortToEEPROM(unsigned int addr, uint16_t data) {
  // store the dat with least significant bytes in lower index
  uint8_t lsb = data & 0x00FF;
  uint8_t msb = data >> 8;
  EEPROM.write(addr, lsb);
  EEPROM.write(addr + 1, msb);
}

uint16_t readShortFromEEPROM(unsigned int addr) {
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

    writeShortToEEPROM(EEPROM_TEST1, 99);
    writeShortToEEPROM(EEPROM_TEST2, 9999);
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

    EEPROM.write(EEPROM_SERIAL_ID, SERIAL_ID);
    Serial.print("logged serial number : ");
    Serial.println(SERIAL_ID);
    Serial.println("\tFinished logging setup configs to EEPROM");
    Serial.println("|||||||||||||||||||||||||||||||||||||||||");
  }
}

void writeAudioUsageToEEPROM(uint8_t used) {
  if (data_logging_active) {
    EEPROM.write(EEPROM_AUDIO_MEM_USAGE, used);
    Serial.println("logged audio memory usage to EEPROM");
  }
}

void updateRuntime() {
  if (last_runtime_update > RUNTIME_POLL_DELAY) {
    // Serial.print("time to update runtime");
    writeRuntimeToEEPROM();
    last_runtime_update = 0;
  }
}

void writeRuntimeToEEPROM() {
  if (data_logging_active) {
    unsigned long t = millis();
    writeLongToEEPROM(EEPROM_RUN_TIME, t);
    Serial.print("updated the runtime to EEPROM: ");
    Serial.println(t);
  }
}

void printEEPROMContents() {
  // todo add printing for the serial_id
  Serial.println("-------------------------------------");
  Serial.println("Printing EEPROM CONTENTS ...");
  Serial.println("-------------------------------------");
  Serial.print("bot serial number: ");
  Serial.println(EEPROM.read(EEPROM_SERIAL_ID));
  Serial.print("run time in ms: ");
  Serial.println(readLongFromEEPROM(EEPROM_RUN_TIME));
  Serial.print("run time in minutes: ");
  Serial.println((int)(readLongFromEEPROM(EEPROM_RUN_TIME) / 1000) / 60);
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
  Serial.print(readShortFromEEPROM(EEPROM_MIN_LUX_READING_FRONT)); Serial.print("\t");
  Serial.println(readShortFromEEPROM(EEPROM_MAX_LUX_READING_FRONT));
  Serial.print("min/max rear lux reading      : \t");
  Serial.print(readShortFromEEPROM(EEPROM_MIN_LUX_READING_REAR)); Serial.print("\t");
  Serial.println(readShortFromEEPROM(EEPROM_MAX_LUX_READING_REAR));
  Serial.print("min/max combined lux reading  : \t");
  Serial.print(readShortFromEEPROM(EEPROM_MIN_LUX_READING_COMBINED)); Serial.print("\t");
  Serial.println(readShortFromEEPROM(EEPROM_MAX_LUX_READING_COMBINED));

  Serial.println("\nTest Values\n||||||||||||||||||||||||||||||||||");
  Serial.print("should be equal to 99 = ");
  Serial.println(readShortFromEEPROM(EEPROM_TEST1));
  Serial.print("should be equal to 9999 = ");
  Serial.println(readShortFromEEPROM(EEPROM_TEST2));
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
  Serial.print("cicada_mode set to : "); Serial.println(cicada_mode);
  stereo_audio = digitalRead(JMP2_PIN);
  num_channels = stereo_audio + 1; // the number of channels we will be using
  Serial.print("stereo_audio set to : "); Serial.println(stereo_audio);
  both_lux_sensors = digitalRead(JMP3_PIN);
  num_lux_sensors = both_lux_sensors + 1;
  Serial.print("both_lux_sensors set to : "); Serial.println(both_lux_sensors);
  combine_lux_readings = digitalRead(JMP4_PIN);
  Serial.print("combine_lux_readings set to : "); Serial.println(combine_lux_readings);
  gain_adjust_active = digitalRead(JMP5_PIN);
  Serial.print("gain_adjust_active set to : "); Serial.println(gain_adjust_active);

  data_logging_active = digitalRead(JMP6_PIN);
  Serial.print("data_logging_active set to : "); Serial.println(data_logging_active);
  Serial.println("Finished reading jumpers\n------------------------------------");
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
  Serial.println("-------------------------------");
  Serial.println("Starting Cicada Mode Setup Loop");

  ////////////// Audio ////////////
  AudioMemory(AUDIO_MEMORY);
  // Audio for the click channel...

  Serial.print("Setting up the audio channel settings : \n");// Serial.println(i);
  click_biquad1.setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad1.setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad1.setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad1.setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);
  click_biquad2.setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad2.setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad2.setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad2.setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);

  click_biquad21.setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad21.setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad21.setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad21.setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);
  click_biquad11.setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad11.setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad11.setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad11.setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);

  click_input_amp1.gain(click_gain[0]);
  click_mid_amp1.gain(click_gain[0]);
  click_post_amp1.gain(click_gain[0]);
  click_input_amp2.gain(click_gain[0]);
  click_mid_amp2.gain(click_gain[0]);
  click_post_amp2.gain(click_gain[0]);

  // Audio for the song channel...
  song_biquad1.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);
  song_biquad2.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);

  song_biquad11.setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad11.setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad11.setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad11.setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);
  song_biquad21.setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad21.setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad21.setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad21.setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);

  song_input_amp1.gain(song_gain[0]);
  song_mid_amp1.gain(song_gain[0]);
  song_post_amp1.gain(song_gain[0]);
  song_input_amp2.gain(song_gain[0]);
  song_mid_amp2.gain(song_gain[0]);
  song_post_amp2.gain(song_gain[0]);
  delay(1000);
  
  Serial.println("Audio setup loop finished");

  /////////////////////////////////
  // Start the LEDs ///////////////
  /////////////////////////////////
  Serial.println("setting up LEDs");
  leds.begin();
  delay(250);
  colorWipe(0, 0, 0, 0); // turn off the LEDs
  colorWipe(0, 0, 0, 1); // turn off the LEDs

  /////////////////////////////////
  // VEML sensors through TCA9543A
  /////////////////////////////////
  setupVEMLthroughTCA();
  delay(200);
  Serial.print("starting click gain: ");
  Serial.println(click_gain[0]);
  Serial.print("starting song gain: ");
  Serial.println(song_gain[0]);
  Serial.println("Finshed cicada Setup Loop");
  Serial.println("------------------");
  delay(200);
}

void setup() {
  Serial.begin(57600);
  delay(1000);
  if (PRINT_EEPROM_CONTENTS == true) {
    Serial.println("preparing to print EEPROM contents");
    delay(4000);
    printEEPROMContents();
  }
  if (JUMPERS_POPULATED) {
    readJumpers();
  }
  if (cicada_mode) {
    Serial.println("Cicada mode is active");
    ////////////// Audio ////////////
    /*AudioMemory(AUDIO_MEMORY);
      // Audio for the click channel...

      for (int i = 0; i < num_channels; i++ ) {
      Serial.print("Setting up the audio channel settings : ");Serial.println(i);
      click_biquad[i].setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
      click_biquad[i].setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
      click_biquad[i].setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
      click_biquad[i].setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);

      click_biquad2[i].setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
      click_biquad2[i].setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
      click_biquad2[i].setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
      click_biquad2[i].setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);

      click_input_amp[i].gain(click_gain[i]);
      click_mid_amp[i].gain(click_gain[i]);
      click_post_amp[i].gain(click_gain[i]);

      // Audio for the song channel...
      song_biquad[i].setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
      song_biquad[i].setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
      song_biquad[i].setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
      song_biquad[i].setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);

      song_biquad2[i].setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
      song_biquad2[i].setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
      song_biquad2[i].setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
      song_biquad2[i].setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);

      song_input_amp[i].gain(song_gain[i]);
      song_mid_amp[i].gain(song_gain[i]);
      song_post_amp[i].gain(song_gain[i]);
      }*/
    cicadaSetup();
  } else {
    while (1) {
      Serial.println("sorry the bird mode is not yet implemented");
      delay(1000);
    }
  }
  writeSetupConfigsToEEPROM();
  // todo make this proper
  if (I2C_MULTI) {
    luxSetupCal();
  }
}

// todo move me to the correct place
void luxSetupCal() {
  // todo change this function so it takes the average of these readings
  Serial.println("------------------------");
  Serial.println("Starting Lux Calibration");
  double lux_tot[3];
  for (int i = 0; i < 10; i++) {
    Serial.print(i);
    Serial.print("\t");
    lux_reading_timer[0] = lux_min_reading_delay;
    lux_reading_timer[1] = lux_min_reading_delay;
    delay(100);
    readLuxSensors(2); // todo change this to not be hard coded
    printLuxReadings();
    if (i > 4) {
      for (int iii = 0; iii < 3; iii++) {
        lux_tot[iii] += lux[iii];
      }
    }
  }
  // when we have the first 10 readings
  Serial.print("Average lux readings : ");
  for (int ii = 0; ii < 3; ii++) {
    lux[ii] = lux_tot[ii] / 5;
    Serial.print(lux[ii]);
    Serial.print("\t");
  }
  Serial.println("\nLux calibration finished");
  Serial.println("------------------------\n");
}

void updateAll() {
  checkAudioUsage();
  autoGainAdjust(); // will call rear as well if in stereo mode
  updateRuntime();
}

void loop() {
  if (cicada_mode) {
    cicadaLoop();
  }
  // Lux //////////////////
  if (checkLuxSensors()) {
    printLuxReadings();
  }
  updateAll();
}
