#include <EEPROM.h>

/*
 * Test firmware for the Moth bot
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

#define BOARD_REVISION 1

/////////////////////////////// NeoP ////////////////////////////////////
#define MAX_BRIGHTNESS 250
#define MIN_BRIGHTNESS 10
// for the clicks
#define MAX_CLICK_LENGTH 40
#define MIN_CLICK_LENGTH 10

#define NUM_LED 10
#define LED_PIN 5

#define RED    0x160000
#define GREEN  0x001600
// changed fro 16 to 46
#define BLUE   0x000046
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010

byte drawingMemory[NUM_LED * 3];       //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LED * 12]; // 12 bytes per LED
// to keep track of the flash
uint16_t flash_delay = 0;
bool flash_on = false;
elapsedMillis last_click_flash = 0;
// ths timer ensures that the LEDs are not turned on when they are in shutdown mode to allow for an accurate LUX reading
elapsedMillis led_shdn_timer = 0;
// the amount of time that the LEDs need to be shutdown to allow lux sensors to get an accurate reading
#define LED_SHDN_LEN 40
// this boolean is set to false if the LEDs are turned off and true if the leds are on...
bool leds_on = false;
// this timer keeps track of how long the LEDs are turned off, it will reset when colorWipe is called with a color of 0
elapsedMillis led_off_len;

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

/////////////////////////////// LUX ////////////////////////////////////
#define FRONT_LUX_INSTALLED true
#define REAR_LUX_INSTALLED true

elapsedMillis last_lux_reading = 0;
unsigned long lux_max_reading_delay = 1000*60*0.5;     // every 6 minutes
unsigned long lux_min_reading_delay = 1000*60*0.25; // 1 minute
double lux_min = 10;
double lux_max = 500;

double brightness_scaler_front = 1.0; // TODO expand to a front and back brightness scaler?
double brightness_scaler_rear = 1.0; // TODO expand to a front and back brightness scaler?
double front_lux;
double rear_lux;

#ifdef FRONT_LUX_INSTALLED
Adafruit_VEML7700 veml_front = Adafruit_VEML7700();
#endif
#ifdef REAR_LUX_INSTALLED
Adafruit_VEML7700 veml_rear = Adafruit_VEML7700();
#endif

////////////////////////////// AUDIO ///////////////////////////////////
#define USB_OUTPUT 1
// One click per ten minutes
#define MIN_CLICKS_PER_MINUTE 1.0
#define MAX_CLICKS_PER_MINUTE 40.0

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.01
#define MAX_SONG_PEAK_AVG 0.15

#define MIN_CLICK_GAIN 0.5
#define MAX_CLICK_GAIN 24.0
#define MIN_SONG_GAIN 0.5
#define MAX_SONG_GAIN 20

#define AUDIO_MEMORY 24

// gain levels
double song_gain = 4; // starting song gain level
double click_gain = 12; // starting click gain level

/////////////////////////////////// Audio Library /////////////////////////////////////////////

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
// AudioInputUSB            usb2;           //xy=55,258.00000190734863
AudioOutputUSB           usb1;           //xy=1024.000015258789,129.00000190734863
AudioConnection          patchCord13(song_post_amp, 0, usb1, 1);
AudioConnection          patchCord18(click_post_amp, 0, usb1, 0);
#endif

double click_rms_delta = 0.0;
double click_rms_val = 0.0;
double last_click_rms_val = 0.0;
double click_peak_val = 0.0;

double song_rms_val = 0.0;
double song_peak_val = 0.0;
uint8_t song_rms_weighted = 0; // 0 -255 depending on the RMS of the song band...
uint8_t song_peak_weighted = 0;

elapsedMillis last_usage_print = 0;

/////////////// Auto Gain Variables ///////////////////
double total_song_peaks;
unsigned long num_song_peaks;
uint16_t num_past_clicks; // number of clicks which has occurred since boot

const uint32_t auto_gain_frequency = 1000*60*5;// how often to calculate auto-gain (in ms)
elapsedMillis last_auto_gain_adjustment; // the time in which the last auto_gain_was_calculated
// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change 
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0 
// with a max/min value of 1.0 / 3.0.
#define MAX_GAIN_ADJUSTMENT 0.125 

///////////////////////////////////////////////////////////////////////
//                       TCA9532A I2C bus expander
///////////////////////////////////////////////////////////////////////

#define TCAADDR 0x70
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setupVEMLthroughTCA() {
  Wire.begin();
  Serial.println("\nTCAScanner ready!");
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);
    for (uint8_t addr = 0; addr <= 127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Serial.print("Found I2C 0x"); Serial.println(addr, HEX);
      }
    }
  }
  
  #ifdef FRONT_LUX_INSTALLED
  tcaselect(0);
  if (!veml_front.begin()) {
    Serial.println("VEML Front not found");
    while (1);
  }
  Serial.println("VEML Front found");
  veml_front.setGain(VEML7700_GAIN_1);
  veml_front.setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
  #endif

  #ifdef REAR_LUX_INSTALLED
  tcaselect(1);
  if (!veml_rear.begin()) {
    Serial.println("VEML Rear not found");
    while (1);
  }
  Serial.println("VEML Rear found");
  veml_rear.setGain(VEML7700_GAIN_1);
  veml_rear.setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
  #endif
}

///////////////////////////////////////////////////////////////////////
//                    Neopixel Related Functions
///////////////////////////////////////////////////////////////////////

uint8_t calculateWeightedSongBrightness(double val) {
  uint8_t bright = map(constrain(val, 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
  // Serial.print(val);
  // Serial.print("\t");
  // Serial.println(bright);
  return bright;
}

void colorWipe(int color, int wait) {
  if (led_shdn_timer < LED_SHDN_LEN) {
    // if the LEDs are in shutdown mode than simply exit without changing the LEDs
    return;
  }
  for (int i = 0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
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

uint32_t scaleColorToLux(uint8_t red, uint8_t green, uint8_t blue) {
  // TODO change code to account for front and back sides
  uint32_t color = 0;
  red = red * brightness_scaler_front;
  green = green * brightness_scaler_front;
  blue = blue * brightness_scaler_front;
  color = (red << 16) + (green << 8) + (blue);
  return color;
}

uint8_t checkLuxSensors() {
   // Return a 1 if the lux sensors are read and a 0 if they are not
   // if the LEDs have been off, on their own regard, for 40ms or longer...
   // and it has been long-enough to warrent a new reading
   if (leds_on == false && led_off_len >= LED_SHDN_LEN && last_lux_reading > lux_min_reading_delay) {
    _readLuxSensors();
    // Serial.println("Sneaky LUX READING, LEDs off for 40ms");
    return 1;
   }
   // if it has been longer than the "lux_max_reading_delay" force a new reading...
  if (last_lux_reading > lux_max_reading_delay && led_shdn_timer > LED_SHDN_LEN) {
    // turn the LEDs off
    colorWipe(0x000000, 0);
    //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
    led_shdn_timer = 0;
    // Serial.println("Standard Lux Reading");
  }
  return 0;
}

void _readLuxSensors() {
    #ifdef FRONT_LUX_INSTALLED
    tcaselect(0);
    front_lux = veml_front.readLux();
    if (front_lux > 10000000) {
      Serial.print("front lux reading error: ");
      Serial.println(front_lux);
      front_lux = veml_front.readLux();
    }
    brightness_scaler_front = map(constrain(front_lux, lux_min, lux_max), lux_min, lux_max, 50, 250)/250;
    #endif
    #ifdef REAR_LUX_INSTALLED
    tcaselect(1);
    rear_lux = veml_rear.readLux();
    if (rear_lux > 10000000) {
      Serial.print("rear lux reading error: ");
      Serial.println(rear_lux);
      rear_lux = veml_rear.readLux();
    }
    brightness_scaler_rear = map(constrain(rear_lux, lux_min, lux_max), lux_min, lux_max, 50, 250)/250;
    #endif
    last_lux_reading = 0;
}

void printLuxReadings() {
  Serial.print("Lux: "); Serial.print(front_lux); Serial.print(" ("); Serial.print(brightness_scaler_front);Serial.print(")");
  Serial.print(" - "); Serial.print(rear_lux);Serial.print(" ("); Serial.print(brightness_scaler_rear);Serial.println(")");
}

void clickFlashes() {
  // check to see if light is already on
  // TODO ////
  if (flash_delay > 0) { // if there is time remaining in the flash
    if (flash_on == false) { //and the light is not currently on
      flash_on = true; // turn the light on along with the flag
      colorWipe(scaleColorToLux(0, 0, 255), 0);
      last_click_flash = 0; // reset the elapsed millis variable as the light was just turned on
      // Serial.println("Turned WHITE flash on");
    } else {
      // if the light is already on
      // subtract the number of ms which have gone by since the last check
      // TODO optimize below lines
      flash_delay = flash_delay - last_click_flash;
      if (flash_delay < 0) {
        flash_delay = 0;
      }
      last_click_flash = 0;
      if (flash_delay == 0) {
        // if there is no more time left then turn off the light and turn flag to false
        flash_on = false;
        colorWipe(0x000000, 0);
        // Serial.println("Turned flash off");
      }
    }
  } else {
    last_click_flash = 0;
  }
}

///////////////////////////////////////////////////////////////////////
//                    Click Audio Functions
///////////////////////////////////////////////////////////////////////
void printClickStats() {
  if (click_rms.available() && click_peak.available()) {
    Serial.print("Click | rms: ");
    Serial.print(click_rms_val * 1000);
    Serial.print(" delta: ");
    Serial.print(click_rms_delta * 1000);
    Serial.print("\t peak: ");
    Serial.println(click_peak_val * 1000);
  }
}

void calculateClickAudioFeatures() {
  if (click_rms.available()) {
    last_click_rms_val = click_rms_val;
    click_rms_val = click_rms.read();
    click_rms_delta = last_click_rms_val - click_rms_val;
  }

  if ( click_peak.available() ) {
    click_peak_val = click_peak.read();
  }
  // need to update 0.03 magic number 
  if (click_rms_delta > 0.03) {
    // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
    // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
    if (flash_delay <= 0) {
      flash_delay = MIN_CLICK_LENGTH;
      num_past_clicks++;
      Serial.print("-------------- CLICK DETECTED --------------------- ");
      Serial.println(num_past_clicks);
    } else if (flash_delay < MAX_CLICK_LENGTH) {
      flash_delay++;
    } else if (flash_delay > MAX_CLICK_LENGTH) {
      flash_delay = MAX_CLICK_LENGTH;
    }
    // Serial.println(flash_delay);
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
void printDouble( double val, unsigned int precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

   Serial.print (int(val));  //prints the int part
   Serial.print("."); // print the decimal point
   unsigned int frac;
   if(val >= 0)
     frac = (val - int(val)) * precision;
   else
      frac = (int(val)- val ) * precision;
   int frac1 = frac;
   while( frac1 /= 10 )
       precision /= 10;
   precision /= 10;
   while(  precision /= 10)
       Serial.print("0");

   Serial.println(frac,DEC) ;
}

void autoGainAdjust() {
  /* The purpose of this function is to determine the gain level for both the "song" and the
   *   "click" bands. The function will run for a number of milliseconds equal to the variable len
   *  passed into it. (usually around 5 or 10 seconds)

     It will poll the RMS and Peak for each band for this period of time and determine what the max
     and average values are.

     It will then adjust the master gain levels, click_gain and song_gain to meet target values
     target_click_rms and target_song_rms.
   *
   *  This function should be run in the setup loop when the teensy is booting as well as a few times an
   *  hour to adjust the gain levels.
   *
   */
  // if it has not been long enough since the last check then exit now
  if (last_auto_gain_adjustment < auto_gain_frequency){ return; };
  Serial.print("num_past_clicks: ");
  Serial.println(num_past_clicks);
  Serial.print("last_auto_gain_adjustment: ");
  Serial.println(last_auto_gain_adjustment);
  // first check is to see if there has been too many/few clicks detected
  double clicks_per_minute = ((double)num_past_clicks*60000)/(double)last_auto_gain_adjustment;
  double cost; // our cost variable
  Serial.print("clicks_per_minute: ");
  // printDouble(clicks_per_minute, 1000000);
  Serial.println(clicks_per_minute);
  // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
  if (clicks_per_minute == 0){
      click_gain += click_gain*MAX_GAIN_ADJUSTMENT;
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
  // if the avg value is more than the max...
  if (avg_song_peak > MAX_SONG_PEAK_AVG) {
      // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
      cost = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak);
      // calculate what the new song_gain will be
      song_gain -= song_gain * MAX_GAIN_ADJUSTMENT * cost;
      // ensure that what we have is not less than the min
      song_gain = max(song_gain, MIN_SONG_GAIN);
      updateSongGain();
  }
  // if the average value is less than the min....
  else if (avg_song_peak < MIN_SONG_PEAK_AVG) {
      // calculate cost between 0 and 1 with higher cost resulting in higher gain attenuation
      cost = 1.0 - (MIN_SONG_PEAK_AVG / avg_song_peak);
      // calculate the new song gain
      song_gain += song_gain * MAX_GAIN_ADJUSTMENT * cost;
      // ensure what we have is not less than the max...
      song_gain = min(song_gain, MAX_SONG_GAIN);
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
  song_input_amp.gain(click_gain);
  song_mid_amp.gain(click_gain);
  song_post_amp.gain(click_gain);
  Serial.print("updated song gain : ");
  Serial.println(song_gain);
}

