#ifndef __MODE_CICADA_H__
#define __MODE_CICADA_H__
#include <Audio.h>
#include <WS2812Serial.h>
#include "Datalog.h"
#include "Configuration.h"
#include "Neos.h"
#include "Lux.h"

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);
NeoGroup neos[2] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 10, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

Lux lux_sensors[NUM_LUX_SENSORS] = {
  Lux(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  Lux(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

/////////////////////////////// Lux Sensors //////////////////////////////
double combined_lux;
double combined_min_lux_reading;
double combined_max_lux_reading;

////////////////////////////// Audio ////////////////////////////////////
// audio usage loggings
uint8_t audio_usage_max = 0;
elapsedMillis last_usage_print = 0;// for keeping track of audio memory usage

elapsedMillis last_runtime_update;
elapsedMillis ten_second_timer;
bool front_mic_active = FRONT_MICROPHONE_INSTALLED;
bool rear_mic_active = REAR_MICROPHONE_INSTALLED;

elapsedMillis last_auto_gain_adjustment; // the time in which the last auto_gain_was_calculated

////////////////////////// EEPROM //////////////////////////////////////
unsigned int cpm_eeprom_idx = EEPROM_CPM_LOG_START;
const long EEPROM_CPM_LOG_END = EEPROM_CPM_LOG_START + (4 * 2 * EEPROM_CPM_LOG_LENGTH);//4 bits to double, front and rear, log length
const long EEPROM_LUX_LOG_END = EEPROM_LUX_LOG_START + (4 * 2 * EEPROM_LUX_LOG_LENGTH);//4 bits to double, front and rear, log length

elapsedMillis log_timer;
const long LOG_POLLING_RATE = (long)((double)LOG_TIME_FRAME / (double)EEPROM_LUX_LOG_LENGTH * 2.0);

unsigned int lux_eeprom_idx = EEPROM_LUX_LOG_START;
int lux_total[2];
int lux_readings[2];

///////////////////////// Cicada Mode ////////////////////////////////////
// #include "MothConfig.h"
#include <Audio.h>

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

// keeping track of clicks //////
long total_clicks_detected[2] = {0, 0}; // number of clicks which has occurred since boot
long num_past_clicks[2];            // number of clicks since last auto-gain adjustment
long num_cpm_clicks[2];

// click gain //////////////////
double click_gain[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN}; // starting click gain level
double click_gain_min[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN};
double click_gain_max[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN};

// click rms ///////////////////
double click_rms_val[2] = {0.0, 0.0};
double last_click_rms_val[2] = {0.0, 0.0};
double click_rms_delta[2] = {0.0, 0.0};

// click peak //////////////////
double click_peak_val[2] = {0.0, 0.0};
double last_click_peak_val[2] = {0.0, 0.0};
double click_peak_delta[2] = {0.0, 0.0};

////////////////////////////////
double song_gain[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};   // starting song gain level
double song_gain_min[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};
double song_gain_max[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};

// song rms
uint8_t song_rms_weighted[2] = {0, 0};  // 0 -255 depending on the RMS of the song band...

// song peak
uint8_t song_peak_weighted[2] = {0, 0}; // 0 -255 depending on the peak of the song band...
double total_song_peaks[2];
unsigned long num_song_peaks[2];


///////////////////////////////// General Purpose Functions //////////////////////////////////
void printClickStats() {
  if (PRINT_CLICK_FEATURES) {
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
}

void updateClickAudioFeaturesRMS(uint8_t i) {
  click_rms_delta[i] = last_click_rms_val[i] - click_rms_val[i];
  if (click_rms_delta[i] > CLICK_RMS_DELTA_THRESH) {
    // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
    // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
    if (neos[i].flashOn() ==  true) { // all flash logic is conducted in the NeoGroup instance
      num_cpm_clicks[i]++;
      Serial.print("cpm clicks increased : ");
      Serial.println(num_cpm_clicks[i]);
    }
  }
}

void updateClickAudioFeaturesPeak(uint8_t i) {
  click_peak_delta[i] = last_click_peak_val[i]  - click_peak_val[i];
  if (click_peak_delta[i] > CLICK_PEAK_DELTA_THRESH) {
    if (neos[i].flashOn() == true) { // all flash logic is conducted in the NeoGroup instance
      num_cpm_clicks[i]++;
      Serial.print("cpm clicks increased : ");
      Serial.println(num_cpm_clicks[i]);
    }
  }
}

///////////////////////////////////////////////////////////////////////
//                       TCA9532A I2C bus expander
///////////////////////////////////////////////////////////////////////
void writeAudioUsageToEEPROM(uint8_t used) {
  if (data_logging_active) {
    EEPROM.update(EEPROM_AUDIO_MEM_USAGE, used);
    dprintln(PRINT_LOG_WRITE, "logged audio memory usage to EEPROM");
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

void updateLuxSensors() {
  // Return a 1 if the lux sensors are read and a 0 if they are not
  // if the LEDs have been off, on their own regard, for 40ms or longer...
  // and it has been long-enough to warrent a new reading
  // dprintln(PRINT_LUX_DEBUG,"\nchecking lux sensors: ");
  combined_lux = 0;
  for (unsigned int i = 0; i < sizeof(lux_sensors) / sizeof(lux_sensors[0]); i++) {
    // for each lux sensor, if the LEDs are off, have been off for longer than the LED_SHDN_LEN
    // and it has been longer than the
    // min reading delay then read the sensors
    lux_sensors[i].update();
    combined_lux += lux_sensors[i].getLux();
  }
}

/*
  long longFIFO(long a[], long val){
  for (int i = (sizeof(a) / sizeof(a[0])) - 2; i > 0; i--) {
    a[i] = a[i-1];
  }
  a[0] = val;
  return a;
  }
*/

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

// this should be in the main audio loop
void updateClicks() {
  // Serial.println("WARNING CALCCULATECLICKAUDIOFEATURES IS BROKEN!");
  for (int i = 0; i < num_channels; i++) {
    if ( (CLICK_FEATURE == RMS_DELTA) || (CLICK_FEATURE == ALL_FEATURES)) {
      if (click_rms1.available() && i == 0) {
        last_click_rms_val[i] = click_rms_val[i];
        click_rms_val[i] = click_rms1.read();
        updateClickAudioFeaturesRMS(i);
      }
      else if (click_rms2.available() && i == 1) {
        last_click_rms_val[i] = click_rms_val[i];
        click_rms_val[i] = click_rms2.read();
        updateClickAudioFeaturesRMS(i);
      }
    }
    if (CLICK_FEATURE == PEAK_DELTA || CLICK_FEATURE == ALL_FEATURES) {
      if (click_peak1.available() && i == 0 ) {
        last_click_peak_val[i] = click_peak_val[i];
        click_peak_val[i] = click_peak1.read();
        updateClickAudioFeaturesPeak(i);
      }
      else if (click_peak2.available() && i == 1 ) {
        last_click_peak_val[i] = click_peak_val[i];
        click_peak_val[i] = click_peak2.read();
        updateClickAudioFeaturesPeak(i);
      }
    }
    if (CLICK_FEATURE != PEAK_DELTA && CLICK_FEATURE != RMS_DELTA) {
      Serial.print("sorry the CLICK_FEATURE ");
      Serial.print(CLICK_FEATURE);
      Serial.println(" is not implemented/available");
    }
  }
  printClickStats();
  for (unsigned int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++) {
    neos[i].updateFlash();
  }
};

void updateSongGain(double song_gain[]) {
  // todo right now it just updates the song gain for everything
  for (unsigned int i = 0; i < num_channels; i++) {
    if (i == 0) {
      song_input_amp1.gain(song_gain[i]);
      song_mid_amp1.gain(song_gain[i]);
      song_post_amp1.gain(song_gain[i]);
      dprint(PRINT_SONG_DATA, "\nupdated song gain "); dprint(PRINT_SONG_DATA, frText(i));dprint(PRINT_SONG_DATA, ":\t");
      dprintln(PRINT_SONG_DATA, song_gain[i]);
      writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT + (i * 4), song_gain[i]);
    } else if (i == 1) {
      song_input_amp2.gain(song_gain[1]);
      song_mid_amp2.gain(song_gain[1]);
      song_post_amp2.gain(song_gain[1]);
      dprint(PRINT_SONG_DATA, "\nupdated song gain "); dprint(PRINT_SONG_DATA, frText(i)); dprint(PRINT_SONG_DATA, ":\t");
      dprintln(PRINT_SONG_DATA, song_gain[i]);
      writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT + (i * 4), song_gain[i]);
    }
  }
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
  Serial.println("- - - - - - - - - - - - - - - - - - - -");
  Serial.print("Setting up the audio channel settings : \n");// Serial.println(i);
  click_biquad1.setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad1.setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad1.setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad1.setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);
  click_biquad2.setHighpass(0, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad2.setHighpass(1, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad2.setHighpass(2, CLICK_BQ1_THRESH, CLICK_BQ1_Q);
  click_biquad2.setLowShelf(3, CLICK_BQ1_THRESH, CLICK_BQ1_DB);
  Serial.print("First Click BiQuads HP-HP-HP-LS       :\t");
  Serial.print("thresh:\t"); Serial.print(CLICK_BQ1_THRESH); Serial.print("\tQ\t");
  Serial.print(CLICK_BQ1_Q); Serial.print("\tdB"); Serial.println(CLICK_BQ1_DB);

  click_biquad21.setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad21.setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad21.setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad21.setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);
  click_biquad11.setLowpass(0,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad11.setLowpass(1,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad11.setLowpass(2,   CLICK_BQ2_THRESH, CLICK_BQ2_Q);
  click_biquad11.setHighShelf(3, CLICK_BQ2_THRESH, CLICK_BQ2_DB);
  Serial.print("Second Click BiQuads HP-HP-HP-LS      :\t");
  Serial.print("thresh:\t"); Serial.print(CLICK_BQ2_THRESH); Serial.print("\tQ\t");
  Serial.print(CLICK_BQ2_Q); Serial.print("\tdB"); Serial.println(CLICK_BQ2_DB);


  // Audio for the song channel...
  song_biquad1.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);
  song_biquad2.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);
  Serial.print("\nFirst Song BiQuads HP-HP-HP-LS        :\t");
  Serial.print("thresh:\t"); Serial.print(SONG_BQ1_THRESH); Serial.print("\tQ\t");
  Serial.print(SONG_BQ1_Q); Serial.print("\tdB"); Serial.println(SONG_BQ1_DB);

  song_biquad11.setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad11.setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad11.setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad11.setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);
  song_biquad21.setLowpass(0,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad21.setLowpass(1,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad21.setLowpass(2,   SONG_BQ2_THRESH, SONG_BQ2_Q);
  song_biquad21.setHighShelf(3, SONG_BQ2_THRESH, SONG_BQ2_DB);
  Serial.print("Second Song BiQuads HP-HP-HP-LS       :\t");
  Serial.print("thresh:\t"); Serial.print(SONG_BQ1_THRESH); Serial.print("\tQ\t");
  Serial.print(SONG_BQ2_Q); Serial.print("\tdB"); Serial.println(SONG_BQ2_DB);

  click_input_amp1.gain(click_gain[0]);
  click_mid_amp1.gain(click_gain[0]);
  click_post_amp1.gain(click_gain[0]);
  click_input_amp2.gain(click_gain[1]);
  click_mid_amp2.gain(click_gain[1]);
  click_post_amp2.gain(click_gain[1]);
  Serial.print("\nClick gains all set to                :\t");
  Serial.print(click_gain[0]); Serial.print("\t"); Serial.println(click_gain[1]);

  song_input_amp1.gain(song_gain[0]);
  song_mid_amp1.gain(song_gain[0]);
  song_post_amp1.gain(song_gain[0]);
  song_input_amp2.gain(song_gain[1]);
  song_mid_amp2.gain(song_gain[1]);
  song_post_amp2.gain(song_gain[1]);
  Serial.print("Song gains all set to                 :\t");
  Serial.print(song_gain[0]); Serial.print("\t"); Serial.println(song_gain[1]);
  delay(1000);

  /////////////////////////////////
  // VEML sensors through TCA9543A
  /////////////////////////////////

  Serial.println();
  printMinorDivide();
  Serial.println("Searching for Lux Sensors");
  Serial.print("Log Polling Rate (ms)              :\t");
  Serial.println(LOG_POLLING_RATE);
  if (lux_max_reading_delay > LOG_POLLING_RATE) {
    lux_max_reading_delay = LOG_POLLING_RATE;
    Serial.print("updated lux_max_reading_delay to   :\t");
    Serial.println(lux_max_reading_delay);
  }
  else {
    Serial.print("lux_max_reading_delay is set to    :\t");
    Serial.println(lux_max_reading_delay);
  }
  if (lux_min_reading_delay > LOG_POLLING_RATE) {
    lux_min_reading_delay = LOG_POLLING_RATE;
    Serial.print("updated lux_min_reading_delay to   :\t");
    Serial.println(lux_min_reading_delay);
  }
  else {
    Serial.print("lux_min_reading_delay is set to    :\t");
    Serial.println(lux_min_reading_delay);
  }
  delay(200);

  /////////////////////////////////
  // Start the LEDs ///////////////
  /////////////////////////////////
  printMinorDivide();
  Serial.println("setting up LEDs");
  delay(250);
  neos[0].colorWipe(0, 10, 0); // turn off the LEDs
  neos[1].colorWipe(0, 0, 10); // turn off the LEDs
  Serial.println("\nFinshed cicada Setup Loop");
  printDivide();
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

void printSongStats() {
  for (int i = 0; i < num_channels; i++) {
    dprint(PRINT_SONG_DATA, "Song -- "); dprint(PRINT_SONG_DATA, i); dprint(PRINT_SONG_DATA, " | rms_weighted: ");
    dprint(PRINT_SONG_DATA, song_rms_weighted[i]);
    dprint(PRINT_SONG_DATA, "\t peak: ");
    dprint(PRINT_SONG_DATA, song_peak_weighted[i]);
    dprintln(PRINT_SONG_DATA);
  }
}


void songDisplay() {
  for (int i = 0; i < num_channels; i++) {
    // if (flash_on[i] == false) {
    // TODO make a user control which allows for selection between RMs and peak
    if (SONG_FEATURE == PEAK_DELTA) {
      if (stereo_audio == false || front_mic_active == false || rear_mic_active == false) {
        if (front_mic_active == true && i == 0) {
          neos[0].colorWipe(song_peak_weighted[i], 0, 0);
          neos[1].colorWipe(song_peak_weighted[i], 0, 0);
        } else if (rear_mic_active == true && i == 1) {
          neos[0].colorWipe(song_peak_weighted[i], 0, 0);
          neos[1].colorWipe(song_peak_weighted[i], 0, 0);          
        }
      } else {
        neos[i].colorWipe(song_peak_weighted[i], 0, 0);
        // colorWipeRear(song_peak_weighted_r, 0, 0);
      }
    } else if (SONG_FEATURE == RMS_DELTA) {
      if (stereo_audio == false) {
        if (front_mic_active == true && i == 0) {
          neos[0].colorWipe(song_rms_weighted[i], 0, 0);
          neos[1].colorWipe(song_rms_weighted[i], 0, 0);
        } else if (rear_mic_active == true && i == 1) {
          neos[0].colorWipe(song_rms_weighted[i], 0, 0);
          neos[1].colorWipe(song_rms_weighted[i], 0, 0);          
        }
      } else  {
        neos[i].colorWipe(song_rms_weighted[i], 0, 0);
        // colorWipeRear(song_rms_weighted_r, 0, 0);
      }
    } else {
      Serial.print("ERROR: the SONG_FEATURE ");
      Serial.print(SONG_FEATURE);
      Serial.println(" is not a valid/implemented SONG_FEATURE");
    }
  }
}

void updateSongGainMinMax() {
  if (song_gain[0] > song_gain_max[0]) {
    song_gain_max[0] = song_gain[0];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX, song_gain_max[0]);
    dprint(PRINT_SONG_DATA, "logged new front song gain high : "); dprintln(PRINT_SONG_DATA, song_gain_max[0]);
  } else if (song_gain[0] < song_gain_min[0]) {
    song_gain_min[0] = song_gain[0];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN, song_gain_min[0]);
    dprint(PRINT_SONG_DATA, "logged new front song gain low  : "); dprintln(PRINT_SONG_DATA, song_gain_min[0]);
  }
  if (song_gain[1] > song_gain_max[1]) {
    song_gain_max[1] = song_gain[1];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX + 4, song_gain_max[1]);
    dprint(PRINT_SONG_DATA, "logged new rear  song gain high : "); dprintln(PRINT_SONG_DATA, song_gain_max[1]);
  } else if (song_gain[1] < song_gain_min[1]) {
    song_gain_min[1] = song_gain[1];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN + 4, song_gain_min[1]);
    dprint(PRINT_SONG_DATA, "logged new rear song gain low   : "); dprintln(PRINT_SONG_DATA, song_gain_min[1]);
  }
}

bool adjustSongGainLedOnRatio() {
  bool success = false;
  for (int i = 0; i < num_channels; i++) {
    double cost = 0.5;
    if (neos[i].getOnRatio() > MAX_LED_ON_RATIO) {
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] -= change;
      // ensure that what we have is not less than the min
      song_gain[i] = max(song_gain[i], MIN_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "led_on_ratio is too high ("); dprint(PRINT_AUTO_GAIN, neos[i].getOnRatio());
      dprint(PRINT_AUTO_GAIN, ") lowering the song gain "); dprintln(PRINT_AUTO_GAIN, i);
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      success = true;
    } else if (neos[i].getOnRatio() < MIN_LED_ON_RATIO) {
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] += change;
      // ensure that what we have is not less than the min
      song_gain[i] = min(song_gain[i], MAX_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "led_on_ratio is too low ("); dprint(PRINT_AUTO_GAIN, neos[i].getOnRatio());
      dprint(PRINT_AUTO_GAIN, ") raising the song gain "); dprintln(PRINT_AUTO_GAIN, i);
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      success = true;
    }
  }
  if (success) {
    updateSongGain(song_gain);
    updateSongGainMinMax();
    return 1;
  }
  return 0;
}

