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

// for some reason the datalog manager had trouble tracking this data?
elapsedMillis fpm_timer;
uint32_t num_flashes[2];
double total_flashes[2];
double fpm[2];
// for tracking the peak something or another?
double total_song_peaks[2];
uint32_t num_song_peaks[2];

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

DLManager datalog_manager = DLManager((String)"Datalog Manager");

FeatureCollector fc[4] = {FeatureCollector("front song"), FeatureCollector("rear song"), FeatureCollector("front click"), FeatureCollector("rear click")};

AutoGain auto_gain[2] = {AutoGain("Song", &fc[0], &fc[1], MIN_SONG_GAIN, MAX_SONG_GAIN, MAX_GAIN_ADJUSTMENT),
                         AutoGain("Click", &fc[2], &fc[3], MIN_CLICK_GAIN, MAX_CLICK_GAIN, MAX_GAIN_ADJUSTMENT)
                        };

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

void initAutoGain() {
  auto_gain[0].setExternalThresholds((String)"Led ON Ratio", MIN_ON_RATIO_THRESH, LOW_ON_RATIO_THRESH,
                                     HIGH_ON_RATIO_THRESH, MAX_ON_RATIO_THRESH);
  auto_gain[1].setExternalThresholds((String)"FPM", MIN_FPM_THRESH, LOW_FPM_THRESH,
                                     HIGH_FPM_THRESH, MAX_FPM_THRESH);
  auto_gain[0].setUpdateRate(AUTOGAIN_FREQUENCY);
  auto_gain[1].setUpdateRate(AUTOGAIN_FREQUENCY);
  //auto_gain[2].setUpdateRate(AUTOGAIN_FREQUENCY);
  //auto_gain[3].setUpdateRate(AUTOGAIN_FREQUENCY);
  auto_gain[0].setStartDelay(AUTOGAIN_START_DELAY);
  auto_gain[1].setStartDelay(AUTOGAIN_START_DELAY);
  //auto_gain[2].setStartDelay(AUTOGAIN_START_DELAY);
  //auto_gain[3].setStartDelay(AUTOGAIN_START_DELAY);
}

