/*
  #ifndef __CICADA_SONG_H__
  #define __CICADA_SONG_H__

  #include "Mode_Cicada.h"
  #include "logging_cicada.h"
  #include "Neos.h"
*/
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

void calculateSongAudioFeatures() {
  // TODO, rework the whole calculate weighted song brigtness to something that makes more sense
  // TODO rework to only calculate the features which are the features used
  if (song_rms1.available()) {
    double song_rms_val = song_rms1.read();
    song_rms_weighted[0] = map(constrain((song_rms_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
  }
  if (song_rms2.available()) {
    double song_rms_val = song_rms2.read();
    song_rms_weighted[1] = map(constrain((song_rms_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
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
  if (song_peak2.available()) {
    // random magiv 5 varaible needs to be better defined/tested TODO
    double song_peak_val = song_peak2.read();
    song_peak_weighted[1] = map(constrain((song_peak_val * 20), 0, 1.0), 0, 1.0, 0, MAX_BRIGHTNESS);
#if (AUTO_GAIN == 1)
    num_song_peaks[1]++;
    total_song_peaks[1] += song_peak_val * 100;
#endif// autogain

    // will only print if flag is set
    printSongStats();
  }
  // TODO, perhaps add another feature or two, perhaps an option for combining the two readings?
}

void songDisplay() {
  for (int i = 0; i < num_channels; i++) {
    // if (flash_on[i] == false) {
    // TODO make a user control which allows for selection between RMs and peak
    if (SONG_FEATURE == "peak") {
      if (stereo_audio == false) {
        neos[0].colorWipe(song_peak_weighted[i], 0, 0);
        neos[1].colorWipe(song_peak_weighted[i], 0, 0);
      } else {
        neos[i].colorWipe(song_peak_weighted[i], 0, 0);
        // colorWipeRear(song_peak_weighted_r, 0, 0);
      }
    } else if (SONG_FEATURE == "rms") {
      if (stereo_audio == false) {
        neos[0].colorWipe(song_rms_weighted[i], 0, 0);
        neos[1].colorWipe(song_rms_weighted[i], 0, 0);
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
  //}
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
  }
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
// void updateClickGain(double click_gain[], double click_gain_min[], double click_gain_max[]);

void flashUpdate() {
  for (int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++){
    neos[i].updateFlash();
  }
}

void updateClickGain(double click_gain[], double click_gain_min[], double click_gain_max[]) {
  //
}
// #endif // CICADA_CONFIG_H