void checkSongAutoGain() {
  adjustSongGainLedOnRatio();
  bool success = false;
  for (int i = 0; i < num_channels; i++) {
    ///////////////////////////////////////////////////////////////
    // second check is to see if the song gain needs to be adjusted
    ///////////////////////////////////////////////////////////////
    // calculate the average peak values since the last auto-gain adjust
    double avg_song_peak = total_song_peaks[i] / num_song_peaks[i];
    double cost; // our cost variable
    dprint(PRINT_AUTO_GAIN, "\n--------- song "); dprint(PRINT_AUTO_GAIN, i); dprintln(PRINT_AUTO_GAIN, " -------------");
    dprint(PRINT_AUTO_GAIN, "total_song_peaks ");
    dprintln(PRINT_AUTO_GAIN, total_song_peaks[i]);
    dprint(PRINT_AUTO_GAIN, "num_song_peaks ");
    dprintln(PRINT_AUTO_GAIN, (long) num_song_peaks[i]);
    // if the avg value is more than the max...
    if (avg_song_peak > MAX_SONG_PEAK_AVG) {
      // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
      cost = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak);
      // calculate what the new song_gain will be
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] -= change;
      // ensure that what we have is not less than the min
      song_gain[i] = max(song_gain[i], MIN_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "song gain decreased by ");
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      success = true;
    }
    // if the average value is less than the min....
    else if (avg_song_peak < MIN_SONG_PEAK_AVG) {
      dprintln(PRINT_AUTO_GAIN);
      dprint(PRINT_AUTO_GAIN, "avg_song_peak lower than MIN_SONG_PEAK_AVG ");
      dprintln(PRINT_AUTO_GAIN, avg_song_peak);
      // calculate cost between 0 and 1 with higher cost resulting in higher gain attenuation
      cost = 1.0 - (MIN_SONG_PEAK_AVG / avg_song_peak);
      dprint(PRINT_AUTO_GAIN, "cost : ");
      dprintln(PRINT_AUTO_GAIN, cost);
      // calculate the new song gain
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] += change;
      // ensure what we have is not less than the max...
      song_gain[i] = min(song_gain[i], MAX_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "song gain increased by ");
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      dprintln(PRINT_AUTO_GAIN);
      success = true;
    }
    // now check the avg on time? todo

    ///////////////////////////////////////////////////////////////
    // last thing to do is reset the last_auto_gain_adjustment timer
    ///////////////////////////////////////////////////////////////
    total_song_peaks[i] = 0;
    num_song_peaks[i] = 0;
  }
  if (success) {
    updateSongGain(song_gain);
    updateSongGainMinMax();
  };
}
// #endif // __CICADA_SONG_H__
/*#ifndef CICADA_CONFIG_H
  #define CICADA_CONFIG_H

  #include "MothConfig.h"
*/
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

