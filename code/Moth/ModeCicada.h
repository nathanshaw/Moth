#ifndef __MODE_CICADA_H__
#define __MODE_CICADA_H__
#include <Audio.h>
#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include "Configuration_cicadas.h"
#include "NeopixelManager/NeopixelManager.h"
#include "LuxManager/LuxManager.h"
#include "AudioEngine/AudioEngine.h"
#include <Audio.h>

//////////////////////////////// Global Objects /////////////////////////

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

NeoGroup neos[2] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 10, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_SENSORS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

DLManager datalog_manager = DLManager((String)"Main");

FeatureCollector fc[4] = {FeatureCollector("front song"), FeatureCollector("rear song"), FeatureCollector("front click"), FeatureCollector("rear click")};

////////////////////////// Audio Objects //////////////////////////////////////////

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

void linkFeatureCollectors() {
  if (RMS_ACTIVE) {
    // fc 0-1 are for the song front/rear
    fc[0].linkRMS(&song_rms1);
    fc[1].linkRMS(&song_rms2);
    // fc 2-3 are for the click front/rear
    fc[2].linkRMS(&click_rms1);
    fc[3].linkRMS(&click_rms2);
  }
  if (PEAK_ACTIVE) {
    // fc 0-1 are for the song front/rear
    fc[0].linkPeak(&song_peak1);
    fc[1].linkPeak(&song_peak2);
    // fc 2-3 are for the click front/rear
    fc[2].linkPeak(&click_peak1);
    fc[3].linkPeak(&click_peak2);
  }
}

// song rms
uint8_t song_rms_weighted[2] = {0, 0};  // 0 -255 depending on the RMS of the song band...

// song peak
uint8_t song_peak_weighted[2] = {0, 0}; // 0 -255 depending on the peak of the song band...
double total_song_peaks[2];
uint32_t num_song_peaks[2];

///////////////////////////////// General Purpose Functions //////////////////////////////////

///////////////////////////////////////////////////////////////////////
//                       TCA9532A I2C bus expander
///////////////////////////////////////////////////////////////////////
void writeAudioUsageToEEPROM(uint8_t used) {
  if (data_logging_active) {
    EEPROM.update(EEPROM_AUDIO_MEM_USAGE, used);
    dprintln(PRINT_LOG_WRITE, "logged audio memory usage to EEPROM");
  }
}

/*
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
*/

