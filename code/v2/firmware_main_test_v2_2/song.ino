void songPrint(String data) {
  if (PRINT_SONG_DATA) {
    Serial.print(data);
  }
}

void songPrintLn() {
  if (PRINT_SONG_DATA) {
    Serial.println();
  }
}

void songPrintLn(String data) {
  if (PRINT_SONG_DATA) {
    Serial.println(data);
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

void updateSongGain() {
  // todo right now it just updates the song gain for everything
  song_input_amp1.gain(song_gain[0]);
  song_mid_amp1.gain(song_gain[0]);
  song_post_amp1.gain(song_gain[0]);
  song_input_amp2.gain(song_gain[1]);
  song_mid_amp2.gain(song_gain[1]);
  song_post_amp2.gain(song_gain[1]);
  if (song_gain[0] > song_gain_max[0]) {
    song_gain_max[0] = song_gain[0];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX_FRONT, song_gain_max[0]);
    songPrint("logged new front song gain high : "); songPrintLn(song_gain_max[0]);
  } else if (song_gain[0] < song_gain_min[0]) {
    song_gain_min[0] = song_gain[0];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN_FRONT, song_gain_min[0]);
    songPrint("logged new front song gain low  : "); songPrintLn(song_gain_min[0]);
  }
  if (song_gain[1] > song_gain_max[1]) {
    song_gain_max[1] = song_gain[1];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MAX_REAR, song_gain_max[1]);
    songPrint("logged new rear  song gain high : "); songPrintLn(song_gain_max[1]);
  } else if (song_gain[1] < song_gain_min[1]) {
    song_gain_min[1] = song_gain[1];
    writeDoubleToEEPROM(EEPROM_SONG_GAIN_MIN_REAR, song_gain_min[1]);
    songPrint("logged new rear song gain low   : "); songPrintLn(song_gain_min[1]);
  }
  songPrint("\nupdated song gain front/rear:\t");
  songPrint(song_gain[0]);
  writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT_FRONT, song_gain[0]);
  songPrint("\t");
  writeDoubleToEEPROM(EEPROM_SONG_GAIN_CURRENT_REAR, song_gain[1]);
  songPrint(song_gain[1]);
  songPrintLn();
  songPrintLn("------------------------");
}

void printSongStats() {
  for (int i = 0; i < num_channels; i++) {
    songPrint("Song -- "); songPrint(i); songPrint(" | rms_weighted: ");
    songPrint(song_rms_weighted[i]);
    songPrint("\t peak: ");
    songPrint(song_peak_weighted[i]);
    songPrintLn();
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
#if (AUTO_GAIN == 1)
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
          colorWipe(song_rms_weighted[i], 0, 0, i);
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
