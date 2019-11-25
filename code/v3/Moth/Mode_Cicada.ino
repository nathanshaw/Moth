// #ifndef __CIDADA_MODE_H__
// #define __CIDADA_MODE_H__

void cicadaLoop() {
  updateSong();
  updateClicks();
}

void updateSong() {
  // SONG /////////////////////////
  calculateSongAudioFeatures();
  songDisplay();
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
  flashUpdate();
};



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
  Serial.print("First Click BiQuads HP-HP-HP-LS:\t");
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
  Serial.print("Second Click BiQuads HP-HP-HP-LS:\t");
  Serial.print("thresh:\t"); Serial.print(CLICK_BQ2_THRESH); Serial.print("\tQ\t");
  Serial.print(CLICK_BQ2_Q); Serial.print("\tdB"); Serial.println(CLICK_BQ2_DB);

  click_input_amp1.gain(click_gain[0]);
  click_mid_amp1.gain(click_gain[0]);
  click_post_amp1.gain(click_gain[0]);
  click_input_amp2.gain(click_gain[1]);
  click_mid_amp2.gain(click_gain[1]);
  click_post_amp2.gain(click_gain[1]);
  Serial.print("Click gains all set to          :\t");
  Serial.print(click_gain[0]); Serial.print("\t"); Serial.println(click_gain[1]);

  // Audio for the song channel...
  song_biquad1.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad1.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);
  song_biquad2.setHighpass(0, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setHighpass(1, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setHighpass(2, SONG_BQ1_THRESH, SONG_BQ1_Q);
  song_biquad2.setLowShelf(3, SONG_BQ1_THRESH, SONG_BQ1_DB);
  Serial.print("First Song BiQuads HP-HP-HP-LS:\t");
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
  Serial.print("Second Song BiQuads HP-HP-HP-LS:\t");
  Serial.print("thresh:\t"); Serial.print(SONG_BQ1_THRESH); Serial.print("\tQ\t");
  Serial.print(SONG_BQ2_Q); Serial.print("\tdB"); Serial.println(SONG_BQ2_DB);

  song_input_amp1.gain(song_gain[0]);
  song_mid_amp1.gain(song_gain[0]);
  song_post_amp1.gain(song_gain[0]);
  song_input_amp2.gain(song_gain[1]);
  song_mid_amp2.gain(song_gain[1]);
  song_post_amp2.gain(song_gain[1]);
  Serial.print("Song gains all set to: ");
  Serial.print(song_gain[0]); Serial.print("\t"); Serial.println(song_gain[1]);
  Serial.println();
  delay(1000);

  /////////////////////////////////
  // VEML sensors through TCA9543A
  /////////////////////////////////

  Serial.println("\n- - - - - - - - - - - - - -");
  Serial.println("Searching for Lux Sensors");
  // setupVEMLthroughTCA();
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
  neos[0].colorWipe(0, 0, 0); // turn off the LEDs
  neos[1].colorWipe(0, 0, 0); // turn off the LEDs
  Serial.println("\nFinshed cicada Setup Loop");
  printDivide();
}

void updateClickAudioFeaturesRMS(uint8_t i) {
  click_rms_delta[i] = last_click_rms_val[i] - click_rms_val[i];
  if (click_rms_delta[i] > CLICK_RMS_DELTA_THRESH) {
    // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
    // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
    neos[i].flashOn(); // all flash logic is conducted in the NeoGroup instance
  }
}

void updateClickAudioFeaturesPeak(uint8_t i) {
  click_peak_delta[i] = last_click_peak_val[i]  - click_peak_val[i];
  if (click_peak_delta[i] > CLICK_PEAK_DELTA_THRESH) {
    neos[i].flashOn();
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
  for (unsigned int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++) {
    neos[i].updateFlash();
  }
}

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
  while (readings < 10 && millis() < a_time + 4000) {
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
  while (readings < 10 && millis() < a_time + 4000) {
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