void updateLuxSensors() {
  // Return a 1 if the lux sensors are read and a 0 if they are not
  // if the LEDs have been off, on their own regard, for 40ms or longer...
  // and it has been long-enough to warrent a new reading
  // dprintln(PRINT_LUX_DEBUG,"\nchecking lux sensors: ");
  combined_lux = 0;
  for (unsigned int i = 0; i < sizeof(lux_managers) / sizeof(lux_managers[0]); i++) {
    // for each lux sensor, if the LEDs are off, have been off for longer than the LED_SHDN_LEN
    // and it has been longer than the
    // min reading delay then read the sensors
    lux_managers[i].update();
    combined_lux += lux_managers[i].getLux();
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

// this should be in the main audio loop
void updateClicks() {
  for (unsigned int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++) {
    neos[i].updateFlash();
  }
};
/*
  void updateSongGain(double song_gain[]) {
  // todo right now it just updates the song gain for everything
  for (unsigned int i = 0; i < num_channels; i++) {
    if (i == 0) {
      song_input_amp1.gain(song_gain[i]);
      song_mid_amp1.gain(song_gain[i]);
      song_post_amp1.gain(song_gain[i]);
      dprint(PRINT_SONG_DATA, "\nupdated song gain "); dprint(PRINT_SONG_DATA, frText(i)); dprint(PRINT_SONG_DATA, ":\t");
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
*/

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

  click_input_amp1.gain(fc[2].gain);
  click_mid_amp1.gain(fc[2].gain);
  click_post_amp1.gain(fc[2].gain);
  click_input_amp2.gain(fc[3].gain);
  click_mid_amp2.gain(fc[3].gain);
  click_post_amp2.gain(fc[3].gain);
  Serial.print("\nClick gains all set to                :\t");
  Serial.print(fc[2].gain); Serial.print("\t"); Serial.println(fc[3].gain);

  song_input_amp1.gain(fc[0].gain);
  song_mid_amp1.gain(fc[0].gain);
  song_post_amp1.gain(fc[0].gain);
  song_input_amp2.gain(fc[1].gain);
  song_mid_amp2.gain(fc[1].gain);
  song_post_amp2.gain(fc[1].gain);
  Serial.print("Song gains all set to                 :\t");
  Serial.print(fc[0].gain); Serial.print("\t"); Serial.println(fc[1].gain);
  delay(1000);

  /////////////////////////////////
  // VEML sensors through TCA9543A
  /////////////////////////////////
  /*
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
  */
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

/*
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
*/

bool adjustSongGainLedOnRatio() {
  bool success = false;
  for (int i = 0; i < num_channels; i++) {
    double cost = 0.5;
    if (neos[i].getOnRatio() > MAX_LED_ON_RATIO) {
      double change = fc[i].gain * MAX_GAIN_ADJUSTMENT * cost;
      fc[i].gain -= change;
      // ensure that what we have is not less than the min
      fc[i].gain = max(fc[i].gain, MIN_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "led_on_ratio is too high ("); dprint(PRINT_AUTO_GAIN, neos[i].getOnRatio());
      dprint(PRINT_AUTO_GAIN, ") lowering the song gain "); dprintln(PRINT_AUTO_GAIN, i);
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      success = true;
    } else if (neos[i].getOnRatio() < MIN_LED_ON_RATIO) {
      double change = fc[i].gain * MAX_GAIN_ADJUSTMENT * cost;
      fc[i].gain += change;
      // ensure that what we have is not less than the min
      fc[i].gain = min(fc[i].gain, MAX_SONG_GAIN);
      dprint(PRINT_AUTO_GAIN, "led_on_ratio is too low ("); dprint(PRINT_AUTO_GAIN, neos[i].getOnRatio());
      dprint(PRINT_AUTO_GAIN, ") raising the song gain "); dprintln(PRINT_AUTO_GAIN, i);
      dprint(PRINT_AUTO_GAIN, change);
      dprint(PRINT_AUTO_GAIN, " ");
      success = true;
    }
  }
  if (success) {
    // todo add back
    // updateSongGain(song_gain);
    // updateSongGainMinMax();
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
      double change = fc[i].gain * MAX_GAIN_ADJUSTMENT * cost;
      fc[i].gain -= change;
      // ensure that what we have is not less than the min
      fc[i].gain = max(fc[i].gain, MIN_SONG_GAIN);
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
      double change = fc[i].gain * MAX_GAIN_ADJUSTMENT * cost;
      fc[i].gain += change;
      // ensure what we have is not less than the max...
      fc[i].gain = min(fc[i].gain, MAX_SONG_GAIN);
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
    // todo add back in
    // updateSongGain(song_gain);
    // updateSongGainMinMax();
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
        fc[i + 2].gain += fc[i + 2].gain * MAX_GAIN_ADJUSTMENT;
        fc[i + 2].gain = min(fc[i + 2].gain, MAX_CLICK_GAIN);
        update_gain = true;
      }
      // then check if there are too few clicks_per_minute
      else if (clicks_per_minute < MIN_CLICKS_PER_MINUTE) {
        // there are too few clicks, need to increase the gain to compensate
        // first we calculate the factor by which the clicks are off
        // the higher the number the more it is off
        cost = 1.0 - (clicks_per_minute / MIN_CLICKS_PER_MINUTE);              // max amount that can be adjusted
        fc[i + 2].gain += fc[i + 2].gain * MAX_GAIN_ADJUSTMENT * cost;
        fc[i + 2].gain = min(fc[i + 2].gain, MAX_CLICK_GAIN);                // make sure that the click_gain is not more than the max_click_gain
        update_gain = true;
      }
      // then check to see if there are too many clicks per-minute
      else if (clicks_per_minute > MAX_CLICKS_PER_MINUTE) {
        cost = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute);              // determine a cost function... the higher the value the more the cost
        fc[i + 2].gain -= fc[i + 2].gain * MAX_GAIN_ADJUSTMENT * cost;       // adjust the click_gain
        fc[i + 2].gain = max(fc[i + 2].gain, MIN_CLICK_GAIN);                // make sure that the gain is not set too low
        update_gain = true;
      }
      num_past_clicks[i] = 0;
    }
  }
  if (update_gain == true) {
    Serial.print("ERROR update_gain unimplemented");
    // updateClickGain(click_gain, click_gain_min, click_gain_max);          // update the click gain in the three gain stages
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
  if (last_auto_gain_adjustment < autogain_frequency || millis() < 60000) {
    return;
  };
  dprintln(PRINT_AUTO_GAIN, "-------------------- Auto Gain Start ---------------------------");

  // todo add back
  // resetOnOffRatioCounters();
  adjustSongGainLedOnRatio();

  checkClickAutoGain();
  checkSongAutoGain();

  last_auto_gain_adjustment = 0;
  dprintMinorDivide(PRINT_AUTO_GAIN);
}

