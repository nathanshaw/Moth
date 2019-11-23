// This section of code is for when the Moth is in pitch mode instead of cicada mode
// This mode will map the pitch content of the sounds it hears to different colors
// it will use the FFT 255 and map the bins to HSB instead of RGB

// this file will build the firmware just for this purpose with the intent of later creating a class which
// then is initalised dynamically depending on the position of a jumper on the PCB...

// lower pitches will be blue
// followed by purple
// green
// then yellow
// orange
// red
// white will be noise

// the brightness will be mapped by taking the rms or peak of the overall signal
#if PITCH_MODE_ACTIVE == true
AudioInputI2S            i2s1;           //xy=70,94.00000095367432
AudioAmplifier           input_amp;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256      fft256;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency note_freq;      //xy=584.0000305175781,201.00000476837158
AudioConnection          patchCord1(i2s1, 0, input_amp, 0);
AudioConnection          patchCord2(input_amp, biquad);
AudioConnection          patchCord5(biquad, tone_detect);
AudioConnection          patchCord6(biquad, peak);
AudioConnection          patchCord7(biquad, fft256);
AudioConnection          patchCord8(biquad, rms);
AudioConnection          patchCord9(biquad, note_freq);

double fft_vals[128]; 

void mapFFTToColor() {
  // determine which bin has the most energy
  
}

int getIndexWithHighestVal(double &vals) {
  double m = 0.0;
  int index = 0;  for (int i = 0; i < sizeof(vals)/sizeof(vals[0]); i++) {
    if (vals[i] > m) {
      m = vals[i];
      index = i;
    }
  }
  return index;
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
}
#endif // PITCH_MODE_ACTIVE
