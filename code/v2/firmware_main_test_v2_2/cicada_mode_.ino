///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Audio Library /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
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

// audio usage loggings
uint8_t audio_usage_max = 0;
elapsedMillis last_usage_print = 0;// for keeping track of audio memory usage
#define AUDIO_MEMORY 24

// keeping track of clicks
long total_clicks_detected[2] = {0, 0}; // number of clicks which has occurred since boot
long num_past_clicks[2];            // number of clicks since last auto-gain adjustment
long num_cpm_clicks[2];

// click gain
double click_gain[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN}; // starting click gain level
double click_gain_min[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN};
double click_gain_max[2] = {STARTING_CLICK_GAIN, STARTING_CLICK_GAIN};

// click rms
double click_rms_val[2] = {0.0, 0.0};
double last_click_rms_val[2] = {0.0, 0.0};
double click_rms_delta[2] = {0.0, 0.0};

// click peak
double click_peak_val[2] = {0.0, 0.0};
double last_click_peak_val[2] = {0.0, 0.0};
double click_peak_delta[2] = {0.0, 0.0};

// song gain
double song_gain[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};   // starting song gain level
double song_gain_min[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};
double song_gain_max[2] = {STARTING_SONG_GAIN, STARTING_SONG_GAIN};

// song rms
uint8_t song_rms_weighted[2] = {0, 0};  // 0 -255 depending on the RMS of the song band...

// song peak
uint8_t song_peak_weighted[2] = {0, 0}; // 0 -255 depending on the peak of the song band...
double total_song_peaks[2];
unsigned long num_song_peaks[2];

void cicadaLoop() {
  // SONG /////////////////
  calculateSongAudioFeatures();  
  songDisplay();
  // Click ////////////////
  // todo add this back
  calculateClickAudioFeatures();
  // printClickStats();
  clickFlashUpdate();
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
  setupVEMLthroughTCA();
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
  colorWipe(0, 0, 0, 0); // turn off the LEDs
  colorWipe(0, 0, 0, 1); // turn off the LEDs

  Serial.println("\nFinshed cicada Setup Loop");
  printDivide();
}