void setupDLManager() {
  // log data to EEPROM if datalogging is active
  if (data_logging_active) {
    Serial.println("configuring datalog_manager timers");
    datalog_manager.configureAutoTimer(0, DATALOG_START_DELAY_1, DATALOG_TIME_FRAME_1, DATALOG_1_LENGTH);
    datalog_manager.configureAutoTimer(1, DATALOG_START_DELAY_2, DATALOG_TIME_FRAME_2, DATALOG_2_LENGTH);
    datalog_manager.configureStaticTimer(2, DATALOG_START_DELAY_3, STATICLOG_RATE_FAST);
    datalog_manager.configureStaticTimer(3, DATALOG_START_DELAY_4, STATICLOG_RATE_SLOW);
    datalog_manager.printTimerConfigs();

    // Hardware / Software / Serial Numbers
    datalog_manager.logSetupConfigByte("Hardware Version majo       : ", H_VERSION_MAJOR);
    datalog_manager.logSetupConfigByte("Hardware Version mino       : ", H_VERSION_MINOR);
    datalog_manager.logSetupConfigByte("Software Version majo       : ", S_VERSION_MAJOR);
    datalog_manager.logSetupConfigByte("Software Version majo       : ", S_VERSION_MINOR);
    datalog_manager.logSetupConfigByte("Software Version majo       : ", S_SUBVERSION);
    datalog_manager.logSetupConfigByte("Bot ID Number               : ", SERIAL_ID);
    datalog_manager.logSetupConfigByte("Datalog Active              : ", data_logging_active);
    datalog_manager.logSetupConfigByte("Firmware Mode               : ", FIRMWARE_MODE);
    // Lux Sensors
    datalog_manager.logSetupConfigByte("Smooth Lux Readings         : ", SMOOTH_LUX_READINGS);
    datalog_manager.logSetupConfigDouble("Lux Low Threshold           : ", LOW_LUX_THRESHOLD);
    datalog_manager.logSetupConfigDouble("Lux Mid Threshold           : ", MID_LUX_THRESHOLD);
    datalog_manager.logSetupConfigDouble("Lux High Threshold          : ", HIGH_LUX_THRESHOLD);
    datalog_manager.logSetupConfigDouble("Brightness Scaler Min       : ", BRIGHTNESS_SCALER_MIN);
    datalog_manager.logSetupConfigDouble("Brightness Scaler Max       : ", BRIGHTNESS_SCALER_MAX);
    datalog_manager.logSetupConfigByte("Min Brightness              : ", MIN_BRIGHTNESS);
    datalog_manager.logSetupConfigByte("Max Brightness              : ", MAX_BRIGHTNESS);
    // Auto Gain
    datalog_manager.logSetupConfigByte("Autogain Active             : ", BRIGHTNESS_SCALER_MAX);
    datalog_manager.logSetupConfigDouble("Max Autogain Adjustment     : ", MAX_GAIN_ADJUSTMENT);
    datalog_manager.logSetupConfigLong("Autogain Frequency          : ", autogain_frequency);
    // Autolog settings
    datalog_manager.logSetupConfigLong("Timer 0 Start Time             : ", datalog_manager.getTimerStart(0));
    datalog_manager.logSetupConfigLong("Timer 0 End Time               : ", datalog_manager.getTimerEnd(0));
    datalog_manager.logSetupConfigLong("Timer 0 Logging Rate           : ", datalog_manager.getTimerRate(0));
    datalog_manager.logSetupConfigLong("Timer 1 Start Time             : ", datalog_manager.getTimerStart(1));
    datalog_manager.logSetupConfigLong("Timer 1 End Time               : ", datalog_manager.getTimerEnd(1));
    datalog_manager.logSetupConfigLong("Timer 1 Logging Rate           : ", datalog_manager.getTimerRate(1));
    datalog_manager.logSetupConfigLong("Timer 2 Start Time             : ", datalog_manager.getTimerStart(2));
    datalog_manager.logSetupConfigLong("Timer 2 End Time               : ", datalog_manager.getTimerEnd(2));
    datalog_manager.logSetupConfigLong("Timer 2 Logging Rate           : ", datalog_manager.getTimerRate(2));
    datalog_manager.logSetupConfigLong("Timer 3 Start Time             : ", datalog_manager.getTimerStart(3));
    datalog_manager.logSetupConfigLong("Timer 3 End Time               : ", datalog_manager.getTimerEnd(3));
    datalog_manager.logSetupConfigLong("Timer 3 Logging Rate           : ", datalog_manager.getTimerRate(3));

    // the constantly updating logs
    if (STATICLOG_LUX_VALUES) {
      datalog_manager.addStaticLog("Lowest Front Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[0].min_reading );
      datalog_manager.addStaticLog("Highest Front Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[0].max_reading );
      datalog_manager.addStaticLog("Lowest Rear Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[1].min_reading );
      datalog_manager.addStaticLog("Highest Rear Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[1].max_reading );

    }
    if (STATICLOG_SONG_GAIN) {
      datalog_manager.addStaticLog("Lowest Front Song Gain  : ",
                                   STATICLOG_SONG_GAIN_TIMER, &fc[0].min_gain);
      datalog_manager.addStaticLog("Highest Front Song Gain ",
                                   STATICLOG_SONG_GAIN_TIMER, &fc[0].max_gain);
      datalog_manager.addStaticLog("Lowest Rear Song Gain  : ",
                                   STATICLOG_SONG_GAIN_TIMER, &fc[1].min_gain);
      datalog_manager.addStaticLog("Highest Rear Song Gain ",
                                   STATICLOG_SONG_GAIN_TIMER, &fc[1].max_gain);
      datalog_manager.logSetupConfigDouble("Song Starting Gain   : ", STARTING_SONG_GAIN);

    }
    if (STATICLOG_CLICK_GAIN) {
      datalog_manager.addStaticLog("Lowest Front Click Gain  : ",
                                   STATICLOG_CLICK_GAIN_TIMER, &fc[2].min_gain);
      datalog_manager.addStaticLog("Highest Front Click Gain ",
                                   STATICLOG_CLICK_GAIN_TIMER, &fc[2].max_gain);
      datalog_manager.addStaticLog("Lowest Rear Click Gain  : ",
                                   STATICLOG_CLICK_GAIN_TIMER, &fc[3].min_gain);
      datalog_manager.addStaticLog("Highest Rear Click Gain ",
                                   STATICLOG_CLICK_GAIN_TIMER, &fc[3].max_gain);
      datalog_manager.logSetupConfigDouble("Click Starting Gain  : ", STARTING_CLICK_GAIN);
    }

    if (STATICLOG_FLASHES) {
      datalog_manager.addStaticLog("Front Total Flashes Detected  : ",
                                   STATICLOG_FLASHES_TIMER, &neos[0].total_flashes);
      datalog_manager.addStaticLog("Rear Total Flashes Detected   : ",
                                   STATICLOG_FLASHES_TIMER, &neos[1].total_flashes);
    }

    // todo double check the addr
    // Datalog lux_log_f = Datalog(EEPROM_LUX_LOG_START, "Lux Front", lux_managers->lux, true);
    // datalog_manager.startAutolog(0);
    double * ptr;
    if (AUTOLOG_LUX_F > 0 && front_lux_active > 0) {
      ptr = &lux_managers[0].lux;
      Serial.println("adding front lux autolog to datalog_manager");
      datalog_manager.addAutolog("Front Lux Log ", AUTOLOG_LUX_TIMER, ptr);
    }
    if (AUTOLOG_LUX_R > 0 && rear_lux_active > 0) {
      ptr = &lux_managers[1].lux;
      Serial.println("adding rear lux autolog to datalog_manager");
      datalog_manager.addAutolog("Rear Lux Log ", AUTOLOG_LUX_TIMER, ptr);
    }

    if (AUTOLOG_LED_ON_OFF_F > 0) {
      ptr = &neos[0].on_ratio;
      Serial.println("adding front led on/off ratio autolog to datalog_manager");
      datalog_manager.addAutolog("Front Led On/Off Ratio Log ", AUTOLOG_LED_ON_OFF_TIMER, ptr);
    }
    if (AUTOLOG_LED_ON_OFF_R > 0) {
      ptr = &neos[1].on_ratio;
      Serial.println("adding rear led on/off ratio autolog to datalog_manager");
      datalog_manager.addAutolog("Rear Led On/Off Ratio Log ", AUTOLOG_LED_ON_OFF_TIMER, ptr);
    }
    if (AUTOLOG_BRIGHTNESS_SCALER_F > 0) {    
      ptr = &lux_managers[0].brightness_scaler_avg;
      lux_managers[0].resetBrightnessScalerAvg();
      Serial.println("adding front brightness scaler autolog to datalog_manager");
      datalog_manager.addAutolog("Front Brightness Scaler Averages ", AUTOLOG_BRIGHTNESS_SCALER_TIMER, ptr);    
    }
    if (AUTOLOG_BRIGHTNESS_SCALER_R > 0) {    
      ptr = &lux_managers[1].brightness_scaler_avg;
      lux_managers[1].resetBrightnessScalerAvg();
      Serial.println("adding rear brightness scaler autolog to datalog_manager");
      datalog_manager.addAutolog("Rear Brightness Scaler Averages  ", AUTOLOG_BRIGHTNESS_SCALER_TIMER, ptr);    
    }
// the brightness scaler avg log
#define AUTOLOG_BRIGHTNESS_SCALER_F     1
#define AUTOLOG_BRIGHTNESS_SCALER_R     1
#define AUTOLOG_BRIGHTNESS_SCALER_TIMER 0
    
    uint32_t * lptr;
    if (AUTOLOG_FLASHES_F > 0) {
      lptr = &neos[0].num_flashes;
      Serial.println("adding front led flash number  autolog to datalog_manager");
      datalog_manager.addAutolog("Front Led Flash Number Log ", AUTOLOG_FLASHES_TIMER, lptr);
    }
    if (AUTOLOG_FLASHES_R > 0) {
      lptr = &neos[1].num_flashes;
      Serial.println("adding rear led flash number autolog to datalog_manager");
      datalog_manager.addAutolog("Rear Led Flash Number Log ", AUTOLOG_FLASHES_TIMER, lptr);
    }

    // printing needs to be at the end so that everything actually displays
    if (PRINT_EEPROM_CONTENTS  > 0) {
      delay(1000);
      datalog_manager.printAllLogs();
    } else {
      Serial.println("Not printing the EEPROM Datalog Contents");
    }


  } else {
    if (PRINT_EEPROM_CONTENTS > 0) {
      datalog_manager.printAllLogs();
    }
  }
  if (CLEAR_EEPROM_CONTENTS > 0) {
    delay(100);
    datalog_manager.clearLogs();
    // datalog_manager.printLogs();
  } else {
    Serial.println("Not printing the EEPROM Datalog Contents");
  }
}

void mothSetup() {
  Serial.begin(57600); delay(5000); Serial.println("Setup Loop has started");
  if (JUMPERS_POPULATED) {
    // readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  leds.begin(); Serial.println("LEDS have been initalised"); delay(250);
  // create either front and back led group, or just one for both
  neos[0].colorWipe(120, 70, 0); // turn off the LEDs
  neos[1].colorWipe(120, 70, 0); // turn off the LEDs
  Serial.println("Leds turned yellow for setup loop\n");
  delay(2000);

  setupDLManager();

  Serial.println("Running Use Specific Setup Loop...");
  cicadaSetup();

  Serial.println("Testing Microphones");
  printTeensyDivide();
  // todo make this adapt to when microphones are broken on one or more side...
  for (int i = 0; i < num_channels; i++) {
    fc[i].testMicrophone();
  }
  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
  }
  printMajorDivide("Setup Loop Finished");
}

void mothLoop() {
  updateLuxSensors();
  // SONG /////////////////////////
  calculateSongAudioFeatures();
  songDisplay();
  updateClicks();
  fc[0].update();
  fc[2].update();
  fc[3].update();
  fc[4].update();
  datalog_manager.update();
  // todo add back
  // checkAudioUsage();
  // todo add back
  // autoGainAdjust(); // will call rear as well if in stereo mode
}
/*
  void testMicrophones() {
  if (testMicrophone(&input, front) == false) {
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
*/
#endif // __MODE_CICADA_H__