void updateClickGain(double click_gain[], double click_gain_min[], double click_gain_max[]) {
  //
  Serial.println("WARNING UPDATECLICKGAIN IS NOT IMPLEMENTED YET");
}


uint8_t testRearMicrophone () {
  // go through and gather 10 features from each channel and make sure it is picking up audio
  uint8_t readings = 0;
  double values = 0.0;
  unsigned long a_time = millis();
  Serial.print("Testing Rear Microphone");
  while (readings < 10 && millis() < a_time + 2000) {
    if (click_rms2.available()) {
      values += click_rms2.read();
      readings++;
      Serial.print(".");
      delay(20);
    }
  }
  if (values > 0) {
    Serial.println("\nRear Microphone is good");
    return true;
  } else {
    Serial.println("\nERROR, Rear Microphone does not work");
    printDivideLn();
    return false;
  }
}

uint8_t testFrontMicrophone () {
  // go through and gather 10 features from each channel and make sure it is picking up audio
  uint8_t readings = 0;
  double values = 0.0;
  unsigned long a_time = millis();
  Serial.print("Testing Front Microphone");
  while (readings < 10 && millis() < a_time + 2000) {
    if (click_rms1.available()) {
      values += click_rms1.read();
      readings++;
      Serial.print(".");
      delay(20);
    }
  }
  if (values > 0) {
    Serial.println("\nFront Microphone is good");
    return true;
  } else {
    Serial.println("\nERROR, Front Microphone does not work");
    return false;
  }
}

