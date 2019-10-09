#include <WS2812Serial.h>
#include <elapsedMillis.h>
const int numled = 12;
const int pin = 5;

// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33

byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

/*
#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
*/

// Less intense...

#define RED    0x160000
#define GREEN  0x001600
// changed fro 16 to 46
#define BLUE   0x000046
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010


// to keep track of the flash
uint16_t flash_delay = 0;
bool flash_on = false;
elapsedMillis last_click_flash;
unsigned long click_length = 20;
unsigned long max_click_length = 60;

////////////////////////////////////////////////////////////////////////////////
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=55,122.00000190734863
AudioInputUSB            usb2;           //xy=55,258.00000190734863
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
AudioOutputUSB           usb1;           //xy=1024.000015258789,129.00000190734863
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
AudioConnection          patchCord13(song_post_amp, 0, usb1, 1);
AudioConnection          patchCord14(song_post_amp, song_rms);
AudioConnection          patchCord15(song_post_amp, song_peak);
AudioConnection          patchCord16(click_post_amp, click_rms);
AudioConnection          patchCord17(click_post_amp, click_peak);
AudioConnection          patchCord18(click_post_amp, 0, usb1, 0);
// GUItool: end automatically generated code

/*
   TODO
   -----------
   Map the rms of the "song" spectrum to LED brightness
   When click occurs, change LED color to Blue/White instead of red

   Determine how to detect clicks (perhaps by comparing either the "peak" of that channel
   or perhaps by comparing its peak with another band?
*/

double click_rms_delta = 0.0;
double click_rms_val = 0.0;
double last_click_rms_val = 0.0;
double click_peak_val = 0.0;

double song_rms_val = 0.0;
double song_peak_val = 0.0;
uint8_t song_rms_weighted = 0; // 0 -255 depending on the RMS of the song band...
uint8_t song_peak_weighted = 0;

elapsedMillis lastUsagePrint = 0;
#define AUDIO_MEMORY 16

float song_gain = 4;
float click_gain = 10;

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

  if (click_rms_delta > 0.02) {
    Serial.print("-------------- CLICK DETECTED --------------------- ");
    // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
    if (flash_delay <= 0) {
      flash_delay += click_length;
    } else if (flash_delay < max_click_length) {
      flash_delay++;
    }
    Serial.println(flash_delay);
  }
}

void clickFlashes() {
  // check to see if light is already on
  // TODO ////
  if (flash_delay > 0) { // if there is time remaining in the flash
    if (flash_on == false) { //and the light is not currently on
      flash_on = true; // turn the light on along with the flag
      colorWipe(BLUE, 0);
      last_click_flash = 0; // reset the elapsed millis variable as the light was just turned on
      // Serial.println("Turned WHITE flash on");
    } else {
      // if the light is already on
      // subtract the number of ms which have gone by since the last check
      // TODO optimize below lines
      flash_delay = flash_delay - last_click_flash;
      if (flash_delay < 0) {flash_delay = 0;}
            
      last_click_flash = 0;
      if (flash_delay == 0) {
        // if there is no more time left then turn off the light and turn flag to false
        flash_on = false;
        colorWipe(0x000000, 0);
        Serial.println("Turned flash off");
      }
    }
  } else {
    last_click_flash = 0;
  }
}


///////////////////////////////////////////////////////////////////////
//                    Song Audio Functions
///////////////////////////////////////////////////////////////////////

/* TODO need a watchdog program which will run adjust gain if certain conditions are met.
 *  One such condition is if too many click events are detected within a certain period of time,
 *  Another condition is if too few click everts are detected within a certain period of time,
 *  Another condition is if the red led brightness is too low, for an extended period of time
 *  Another condition is if the red led brightness is too high over an extended period of time.
*/

/* TODO - need a function which will poll the lux sensor and then adjust the brightness of the
 *  LEDs.
 *  
 * Also there should be a watchdog which every 10 minutes (or so) will re-poll the lux sensor 
 * when the LEDs are turned off to readjst the brightness scales.
 * 
 */

double target_song_rms = ?;
double target_click_rms = ?;

void adjustGain(unsigned long len) {
  /* The purpose of this function is to determine the gain level for both the "song" and the
   *  "click" bands. The function will run for a number of milliseconds equal to the variable len
   *  passed into it. (usually around 5 or 10 seconds)
   * 
   * It will poll the RMS and Peak for each band for this period of time and determine what the max 
   * and average values are.
   * 
   * It will then adjust the master gain levels, click_gain and song_gain to meet target values 
   * target_click_rms and target_song_rms.
   * 
   * This function should be run in the setup loop when the teensy is booting as well as a few times an 
   * hour to adjust the gain levels.
   * 
  */
  elapsedMillis start_time = 0;
  while (start_time < len) {
    
  }
}

void printSongStats() {
  if (song_rms.available() && song_peak.available()) {
    Serial.print("*1000 Song | rms: ");
    Serial.print(song_rms_val * 1000);
    Serial.print("\t peak: ");
    Serial.println(song_peak_val * 1000);
  }
}

void calculateSongAudioFeatures() {
  if (song_rms.available()) {
    song_rms_val = song_rms.read();
    song_rms_weighted = calculateWeightedSongBrightness(song_rms_val*5);
  }

  if (song_peak.available()) {
    song_peak_val = song_peak.read();
    song_peak_weighted = calculateWeightedSongBrightness(song_peak_val*5);
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
  color = song_peak_weighted << 16; // TODO make sure this shifting is correct
  colorWipe(color, 0);
}

///////////////////////////////////////////////////////////////////////
//                    General Purpose Audio Functions
///////////////////////////////////////////////////////////////////////

void printAudioUsage() {
  // print the audio usage every second or so
  if (lastUsagePrint > 5000) {
    Serial.print("memory usage: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.print(" out of ");
    Serial.println(AUDIO_MEMORY);
    lastUsagePrint = 0;
    AudioMemoryUsageMaxReset();
  }
}

///////////////////////////////////////////////////////////////////////
//                    Neopixel Related Functions
///////////////////////////////////////////////////////////////////////

uint8_t calculateWeightedSongBrightness(double val) {
  uint8_t bright = map(constrain(val, 0, 1.0), 0, 1.0, 0, 255);
  // Serial.print(val);
  // Serial.print("\t");
  // Serial.println(bright);
  return bright;
}

void colorWipe(int color, int wait) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  } 
}

///////////////////////////////////////////////////////////////////////
//                    Setup and Main Loops
///////////////////////////////////////////////////////////////////////

void setup() {
  AudioMemory(AUDIO_MEMORY);
  
  /////////////////////////////////
  // Audio for the click channel...
  ///////////////////////////////// 
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
  
  ////////////////////////////////
  // Audio for the song channel...
  ////////////////////////////////
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
  leds.begin();

  /////////////////////////////////
  // Start Serial and let things settle
  /////////////////////////////////
  delay(1000);
  Serial.begin(57600);
  Serial.println("Finshed Setup Loop");
  Serial.println("------------------");
  delay(500);
}

void loop() {
  calculateClickAudioFeatures();
  calculateSongAudioFeatures();
  clickFlashes();
  songDisplay();
  // printClickStats();
  // printAudioUsage(); // print audio usage every five seconds
}