void updateClickGain() {
  click_input_amp.gain(click_gain);
  click_mid_amp.gain(click_gain);
  click_post_amp.gain(click_gain);
  Serial.print("updated click gain : ");
  Serial.println(click_gain);
}

void printSongStats() {
    Serial.print("*1000 Song | rms: ");
    Serial.print(song_rms_val * 1000);
    Serial.print("\t peak: ");
    Serial.println(song_peak_val * 1000);
}

void calculateSongAudioFeatures() {
  if (song_rms.available()) {
    song_rms_val = song_rms.read();
    song_rms_weighted = calculateWeightedSongBrightness(song_rms_val * 5);
  }

  if (song_peak.available()) {
    // random magiv 5 varaible needs to be better defined/tested TODO
    song_peak_val = song_peak.read();
    song_peak_weighted = calculateWeightedSongBrightness(song_peak_val * 5);
    num_song_peaks++;
    total_song_peaks += song_peak_val;
  }
}

void songDisplay() {
  // if the click WHITE light is on, then ignore the song rms
  if (flash_on == true) {
    return;
  }
  // if not then adjust the brightness of the LEDs according to the RMS
  uint32_t color = 0;
  // TODO add compensation to the brightness according to the ambiant brightness
  // TODO calculate the song_rms_weighted
  // color = song_peak_weighted << 16; // TODO make sure this shifting is correct
  colorWipe(scaleColorToLux(song_peak_weighted, 0, 0), 0);
}