void testMicrophones() {
  if (testFrontMicrophone() == false) {
    // todo - do something to deactive this audio channel
    Serial.println("setting front_mic_active to false");
    front_mic_active = false;
  }
  if (testRearMicrophone() == false) {
    // todo - do something to deactive this audio channel
    Serial.println("setting rear_mic_active to false");
    rear_mic_active = false;
  }
  printDivideLn();
}
// #endif // CICADA_CONFIG_H
// #endif // __CIDADA_MODE_H__

/*#ifndef __CICADA_GAIN_CONTROL_H__
  #define __CICADA_GAIN_CONTROL_H__
  #include "MothConfig.h"
  // #include "Mode_Cicada.h"
*/
/*
class FeatureCollector {
  public:
    FeatureCollector(AudioAnalyzeRMS *r, AudioAnalyzePeak *p);
    void update();
  private:
    AudioAnalyzeRMS *rms_ana;
    AudioAnalyzePeak *peak_ana;
    void calculateRMS();
    void calculatePeak();
    double rms;
    double rms_pos_delta;
    double peak;
    double peak_pos_delta;
};

class AutoGain {
  public:
    AutoGain(AudioAnalyzeRMS *r, AudioAnalyzePeak *p);
    void update();
  private:
    FeatureCollector fc;

};

void FeatureCollector::update() {

}

FeatureCollector::FeatureCollector(AudioAnalyzeRMS *r, AudioAnalyzePeak *p) {
  rms_ana = r;
  peak_ana = p;
}
*/