void linkFeatureCollectors() {
  fc[0].linkAmplifier(&song_input_amp1, MIN_SONG_GAIN, MAX_SONG_GAIN);
  fc[0].linkAmplifier(&song_mid_amp1, MIN_SONG_GAIN, MAX_SONG_GAIN);
  fc[0].linkAmplifier(&song_post_amp1, MIN_SONG_GAIN, MAX_SONG_GAIN);
  fc[1].linkAmplifier(&song_input_amp2, MIN_SONG_GAIN, MAX_SONG_GAIN);
  fc[1].linkAmplifier(&song_mid_amp2, MIN_SONG_GAIN, MAX_SONG_GAIN);
  fc[1].linkAmplifier(&song_post_amp2, MIN_SONG_GAIN, MAX_SONG_GAIN);
  fc[2].linkAmplifier(&click_input_amp1, MIN_CLICK_GAIN, MAX_CLICK_GAIN);
  fc[2].linkAmplifier(&click_mid_amp1, MIN_CLICK_GAIN, MAX_CLICK_GAIN);
  fc[2].linkAmplifier(&click_post_amp1, MIN_CLICK_GAIN, MAX_CLICK_GAIN);
  fc[3].linkAmplifier(&click_input_amp2, MIN_CLICK_GAIN, MAX_CLICK_GAIN);
  fc[3].linkAmplifier(&click_mid_amp2, MIN_CLICK_GAIN, MAX_CLICK_GAIN);
  fc[3].linkAmplifier(&click_post_amp2, MIN_CLICK_GAIN, MAX_CLICK_GAIN);
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

///////////////////////////////// General Purpose Functions //////////////////////////////////

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

void audioSetup() {
  ////////////// Audio ////////////
  AudioMemory(AUDIO_MEMORY);
  linkFeatureCollectors();
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

  // Audio for the song channel...n
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

  fc[0].updateGain(STARTING_SONG_GAIN);
  fc[1].updateGain(STARTING_SONG_GAIN);
  fc[2].updateGain(STARTING_CLICK_GAIN);
  fc[3].updateGain(STARTING_CLICK_GAIN);

  Serial.println("Testing Microphones");
  printTeensyDivide();
  // todo make this adapt to when microphones are broken on one or more side...
  for (int i = 0; i < num_channels; i++) {
    fc[i].testMicrophone();
  }
  initAutoGain();
  printDivide();
}

uint8_t calculateRMSWeighted(FeatureCollector *f) {
  double rms = 0;
  rms = f->getRMS() * (double)RMS_SCALER;
  if (rms > 1.0) {
    rms = 1.0;
  }
  uint8_t scaler = (uint8_t)(rms * (double)MAX_BRIGHTNESS);
  return scaler;
}

uint8_t calculatePeakWeighted(FeatureCollector *f) {
  double peak = 0;
  peak = f->getPeak() * (double)PEAK_SCALER;
  if (peak > 1.0) {
    peak = 1.0;
  }
  uint8_t scaler = peak * MAX_BRIGHTNESS;
  return scaler;
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
    printMinorDivide();
    datalog_manager.logSetupConfigByte("Smooth Lux Readings         : ", SMOOTH_LUX_READINGS);
    datalog_manager.logSetupConfigDouble("Lux Low Threshold           : ", LOW_LUX_THRESHOLD);
    datalog_manager.logSetupConfigDouble("Lux Mid Threshold           : ", MID_LUX_THRESHOLD);
    datalog_manager.logSetupConfigDouble("Lux High Threshold          : ", HIGH_LUX_THRESHOLD);
    datalog_manager.logSetupConfigDouble("Brightness Scaler Min       : ", BRIGHTNESS_SCALER_MIN);
    datalog_manager.logSetupConfigDouble("Brightness Scaler Max       : ", BRIGHTNESS_SCALER_MAX);
    datalog_manager.logSetupConfigByte("Min Brightness              : ", MIN_BRIGHTNESS);
    datalog_manager.logSetupConfigByte("Max Brightness              : ", MAX_BRIGHTNESS);
    // Auto Gain
    printMinorDivide();
    datalog_manager.logSetupConfigByte("Autogain Active             : ", BRIGHTNESS_SCALER_MAX);
    datalog_manager.logSetupConfigDouble("Max Autogain Adjustment     : ", MAX_GAIN_ADJUSTMENT);
    datalog_manager.logSetupConfigLong("Autogain Frequency          : ", AUTOGAIN_FREQUENCY);
    // Autolog settings
    printMinorDivide();
    datalog_manager.logSetupConfigLong("Timer 0 Start Time           : ", datalog_manager.getTimerStart(0));
    datalog_manager.logSetupConfigLong("Timer 0 End Time             : ", datalog_manager.getTimerEnd(0));
    datalog_manager.logSetupConfigLong("Timer 0 Logging Rate         : ", datalog_manager.getTimerRate(0));
    datalog_manager.logSetupConfigLong("Timer 1 Start Time           : ", datalog_manager.getTimerStart(1));
    datalog_manager.logSetupConfigLong("Timer 1 End Time             : ", datalog_manager.getTimerEnd(1));
    datalog_manager.logSetupConfigLong("Timer 1 Logging Rate         : ", datalog_manager.getTimerRate(1));
    datalog_manager.logSetupConfigLong("Timer 2 Start Time           : ", datalog_manager.getTimerStart(2));
    datalog_manager.logSetupConfigLong("Timer 2 End Time             : ", datalog_manager.getTimerEnd(2));
    datalog_manager.logSetupConfigLong("Timer 2 Logging Rate         : ", datalog_manager.getTimerRate(2));
    datalog_manager.logSetupConfigLong("Timer 3 Start Time           : ", datalog_manager.getTimerStart(3));
    datalog_manager.logSetupConfigLong("Timer 3 End Time             : ", datalog_manager.getTimerEnd(3));
    datalog_manager.logSetupConfigLong("Timer 3 Logging Rate         : ", datalog_manager.getTimerRate(3));

    // runtime log
    if (STATICLOG_RUNTIME) {
      datalog_manager.addStaticLog("Program Runtime (minutes) : ",
                                   STATICLOG_RUNTIME_TIMER, &runtime);
    }
    // the constantly updating logs
    if (STATICLOG_LUX_VALUES) {
      datalog_manager.addStaticLog("Lowest Front Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[0].min_reading);
      datalog_manager.addStaticLog("Highest Front Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[0].max_reading);
      datalog_manager.addStaticLog("Lowest Rear Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[1].min_reading);
      datalog_manager.addStaticLog("Highest Rear Lux Recorded : ",
                                   STATICLOG_LUX_MIN_MAX_TIMER, &lux_managers[1].max_reading);
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
      datalog_manager.logSetupConfigDouble("Song Starting Gain           : ", STARTING_SONG_GAIN);

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
      datalog_manager.logSetupConfigDouble("Click Starting Gain          : ", STARTING_CLICK_GAIN);
    }

    if (STATICLOG_FLASHES) {
      datalog_manager.addStaticLog("Front Total Flashes Detected  : ",
                                   STATICLOG_FLASHES_TIMER, &total_flashes[0]);
      datalog_manager.addStaticLog("Rear Total Flashes Detected   : ",
                                   STATICLOG_FLASHES_TIMER, &total_flashes[1]);
    }

    // todo double check the addr
    // Datalog lux_log_f = Datalog(EEPROM_LUX_LOG_START, "Lux Front", lux_managers->lux, true);
    // datalog_manager.startAutolog(0);
    double * ptr;
    if (AUTOLOG_LUX_F > 0 && front_lux_active > 0) {
      datalog_manager.addAutolog("Front Lux Log ", AUTOLOG_LUX_TIMER, &lux_managers[0].lux);
    }
    if (AUTOLOG_LUX_R > 0 && rear_lux_active > 0) {
      datalog_manager.addAutolog("Rear Lux Log ", AUTOLOG_LUX_TIMER, &lux_managers[1].lux);
    }

    if (AUTOLOG_LED_ON_OFF_F > 0) {
      ptr = &neos[0].on_ratio;
      datalog_manager.addAutolog("Front Led On/Off Ratio Log ", AUTOLOG_LED_ON_OFF_TIMER, ptr);
    }
    if (AUTOLOG_LED_ON_OFF_R > 0) {
      ptr = &neos[1].on_ratio;
      datalog_manager.addAutolog("Rear Led On/Off Ratio Log ", AUTOLOG_LED_ON_OFF_TIMER, ptr);
    }
    if (AUTOLOG_BRIGHTNESS_SCALER_F > 0) {
      ptr = &lux_managers[0].brightness_scaler_avg;
      lux_managers[0].resetBrightnessScalerAvg();
      datalog_manager.addAutolog("Front Brightness Scaler Averages ", AUTOLOG_BRIGHTNESS_SCALER_TIMER, ptr);
    }
    if (AUTOLOG_BRIGHTNESS_SCALER_R > 0) {
      ptr = &lux_managers[1].brightness_scaler_avg;
      lux_managers[1].resetBrightnessScalerAvg();
      datalog_manager.addAutolog("Rear Brightness Scaler Averages  ", AUTOLOG_BRIGHTNESS_SCALER_TIMER, ptr);
    }
    if (AUTOLOG_FPM_F > 0) {
      ptr = &neos[0].fpm;
      lux_managers[1].resetBrightnessScalerAvg();
      datalog_manager.addAutolog("Front Flashes Per Minute  ", AUTOLOG_FPM_TIMER, ptr);
    }
    if (AUTOLOG_FPM_R > 0) {
      ptr = &neos[1].fpm;
      lux_managers[1].resetBrightnessScalerAvg();
      datalog_manager.addAutolog("Rear Flashes Per Minute  ", AUTOLOG_FPM_TIMER, ptr);
    }

    if (AUTOLOG_FLASHES_F > 0) {
      datalog_manager.addAutolog("Front Led Flash Number Log ", AUTOLOG_FLASHES_TIMER, &total_flashes[0]);
    }
    if (AUTOLOG_FLASHES_R > 0) {
      datalog_manager.addAutolog("Rear Led Flash Number Log ", AUTOLOG_FLASHES_TIMER, &total_flashes[1]);
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
  } else {
    Serial.println("Not printing the EEPROM Datalog Contents");
  }
}

void mothSetup() {
  Serial.begin(57600);
  leds.begin();
  delay(2000);
  Serial.println("LEDS have been initalised");
  neos[0].colorWipe(250, 90, 0); // turn off the LEDs
  neos[1].colorWipe(250, 90, 0); // turn off the LED
  delay(3000); Serial.println("Setup Loop has started");
  if (JUMPERS_POPULATED) {
    // readJumpers();
  } else {
    printMajorDivide("Jumpers are not populated, not printing values");
  }
  // create either front and back led group, or just one for both
  neos[0].colorWipe(120, 70, 0); // turn off the LEDs
  neos[1].colorWipe(120, 70, 0); // turn off the LEDs
  Serial.println("Leds turned yellow for setup loop\n");
  delay(1000);
  setupDLManager();
  neos[0].colorWipe(100, 150, 0); // turn off the LEDs
  neos[1].colorWipe(100, 150, 0); // turn off the LEDs
  Serial.println("Running Use Specific Setup Loop...");
  audioSetup();
  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo

    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    neos[0].colorWipe(0, 0, 0); // turn off the LEDs
    neos[1].colorWipe(0, 0, 0); // turn off the LED
    delay(200);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
  }
  neos[0].colorWipe(0, 0, 0); // turn off the LEDs
  neos[1].colorWipe(0, 0, 0); // turn off the LEDs
  printMajorDivide("Setup Loop Finished");
}

void updateLuxSensors() {
  combined_lux = 0;
  for (unsigned int i = 0; i < sizeof(lux_managers) / sizeof(lux_managers[0]); i++) {
    if (lux_managers[i].update()) {
      combined_lux += lux_managers[i].getLux();
    }
  }
}

void updateFeatureCollectors() {
  fc[0].update();
  fc[1].update();
  fc[2].update();
  fc[3].update();
}

void updateSong() {
  for (int i = 0; i < num_channels; i++) {
    uint8_t song_rms_weighted = calculateRMSWeighted(&fc[i]);
    uint8_t song_peak_weighted = calculatePeakWeighted(&fc[i]);
    // if (flash_on[i] == false) {
    if (SONG_FEATURE == PEAK_DELTA) {
      if (stereo_audio == false || front_mic_active == false || rear_mic_active == false) {
        if (front_mic_active == true && i == 0) {
          neos[0].colorWipe(song_peak_weighted, 0, 0);
          neos[1].colorWipe(song_peak_weighted, 0, 0);
        } else if (rear_mic_active == true && i == 1) {
          neos[0].colorWipe(song_peak_weighted, 0, 0);
          neos[1].colorWipe(song_peak_weighted, 0, 0);
        }
      } else {
        neos[i].colorWipe(song_peak_weighted, 0, 0);
      }
    } else if (SONG_FEATURE == RMS_DELTA) {
      if (stereo_audio == false) {
        if (front_mic_active == true && i == 0) {
          neos[0].colorWipe(song_rms_weighted, 0, 0);
          neos[1].colorWipe(song_rms_weighted, 0, 0);
        } else if (rear_mic_active == true && i == 1) {
          neos[0].colorWipe(song_rms_weighted, 0, 0);
          neos[1].colorWipe(song_rms_weighted, 0, 0);
        }
      } else  {
        neos[i].colorWipe(song_rms_weighted, 0, 0);
      }
    } else {
      Serial.print("ERROR: the SONG_FEATURE ");
      Serial.print(SONG_FEATURE);
      Serial.println(" is not a valid/implemented SONG_FEATURE");
    }
  }
}

void updateClick() {
  if (fc[2].getPeakPosDelta() > CLICK_PEAK_DELTA_THRESH) {
    if (neos[0].flashOn()) {
      num_flashes[0]++;
      total_flashes[0]++;
      fpm[0] = num_flashes[0] / fpm_timer;
      // Serial.print("num_flashes 0: "); Serial.println(num_flashes[0]);
    }
  }
  if (fc[3].getPeakPosDelta() > CLICK_PEAK_DELTA_THRESH) {
    if (neos[1].flashOn()) {
      num_flashes[1]++;
      total_flashes[1]++;
      fpm[0] = num_flashes[1] / fpm_timer;
      // Serial.print("num_flashes 1: "); Serial.println(num_flashes[1]);
    }
  }
  for (unsigned int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++) {
    neos[i].update();
  }
}

void mothLoop() {
  updateLuxSensors();
  updateFeatureCollectors();
  updateSong();
  updateClick();
  if (AUTOGAIN_ACTIVE) {
    auto_gain[0].updateExternal((neos[0].getOnRatio() + neos[1].getOnRatio()) * 0.5);
    auto_gain[1].updateExternal((neos[0].fpm + neos[1].fpm) * 0.5);
  }
  datalog_manager.update();
  runtime = (double)millis() / 60000;
}
#endif // __MODE_CICADA_H__