///////////////////////////////////////////////////////////////////////
//                    General Purpose Audio Functions
///////////////////////////////////////////////////////////////////////

void printAudioUsage() {
  // print the audio usage every second or so
  if (last_usage_print > 5000) {
    Serial.print("memory usage: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" out of ");
    Serial.println(AUDIO_MEMORY);
    last_usage_print = 0;
    AudioMemoryUsageMaxReset();
  }
}

///////////////////////////////////////////////////////////////////////
//                    Setup and Main Loops
///////////////////////////////////////////////////////////////////////

void setup() {
  /////////////////////////////////
  // Start Serial and let things settle
  /////////////////////////////////
  Serial.begin(57600);
  // delay(1000);
  // turn off the onboard LED to conserve power
  // pinMode(13, OUTPUT);
  // digitalWrite(13, LOW);

  ////////////// Audio ////////////
  AudioMemory(AUDIO_MEMORY);
  // Audio for the click channel...

  click_biquad.setHighpass(0, 1200, 0.95);
  click_biquad.setHighpass(1, 1200, 0.95);
  click_biquad.setHighpass(2, 1200, 0.95);
  // click_biquad.setLowShelf(2, 1200, -24);
  click_biquad.setLowShelf(3, 1200, -24);

  click_biquad2.setLowpass(0, 2500, 0.95);
  click_biquad2.setLowpass(1, 2500, 0.95);
  click_biquad2.setLowpass(2, 2500, 0.95);
  // click_biquad2.setHighShelf(2, 2500, -24);
  click_biquad2.setHighShelf(3, 2500, -24);

  click_input_amp.gain(click_gain);
  click_mid_amp.gain(click_gain);
  click_post_amp.gain(click_gain);

  // Audio for the song channel...
  song_biquad.setHighpass(0, 3500, 0.75);
  song_biquad.setHighpass(1, 3500, 0.75);
  song_biquad.setHighpass(2, 3500, 0.75);
  song_biquad.setLowShelf(3, 3500, -12);

  song_biquad2.setLowpass(0, 14000, 0.75);
  song_biquad2.setLowpass(1, 14000, 0.75);
  song_biquad2.setLowpass(2, 14000, 0.75);
  song_biquad2.setHighShelf(3, 14000, -12);

  song_input_amp.gain(song_gain);
  song_mid_amp.gain(song_gain);
  song_post_amp.gain(song_gain);
  
  /////////////////////////////////
  // Start the LEDs ///////////////
  /////////////////////////////////
  // Serial.println("setting up LEDs");
  leds.begin();
  delay(250);
  colorWipe(0,0); // turn off the LEDs 
  /////////////////////////////////
  // VEML sensors through TCA9543A
  /////////////////////////////////
  setupVEMLthroughTCA();
  delay(500);
  Serial.print("starting click gain: ");
  Serial.println(click_gain);
  Serial.print("starting song gain: ");
  Serial.println(song_gain);
  Serial.println("Finshed Setup Loop");
  Serial.println("------------------");
  delay(500);
}

void loop() {
  // SONG /////////////////
  calculateSongAudioFeatures();
  // printSongStats();
  songDisplay();
  
  // Click ////////////////
  calculateClickAudioFeatures();
  // printClickStats();
  clickFlashes();

  // Lux //////////////////
  if (checkLuxSensors()) {
    printLuxReadings();
  }
  // printAudioUsage(); // print audio usage every five seconds
  autoGainAdjust();
}