void calculateSongAudioFeatures() {
  // TODO, rework the whole calculate weighted song brigtness to something that makes more sense
  // TODO rework to only calculate the features which are the features used
  if (front_mic_active) {
    if (song_rms1.available()) {
      double song_rms_val = song_rms1.read();
      song_rms_weighted[0] = map(constrain((song_rms_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
    }
    if (song_peak1.available()) {
      // random magiv 5 varaible needs to be better defined/tested TODO
      double song_peak_val = song_peak1.read();
      song_peak_weighted[0] = map(constrain((song_peak_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
#if (auto_gain_active == 1)
      num_song_peaks[0]++;
      total_song_peaks[0] += song_peak_val * 100;
#endif// autogain
    }
  }
  if (rear_mic_active) {
    if (song_rms2.available()) {
      double song_rms_val = song_rms2.read();
      song_rms_weighted[1] = map(constrain((song_rms_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
    }
    if (song_peak2.available()) {
      // random magiv 5 varaible needs to be better defined/tested TODO
      double song_peak_val = song_peak2.read();
      song_peak_weighted[1] = map(constrain((song_peak_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
#if (AUTO_GAIN == 1)
      num_song_peaks[1]++;
      total_song_peaks[1] += song_peak_val * 100;
#endif// autogain
    }
  }
  // TODO, perhaps add another feature or two, perhaps an option for combining the two readings?
  // will only print if flag is set
  printSongStats();
}


void checkClickAutoGain() {
  bool update_gain = false;                                                   // will we update the gain at the end of the function?
  for (int i = 0; i < num_channels; i++) {
    if ((front_mic_active && i == 0) || (rear_mic_active && i == 1)) {
      dprint(PRINT_AUTO_GAIN, "num_past_clicks: ");
      dprintln(PRINT_AUTO_GAIN, (String)num_past_clicks[i]);
      // first check is to see if there has been too many/few clicks detected
      double clicks_per_minute = ((double)num_past_clicks[i] * 60000) / (double)last_auto_gain_adjustment;
      double cost;                                                              // our cost variable
      dprint(PRINT_AUTO_GAIN, "clicks_per_minute: ");
      // printDouble(clicks_per_minute, 1000000);
      dprintln(PRINT_AUTO_GAIN, clicks_per_minute);
      if (clicks_per_minute == 0) {                                             // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
        click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT;
        click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);
        update_gain = true;
      }
      // then check if there are too few clicks_per_minute
      else if (clicks_per_minute < MIN_CLICKS_PER_MINUTE) {
        // there are too few clicks, need to increase the gain to compensate
        // first we calculate the factor by which the clicks are off
        // the higher the number the more it is off
        cost = 1.0 - (clicks_per_minute / MIN_CLICKS_PER_MINUTE);              // max amount that can be adjusted
        click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
        click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);                    // make sure that the click_gain is not more than the max_click_gain
        update_gain = true;
      }
      // then check to see if there are too many clicks per-minute
      else if (clicks_per_minute > MAX_CLICKS_PER_MINUTE) {
        cost = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute);              // determine a cost function... the higher the value the more the cost
        click_gain[i] -= click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;           // adjust the click_gain
        click_gain[i] = max(click_gain[i], MIN_CLICK_GAIN);                    // make sure that the gain is not set too low
        update_gain = true;
      }
      num_past_clicks[i] = 0;
    }
  }
  if (update_gain == true) {
    updateClickGain(click_gain, click_gain_min, click_gain_max);          // update the click gain in the three gain stages
  }
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
  // if it has not been long enough since the last check then exit now
  if (last_auto_gain_adjustment < auto_gain_frequency || millis() < 60000) {
    return;
  };
  dprintln(PRINT_AUTO_GAIN, "-------------------- Auto Gain Start ---------------------------");

  // todo add back
  // resetOnOffRatioCounters();
  adjustSongGainLedOnRatio();

  checkClickAutoGain();
  checkSongAutoGain();

  last_auto_gain_adjustment = 0;
  dprintln(PRINT_AUTO_GAIN, " ------------------------------------------------- ");
}

uint8_t updateLuxLog() {
  // if datalogging is active
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && lux_eeprom_idx < EEPROM_LUX_LOG_END) {
    dprint(PRINT_LOG_WRITE, "Logging Lux Data into EEPROM location: "); dprintln(PRINT_LOG_WRITE, (int)lux_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "lux_average                          :\t");  
    // store the current lux readings
    // increment the index, 4 bytes to a double
    if (front_lux_active) {
      dprint(PRINT_LOG_WRITE, lux_sensors[0].getAvgLux()); 
      writeDoubleToEEPROM(lux_eeprom_idx, lux_sensors[0].getAvgLux());
      lux_eeprom_idx += 4;
    } else{
      dprint(PRINT_LOG_WRITE, "Front sensor deactivated ");
    }
    if (rear_lux_active) {
      writeDoubleToEEPROM(lux_eeprom_idx, lux_sensors[1].getAvgLux());
      lux_eeprom_idx += 4;
      dprint(PRINT_LOG_WRITE, "\t");
      dprintln(PRINT_LOG_WRITE, lux_sensors[1].getAvgLux());
    } else {
      dprint(PRINT_LOG_WRITE, "Front sensor deactivated ");
    }
    return 1;
  }
  return 0;
}

bool updateLuxMinMaxDatalog() {
  // give the program some time to settle
  if (data_logging_active && millis() > 20000) {
    // front
    for (int i = 0; i < NUM_LUX_SENSORS; i++) {
      // is the current reading more than the max recorded?
      // if the current reading is the same then nothing is written, if it is different something is writen
      writeDoubleToEEPROM(EEPROM_MAX_LUX_READINGS + (i * 4) , lux_sensors[i].getMaxLux());
      dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, lux_sensors[i].getName());
      dprint(PRINT_LOG_WRITE, " max_lux_reading to EEPROM at addr: "); dprint(PRINT_LOG_WRITE, EEPROM_MAX_LUX_READINGS + (i * 4));
      dprint(PRINT_LOG_WRITE, " :\t"); dprintln(PRINT_LOG_WRITE, lux_sensors[i].getMaxLux());
      dprint(PRINT_LOG_WRITE, " read back:\t");dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + (i*4)));

      writeDoubleToEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4) , lux_sensors[i].getMinLux());
      dprint(PRINT_LOG_WRITE, "logged new "); dprint(PRINT_LOG_WRITE, lux_sensors[i].getName());
      dprint(PRINT_LOG_WRITE, " min_lux_reading to EEPROM\t"); dprint(PRINT_LOG_WRITE, lux_sensors[i].getMinLux());
      dprint(PRINT_LOG_WRITE, " read back:\t"); dprintln(PRINT_LOG_WRITE, readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + (i * 4)));
    }
  }
  // combined
  if (combined_lux > combined_max_lux_reading) {
    combined_max_lux_reading = combined_lux;
    writeDoubleToEEPROM(EEPROM_MAX_LUX_READING_COMBINED, combined_max_lux_reading);
    dprint(PRINT_LOG_WRITE, "logged new combined max_lux_reading to EEPROM\t"); dprintln(PRINT_LOG_WRITE, combined_max_lux_reading);
    return 1;
  } else if (combined_lux < combined_min_lux_reading) {
    combined_min_lux_reading = combined_lux;
    writeDoubleToEEPROM(EEPROM_MIN_LUX_READING_COMBINED , combined_min_lux_reading);
    dprint(PRINT_LOG_WRITE, "logged new combined min_lux_reading to EEPROM\t"); dprintln(PRINT_LOG_WRITE, combined_max_lux_reading);
    return 1;
  }
  return 0;
}

uint8_t updateBrightnessScalerAvgLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    dprint(PRINT_LOG_WRITE,"Logging the average brightness scalers  :");
    for (int  i = 0; i < 2; i++) { // replace 2 with something variable
      dprint(PRINT_LOG_WRITE,"\t");
      writeDoubleToEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4), neos[i].getAvgBrightnessScaler());
      dprint(PRINT_LOG_WRITE,neos[i].getAvgBrightnessScaler());
    }
    dprintln(PRINT_LOG_WRITE);
    return 1;
  }
  return 0;
}

void printBrightnessAverageLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  Serial.print("Printing the average brightness scalers :");
  for (int  i = 0; i < 3; i++) {
    Serial.print("\t");
    Serial.print(readDoubleFromEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4)));
  }
  Serial.println();
}

uint8_t updateOnRateLog() {
  // TODO
  if (log_timer > LOG_POLLING_RATE) {
    dprint(PRINT_LOG_WRITE, "Logging the LED on rates                :");
    for (int  i = 0; i < 2; i++) {
      dprint(PRINT_LOG_WRITE,"\t");
      writeDoubleToEEPROM(EEPROM_LED_ON_RATIO + (i * 4), neos[i].getOnRatio());
      Serial.print(neos[i].getOnRatio());
    }
    dprintln(PRINT_LOG_WRITE);
    return 1;
  }
  return 0;
}

void printOnRatioLog() {
  dprint(PRINT_LOG_WRITE,"Reading the LED on rates                :");
  for (int  i = 0; i < 2; i++) {
    dprint(PRINT_LOG_WRITE,"\t");
    dprint(PRINT_LOG_WRITE,readDoubleFromEEPROM(EEPROM_LED_ON_RATIO + (i * 4)));
  }
  dprintln(PRINT_LOG_WRITE);
}

//#ifndef __LOGGING_CICADA_MODE_H__
//#define __LOGGING_CICADA_MODE_H__

void writeTotalClicksToEEPROM() {
  if (data_logging_active) {
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS, total_clicks_detected[0]);
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS + 4, total_clicks_detected[1]);
    dprint(PRINT_LOG_WRITE, "Updated EEPROM with total clicks detected front/rear: ");
    dprint(PRINT_LOG_WRITE, (int)total_clicks_detected[0]);
    dprint(PRINT_LOG_WRITE, "\t"); dprintln(PRINT_LOG_WRITE, (int)total_clicks_detected[1]);
  }
}

void updateRuntimeAndClicks() {
  if (last_runtime_update > RUNTIME_POLL_DELAY) {
    // dprint(PRINT_LOG_WRITE,"time to update runtime");
    writeRuntimeToEEPROM();
    writeTotalClicksToEEPROM();
    last_runtime_update = 0;
  }
}

uint8_t updateCPMLog() {
  // if enough time has passed since last logging and the log still has space allowcated to it
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    double cpm[2];
    for (int i = 0; i < 2; i++) {
      dprint(PRINT_LOG_WRITE, "num_cpm_clicks / cpm "); 
      if ( i == 0) {
        dprint(PRINT_LOG_WRITE, "Front           :\t");
      } else{
        dprint(PRINT_LOG_WRITE, "Rear            :\t");
      }
      dprint(PRINT_LOG_WRITE, num_cpm_clicks[i]);
      cpm[i] = (double)num_cpm_clicks[i] / ((double)log_timer / 60000);
      dprint(PRINT_LOG_WRITE, "/"); dprintln(PRINT_LOG_WRITE, cpm[i]);
      // reset the average values
      num_cpm_clicks[i] = 0;
    }
    dprint(PRINT_LOG_WRITE, "Logging CPM Data into EEPROM location: "); dprint(PRINT_LOG_WRITE, cpm_eeprom_idx);
    dprint(PRINT_LOG_WRITE, "\tcpm :\t"); dprint(PRINT_LOG_WRITE, cpm[0]); dprint(PRINT_LOG_WRITE, "\t");
    dprintln(PRINT_LOG_WRITE, cpm[1]);
    dprintMinorDivide(PRINT_LOG_WRITE);
    // store the current lux readings
    // increment the index, 4 bytes to a double
    writeDoubleToEEPROM(cpm_eeprom_idx, cpm[0]);
    cpm_eeprom_idx += 4;
    writeDoubleToEEPROM(cpm_eeprom_idx, cpm[1]);
    cpm_eeprom_idx += 4;
    return 1;
  }
  return 0;
}
/*
  void readLuxLogFromEEPROM() {
  Serial.print("luxlog as stored in EEPROM");
  for (int i = EEPROM_LUX_LOG; i < LUX_LOG_LAST_ADDR; i  += 4) {
    double val = readDoubleFromEEPROM(i);
    if ( val > 0) {
      Serial.print(i - 1000 / 4); Serial.print("\t"); Serial.print(val); dprintln(PRINT_LOG_WRITE, "\t");
      if (i % 20 ==  0) {
        dprintln(PRINT_LOG_WRITE, );
      }
    }
  }
  }
*/
void updateEEPROMLogs() {
  if (data_logging_active) {
    // if enough time has passed since init for data-logging to be active
    uint8_t updates = 0;
    if (millis() > LOG_START_DELAY) {
      //  if one of the logs update then update the log timer
      updates += updateOnRateLog();
      updates += updateBrightnessScalerAvgLog();
      updates += updateLuxLog();
      updates += updateCPMLog();
      updates += updateLuxMinMaxDatalog();
    }
    if (updates) {
      log_timer = 0;
      Serial.print("A Total of "); Serial.print(updates); Serial.println(" logs were updated");
      printDivide();
    }
  }
}

void printEEPROMContents() {
  // todo add printing for the serial_id
  printMajorDivide((String)"Printing EEPROM CONTENTS ...");
  Serial.print("hardware version    :\t");Serial.print(EEPROM.read(EEPROM_H_VERSION_MAJOR));
  Serial.print(".");Serial.println(EEPROM.read(EEPROM_H_VERSION_MINOR));
  
  Serial.print("firmware version    :\t");Serial.print(EEPROM.read(EEPROM_S_VERSION_MAJOR));
  Serial.print(".");Serial.print(EEPROM.read(EEPROM_S_VERSION_MINOR));Serial.print(".");
  Serial.println(EEPROM.read(EEPROM_S_SUBVERION));
  
  Serial.print("bot serial number   :\t");
  Serial.println(EEPROM.read(EEPROM_SERIAL_ID));
  Serial.print("run time in ms      :\t");
  Serial.println(readLongFromEEPROM(EEPROM_RUN_TIME));
  Serial.print("run time in minutes :\t");
  double rt = readLongFromEEPROM(EEPROM_RUN_TIME) / 1000 / 60;
  Serial.println(rt);
  printMinorDivide();
  Serial.print("onboard jumper settings\t\t");
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
  printMinorDivide();

  Serial.println("\nDatalogging settings");
  printMinorDivide();
  Serial.print("Datalog Active :\t"); Serial.println(EEPROM.read(EEPROM_LOG_ACTIVE));
  Serial.print("Start time     :\t"); Serial.println((double)readLongFromEEPROM(EEPROM_LOG_START_TIME) / 60000);
  Serial.print("End time       :\t"); Serial.println((double)readLongFromEEPROM(EEPROM_LOG_END_TIME) / 60000);
  Serial.print("Logging Rate   :\t"); Serial.println((double)readLongFromEEPROM(EEPROM_LOG_POLLING_RATE) / 60000);

  Serial.println("\nAudio Settings");
  printMinorDivide();
  Serial.print("Audio memory usage/max            :\t"); Serial.print(EEPROM.read(EEPROM_AUDIO_MEM_USAGE));
  Serial.print("\t"); Serial.println(EEPROM.read(EEPROM_AUDIO_MEM_MAX));

  Serial.println("\n - CLICK");
  printMinorDivide();
  Serial.print("front/rear starting gain          :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_START + 4));
  Serial.print("front min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX));
  Serial.print("rear  min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MIN + 4)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_MAX + 4));
  Serial.print("last recorded gain front/rear     :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_CURRENT)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_CLICK_GAIN_CURRENT + 4));
  Serial.print("total clicks detected front/rear  :\t");
  Serial.print(readLongFromEEPROM(EEPROM_TOTAL_CLICKS)); Serial.print("\t");
  Serial.println(readLongFromEEPROM(EEPROM_TOTAL_CLICKS + 4));
  Serial.print("Average number of CPM front/rear  :\t");
  Serial.print(readLongFromEEPROM(EEPROM_TOTAL_CLICKS) / rt); Serial.print("\t");
  Serial.println(readLongFromEEPROM(EEPROM_TOTAL_CLICKS + 4) / rt);
  printAndClearDoubleLog(EEPROM_CPM_LOG_START, EEPROM_CPM_LOG_END, "Clicks Per Minute ");

  Serial.println("\n -  SONG  ");
  printMinorDivide();
  Serial.print("front/read starting gain          :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_START)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_START + 4));
  Serial.print("front min/max recorded gain       :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MIN)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MAX));
  Serial.print("rear min/max recorded gain        :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MIN + 4)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_SONG_GAIN_MAX + 4));
  Serial.print("last recorded gain front/rear     :\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_CURRENT)); Serial.print("\t");
  Serial.print(readDoubleFromEEPROM(EEPROM_SONG_GAIN_CURRENT + 4)); Serial.println();

  Serial.println("\nLux Settings");
  printDivide();
  Serial.print("min/max front lux reading         : \t");
  Serial.print(readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS));
  Serial.print("min/max rear lux reading          : \t");
  Serial.print(readDoubleFromEEPROM(EEPROM_MIN_LUX_READINGS + 4)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_MAX_LUX_READINGS + 4));
  Serial.print("min/max combined lux reading      : \t");
  Serial.print(readDoubleFromEEPROM(EEPROM_MIN_LUX_READING_COMBINED)); Serial.print("\t");
  Serial.println(readDoubleFromEEPROM(EEPROM_MAX_LUX_READING_COMBINED));
  printAndClearDoubleLog(EEPROM_LUX_LOG_START, EEPROM_LUX_LOG_END, " LUX ");

  Serial.println("\nLED Settings");
  printDivide();
  printOnRatioLog();
  printBrightnessAverageLog();

  printMajorDivide("Finished Printing EEPROM Datalog");
}

void writeSetupConfigsToEEPROM() {
  if (data_logging_active) {
    EEPROM.update(EEPROM_JMP1, cicada_mode);
    EEPROM.update(EEPROM_JMP2, stereo_audio);
    EEPROM.update(EEPROM_JMP3, NUM_LUX_SENSORS);
    EEPROM.update(EEPROM_JMP4, combine_lux_readings);
    EEPROM.update(EEPROM_JMP5, gain_adjust_active);
    EEPROM.update(EEPROM_JMP6, data_logging_active);
    dprintln(PRINT_LOG_WRITE, "logged jumper values to EEPROM");

    // log the starting gains to the min/max EEPROM
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN + 4, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN + 4, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_START, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_START + 4, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_START, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_START + 4, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX + 4, click_gain[1]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX + 4, song_gain[1]);

    writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT, song_gain[0]);
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT + 4, song_gain[1]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT, click_gain[0]);
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT + 4, click_gain[1]);

    writeLongToEEPROM(EEPROM_TOTAL_CLICKS, 0);
    writeLongToEEPROM(EEPROM_TOTAL_CLICKS + 4, 0);


    EEPROM.update(EEPROM_AUDIO_MEM_MAX, AUDIO_MEMORY);
    dprintln(PRINT_LOG_WRITE, "logged AUDIO_MEMORY to EEPROM");

    // auto-log values
    EEPROM.update(EEPROM_LOG_ACTIVE, data_logging_active);
    writeLongToEEPROM(EEPROM_LOG_POLLING_RATE, LOG_POLLING_RATE);
    writeLongToEEPROM(EEPROM_LOG_START_TIME, LOG_START_DELAY);
    writeLongToEEPROM(EEPROM_LOG_END_TIME,  LOG_START_DELAY + LOG_TIME_FRAME);
    dprintln(PRINT_LOG_WRITE, "Logged Log info (in minutes):");
    dprint(PRINT_LOG_WRITE, "Datalog Active :\t"); dprintln(PRINT_LOG_WRITE, data_logging_active);
    dprint(PRINT_LOG_WRITE, "Start time     :\t"); dprintln(PRINT_LOG_WRITE, LOG_START_DELAY / ONE_MINUTE);
    dprint(PRINT_LOG_WRITE, "End time       :\t"); dprintln(PRINT_LOG_WRITE, (LOG_START_DELAY + LOG_TIME_FRAME) / ONE_MINUTE);
    dprint(PRINT_LOG_WRITE, "Logging Rate   :\t"); dprintln(PRINT_LOG_WRITE, (String)(LOG_POLLING_RATE / ONE_MINUTE));

    EEPROM.update(EEPROM_SERIAL_ID, SERIAL_ID);
    dprint(PRINT_LOG_WRITE, "logged serial number : ");
    dprintln(PRINT_LOG_WRITE, SERIAL_ID);

    // the software and hardware version numbers
    dprint(PRINT_LOG_WRITE, "Software Version:\t"); 
    dprint(PRINT_LOG_WRITE, S_VERSION_MAJOR);
    dprint(PRINT_LOG_WRITE,".");dprint(PRINT_LOG_WRITE, S_VERSION_MINOR);
    dprint(PRINT_LOG_WRITE,".");dprintln(PRINT_LOG_WRITE, S_SUBVERSION);
    EEPROM.update(EEPROM_S_SUBVERION, S_SUBVERSION);
    EEPROM.update(EEPROM_S_VERSION_MINOR, S_VERSION_MINOR);
    EEPROM.update(EEPROM_S_VERSION_MAJOR, S_VERSION_MAJOR);

    dprint(PRINT_LOG_WRITE, "hardware Version:\t"); 
    dprint(PRINT_LOG_WRITE, H_VERSION_MAJOR);
    dprint(PRINT_LOG_WRITE,".");dprint(PRINT_LOG_WRITE, H_VERSION_MINOR);
    EEPROM.update(EEPROM_H_VERSION_MINOR, H_VERSION_MINOR);
    EEPROM.update(EEPROM_H_VERSION_MAJOR, H_VERSION_MAJOR);
    
    dprintln(PRINT_LOG_WRITE, "\nFinished logging setup configs to EEPROM");
    dprintln(PRINT_LOG_WRITE, "|||||||||||||||||||||||||||||||||||||||||");
  }
}

void mothSetup() {
  Serial.begin(57600);
  delay(5000);
  Serial.println("Setup Loop has started");
  leds.begin();
  Serial.println("LEDS have been initalised");
  delay(250);
  // create either front and back led group, or just one for both
  neos[0].colorWipe(120, 70, 0); // turn off the LEDs
  neos[1].colorWipe(120, 70, 0); // turn off the LEDs
  Serial.println("Leds turned yellow for setup loop\n");
  delay(2000);

  if (JUMPERS_POPULATED) {
    printMinorDivide();
    Serial.println("Checking Hardware Jumpers");
    readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  if (PRINT_EEPROM_CONTENTS  > 0) {
    delay(1000);
    printEEPROMContents();
  } else {
    Serial.println("Not printing the EEPROM Datalog Contents");
  }
  Serial.println("Running Use Specific Setup Loop...");
  cicadaSetup();

  Serial.println("Testing Microphones");
  printTeensyDivide();
  testMicrophones();

  if (data_logging_active) {
    writeSetupConfigsToEEPROM();
  }
  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_sensors[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_sensors[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    lux_sensors[0].calibrate(LUX_CALIBRATION_TIME);
    lux_sensors[1].calibrate(LUX_CALIBRATION_TIME);
  }
  printMajorDivide("Setup Loop Finished");
}

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

void updateSong() {
  // SONG /////////////////////////
  calculateSongAudioFeatures();
  songDisplay();
}

void mothLoop() {
  tenSecondUpdate(); // this creates a slower ten second update loop (should likely be using a timer inturrupt thing)
  updateLuxSensors(); 
  updateSong();
  updateClicks();
}

#endif // __MODE_CICADA_H__
