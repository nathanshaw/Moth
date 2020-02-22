#ifndef __MODE_CICADA_H__
#define __MODE_CICADA_H__
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
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);

NeoGroup neos[2] = {
  NeoGroup(&leds, 0, (NUM_LED / 2) - 1, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, NUM_LED / 2, NUM_LED - 1, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_SENSORS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};


DLManager datalog_manager = DLManager((String)"Datalog Manager");

FeatureCollector fc[4] = {FeatureCollector("front song"), FeatureCollector("rear song"), FeatureCollector("front click"), FeatureCollector("rear click")};

AutoGain auto_gain[2] = {AutoGain("Song", &fc[0], &fc[1], STARTING_SONG_GAIN, STARTING_SONG_GAIN, MAX_GAIN_ADJUSTMENT),
                         AutoGain("Click", &fc[2], &fc[3], STARTING_CLICK_GAIN, STARTING_CLICK_GAIN, MAX_GAIN_ADJUSTMENT)
                        };

////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S            i2s1;           //xy=430.0000194311142,1016.9999933242798
AudioAmplifier           click_input_amp1; //xy=651,1006
AudioAmplifier           song_input_amp1; //xy=651,1039
AudioAmplifier           song_input_amp2; //xy=651,1072
AudioAmplifier           click_input_amp2; //xy=655,973
AudioFilterBiquad        click_biquad2;  //xy=829,976
AudioFilterBiquad        song_biquad2;   //xy=831,1071
AudioFilterBiquad        click_biquad1;  //xy=833,1008
AudioFilterBiquad        song_biquad1;   //xy=833,1038
AudioAmplifier           click_mid_amp2; //xy=999,974
AudioAmplifier           click_mid_amp1; //xy=1002,1005
AudioAmplifier           song_mid_amp2;  //xy=1003,1073
AudioAmplifier           song_mid_amp1;  //xy=1005,1041
AudioFilterBiquad        song_biquad11;  //xy=1179,1041
AudioFilterBiquad        click_biquad21; //xy=1180,974
AudioFilterBiquad        song_biquad21;  //xy=1181,1073
AudioFilterBiquad        click_biquad11; //xy=1182,1005
AudioAmplifier           click_post_amp2; //xy=1360,974
AudioAmplifier           click_post_amp1; //xy=1360,1005
AudioAmplifier           song_post_amp2; //xy=1360,1073
AudioAmplifier           song_post_amp1; //xy=1361,1041
AudioOutputUSB           usb1;           //xy=1623.5000457763672,975.500036239624

#if RMS_ACTIVE_SONG == true
AudioAnalyzeRMS          song_rms1;      //xy=1626,1048
AudioAnalyzeRMS          song_rms2;      //xy=1627,1013
AudioConnection          patchCord29(song_post_amp1, song_rms1);
AudioConnection          patchCord26(song_post_amp2, song_rms2);
#endif// song == true

#if RMS_ACTIVE_CLICK == true
AudioAnalyzeRMS          click_rms1;     //xy=1629,875
AudioAnalyzeRMS          click_rms2;     //xy=1630,840
AudioConnection          patchCord23(click_post_amp1, click_rms1);
AudioConnection          patchCord21(click_post_amp2, click_rms2);
#endif // click == true

#if PEAK_ACTIVE_CLICK == true
AudioAnalyzePeak         click_peak1;    //xy=1633,907
AudioAnalyzePeak         click_peak2;    //xy=1634,940
AudioConnection          patchCord24(click_post_amp1, click_peak1);
AudioConnection          patchCord22(click_post_amp2, click_peak2);
#endif // peak_active_click == true

#if PEAK_ACTIVE_SONG  == true
AudioAnalyzePeak         song_peak1;     //xy=1630,1080
AudioAnalyzePeak         song_peak2;     //xy=1631,1112
AudioConnection          patchCord30(song_post_amp1, song_peak1);
AudioConnection          patchCord27(song_post_amp2, song_peak2);
#endif // peak_active_song == true

#if RMS_ACTIVE_SONG == true || RMS_ACTIVE_CLICK == true
AudioAnalyzeFFT256       input_fft;       //xy=1632.5000457763672,1145.000036239624
AudioConnection          patchCord32(click_input_amp1, input_fft);
#endif // fft?

// AudioAnalyzeFFT1024      song_fft2;       //xy=1632.5000457763672,1177.5000343322754
AudioConnection          patchCord1(i2s1, 0, click_input_amp1, 0);
AudioConnection          patchCord2(i2s1, 0, click_input_amp2, 0);
AudioConnection          patchCord3(i2s1, 1, song_input_amp1, 0);
AudioConnection          patchCord4(i2s1, 1, song_input_amp2, 0);
AudioConnection          patchCord5(click_input_amp1, click_biquad1);
AudioConnection          patchCord6(song_input_amp1, song_biquad1);
AudioConnection          patchCord7(song_input_amp2, song_biquad2);
AudioConnection          patchCord8(click_input_amp2, click_biquad2);
AudioConnection          patchCord9(click_biquad2, click_mid_amp2);
AudioConnection          patchCord10(song_biquad2, song_mid_amp2);
AudioConnection          patchCord11(click_biquad1, click_mid_amp1);
AudioConnection          patchCord12(song_biquad1, song_mid_amp1);
AudioConnection          patchCord13(click_mid_amp2, click_biquad21);
AudioConnection          patchCord14(click_mid_amp1, click_biquad11);
AudioConnection          patchCord15(song_mid_amp2, song_biquad21);
AudioConnection          patchCord16(song_mid_amp1, song_biquad11);
AudioConnection          patchCord17(song_biquad11, song_post_amp1);
AudioConnection          patchCord18(click_biquad21, click_post_amp2);
AudioConnection          patchCord19(song_biquad21, song_post_amp2);
AudioConnection          patchCord20(click_biquad11, click_post_amp1);
AudioConnection          patchCord25(click_post_amp1, 0, usb1, 0);
AudioConnection          patchCord31(song_post_amp1, 0, usb1, 1);

void initAutoGain() {
  auto_gain[0].setExternalThresholds((String)"Led ON Ratio", MIN_ON_RATIO_THRESH, LOW_ON_RATIO_THRESH,
                                     HIGH_ON_RATIO_THRESH, MAX_ON_RATIO_THRESH);
  auto_gain[1].setExternalThresholds((String)"FPM", MIN_FPM_THRESH, LOW_FPM_THRESH,
                                     HIGH_FPM_THRESH, MAX_FPM_THRESH);
  auto_gain[0].setUpdateRate(AUTOGAIN_FREQUENCY);
  auto_gain[1].setUpdateRate(AUTOGAIN_FREQUENCY);
  auto_gain[0].setStartDelay(AUTOGAIN_START_DELAY);
  auto_gain[1].setStartDelay(AUTOGAIN_START_DELAY);
}

void linkFeatureCollectors() {
  fc[0].linkAmplifier(&song_input_amp1, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[0].linkAmplifier(&song_mid_amp1, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[0].linkAmplifier(&song_post_amp1, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[1].linkAmplifier(&song_input_amp2, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[1].linkAmplifier(&song_mid_amp2, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[1].linkAmplifier(&song_post_amp2, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[2].linkAmplifier(&click_input_amp1, MIN_CLICK_GAIN * MASTER_GAIN_SCALER, MAX_CLICK_GAIN * MASTER_GAIN_SCALER);
  fc[2].linkAmplifier(&click_mid_amp1, MIN_CLICK_GAIN * MASTER_GAIN_SCALER, MAX_CLICK_GAIN * MASTER_GAIN_SCALER);
  fc[2].linkAmplifier(&click_post_amp1, MIN_CLICK_GAIN * MASTER_GAIN_SCALER, MAX_CLICK_GAIN * MASTER_GAIN_SCALER);
  fc[3].linkAmplifier(&click_input_amp2, MIN_CLICK_GAIN * MASTER_GAIN_SCALER, MAX_CLICK_GAIN * MASTER_GAIN_SCALER);
  fc[3].linkAmplifier(&click_mid_amp2, MIN_CLICK_GAIN * MASTER_GAIN_SCALER, MAX_CLICK_GAIN * MASTER_GAIN_SCALER);
  fc[3].linkAmplifier(&click_post_amp2, MIN_CLICK_GAIN * MASTER_GAIN_SCALER, MAX_CLICK_GAIN * MASTER_GAIN_SCALER);

#if RMS_ACTIVE_SONG
    // fc 0-1 are for the song front/rear
    fc[0].linkRMS(&song_rms1, global_rms_scaler, PRINT_RMS_VALS);
    fc[1].linkRMS(&song_rms2, global_rms_scaler, PRINT_RMS_VALS);
    Serial.println("Feature collectors 0 and 1 have been linked to RMS");
#endif//rms_active_song

#if RMS_ACTIVE_CLICK
    // fc 2-3 are for the click front/rear
    fc[2].linkRMS(&click_rms1, global_rms_scaler, PRINT_RMS_VALS);
    fc[3].linkRMS(&click_rms2, global_rms_scaler, PRINT_RMS_VALS);
    Serial.println("Feature collectors 2 and 3 have been linked to RMS");
#endif // rms_active_click

#if PEAK_ACTIVE_SONG
    // fc 0-1 are for the song front/rear
    fc[0].linkPeak(&song_peak1, global_peak_scaler, PRINT_PEAK_VALS);
    fc[1].linkPeak(&song_peak2, global_peak_scaler, PRINT_PEAK_VALS);
    Serial.println("Feature collectors 0 and 1 have been linked to Peak");
#endif // peak_active_song

#if PEAK_ACTIVE_CLICK
    // fc 2-3 are for the click front/rear
    fc[2].linkPeak(&click_peak1, global_peak_scaler, PRINT_PEAK_VALS);
    fc[3].linkPeak(&click_peak2, global_peak_scaler, PRINT_PEAK_VALS);
    Serial.println("Feature collectors 2 and 3 have been linked to Peak");
#endif

#if FFT_ACTIVE_SONG
    fc[0].linkFFT(&input_fft, 23, 93, (double)global_fft_scaler, SCALE_FFT_BIN_RANGE, true, false);
    fc[1].linkFFT(&input_fft, 23, 93, (double)global_fft_scaler, SCALE_FFT_BIN_RANGE, true, false);
    Serial.println("Feature collectors 0 and 1 have been linked to FFT");
#endif // is fft active?

#if FFT_ACTIVE_CLICK
    fc[2].linkFFT(&input_fft, 12, 20, (double)global_fft_scaler,  SCALE_FFT_BIN_RANGE, true, true);
    fc[3].linkFFT(&input_fft, 12, 20, (double)global_fft_scaler,  SCALE_FFT_BIN_RANGE, true, true);
    Serial.println("Feature collectors 2 and 3 have been linked to FFT");
#endif

#if CENTROID_ACTIVE_SONG
    fc[0].setCentroidActive(true);
    fc[1].setCentroidActive(true);
    Serial.println("Feature collectors 0 and 1 have Centroid active now");
#endif

#if CENTROID_ACTIVE_CLICK
    fc[2].setCentroidActive(true);
    fc[3].setCentroidActive(true);
    Serial.println("Feature collectors 2 and 3 have Centroid active now");
#endif

#if FLUX_ACTIVE_SONG
    fc[0].setFluxActive(true);
    fc[1].setFluxActive(true);
    Serial.println("Feature collectors 0 and 1 have Flux active now");
#endif

#if FLUX_ACTIVE_CLICK
    fc[2].setFluxActive(true);
    fc[3].setFluxActive(true);
    Serial.println("Feature collectors 2 and 3 have Flux active now");
#endif
}

void setupAudio() {
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

  fc[0].updateGain(STARTING_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[1].updateGain(STARTING_SONG_GAIN * MASTER_GAIN_SCALER);
  fc[2].updateGain(STARTING_CLICK_GAIN * MASTER_GAIN_SCALER);
  fc[3].updateGain(STARTING_CLICK_GAIN * MASTER_GAIN_SCALER);

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
  rms = f->getRMS() * global_rms_scaler - RMS_LOW_THRESH;
  if (rms > 1.0) {
    rms = 1.0;
  } else if (rms < 0.0) {
    rms = 0.0;
  }
  uint8_t scaler = (uint8_t)(rms * (double)MAX_BRIGHTNESS);
  return scaler;
}

uint8_t calculatePeakWeighted(FeatureCollector *f) {
  double peak = 0;
  peak = f->getPeak() * global_peak_scaler - PEAK_LOW_THRESH;
  if (peak > 1.0) {
    peak = 1.0;
  } else if (peak < 0.0) {
    peak = 0.0;
  }
  uint8_t scaler = uint8_t(peak * (double)MAX_BRIGHTNESS);
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
    datalog_manager.logSetupConfigByte("Hardware Version major      : ", H_VERSION_MAJOR);
    datalog_manager.logSetupConfigByte("Hardware Version minor      : ", H_VERSION_MINOR);
    datalog_manager.logSetupConfigByte("Software Version major      : ", S_VERSION_MAJOR);
    datalog_manager.logSetupConfigByte("Software Version major      : ", S_VERSION_MINOR);
    datalog_manager.logSetupConfigByte("Software Version major      : ", S_SUBVERSION);
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
    datalog_manager.logSetupConfigLong("Timer 0 Start Time          : ", datalog_manager.getTimerStart(0));
    datalog_manager.logSetupConfigLong("Timer 0 End Time            : ", datalog_manager.getTimerEnd(0));
    datalog_manager.logSetupConfigLong("Timer 0 Logging Rate        : ", datalog_manager.getTimerRate(0));
    datalog_manager.logSetupConfigLong("Timer 1 Start Time          : ", datalog_manager.getTimerStart(1));
    datalog_manager.logSetupConfigLong("Timer 1 End Time            : ", datalog_manager.getTimerEnd(1));
    datalog_manager.logSetupConfigLong("Timer 1 Logging Rate        : ", datalog_manager.getTimerRate(1));
    datalog_manager.logSetupConfigLong("Timer 2 Start Time          : ", datalog_manager.getTimerStart(2));
    datalog_manager.logSetupConfigLong("Timer 2 End Time            : ", datalog_manager.getTimerEnd(2));
    datalog_manager.logSetupConfigLong("Timer 2 Logging Rate        : ", datalog_manager.getTimerRate(2));
    datalog_manager.logSetupConfigLong("Timer 3 Start Time          : ", datalog_manager.getTimerStart(3));
    datalog_manager.logSetupConfigLong("Timer 3 End Time            : ", datalog_manager.getTimerEnd(3));
    datalog_manager.logSetupConfigLong("Timer 3 Logging Rate        : ", datalog_manager.getTimerRate(3));
    printMinorDivide();
    // runtime log
    if (STATICLOG_RUNTIME) {
      datalog_manager.addStaticLog("Program Runtime (minutes)              : ",
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

    if (STATICLOG_RGB_AVG_VALS) {
      datalog_manager.addStaticLog("Average Red Value (Front)     : ",
                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos[0].red_avg);
      datalog_manager.addStaticLog("Average Red Value (Rear)      : ",
                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos[1].red_avg);
      datalog_manager.addStaticLog("Average Green Value (Front)   : ",
                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos[0].green_avg);
      datalog_manager.addStaticLog("Average Green Value (Rear)    : ",
                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos[1].green_avg);
      datalog_manager.addStaticLog("Average Blue Value (Front)    : ",
                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos[0].blue_avg);
      datalog_manager.addStaticLog("Average Blue Value (Rear)     : ",
                                   STATICLOG_RGB_AVG_VALS_TIMER, &neos[1].blue_avg);
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
    if (PRINT_EEPROM_CONTENTS > 0) {
      delay(100);
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
    Serial.println("Not clearing the EEPROM Datalog Contents");
  }
}

double feature_min = 9999999.99;
double feature_max = 0.0000001;
elapsedMillis feature_reset_tmr;
const unsigned long feature_reset_time = (1000 * 60);// every minute?

void updateSong() {
  for (int i = 0; i < num_channels; i++) {
    // Serial.print("num leds : ");Serial.println(NUM_LED);
    uint8_t brightness = 0;
    uint16_t red, green;
    if (SONG_FEATURE == PEAK_RAW) {
      brightness = calculatePeakWeighted(&fc[i]);
    } else if (SONG_FEATURE == RMS_RAW) {
      brightness = calculateRMSWeighted(&fc[i]);
    }
    if (SONG_COLOR_FEATURE == SPECTRAL_CENTROID) {
      double cent = fc[i].getSmoothedCentroid();
      if (feature_reset_tmr > feature_reset_time) {
        feature_min = 999999.999;
        feature_max = 0.00000001;
        Serial.println("reset feature min and max");
        feature_reset_tmr = 0;
      }
      if (cent < feature_min) {
        feature_min = cent;
      };
      if (cent > feature_max) {
        feature_max = cent;
      };
      if (PRINT_SONG_DEBUG) {
        Serial.print("bright: ");
        Serial.print(brightness);
        Serial.print("\tcent : ");
        Serial.print(cent);
      }
      
      cent = (cent - feature_min) / (feature_max - feature_min);
      
      if (PRINT_SONG_DEBUG) {
        Serial.print("\tcent : ");
        Serial.print(cent);
        Serial.print("\tmin/max:\t");
        Serial.print(feature_min);
        Serial.print(" / ");
        Serial.println(feature_max);
      }
      red = brightness * cent ;
      green = brightness * (1.0 - cent);
    } else if (SONG_COLOR_FEATURE == MAX_ENERGY_BIN) {
      double bin_pos = fc[i].getRelativeBinPos();
      red = brightness * bin_pos;
      green = brightness * (1.0 - bin_pos);
    } else {
      red = brightness;
      green = 0;
    }
    // dprint(PRINT_SONG_DEBUG, "brightness - ");
    // dprint(PRINT_SONG_DEBUG, brightness);
    // dprint(PRINT_SONG_DEBUG, "\tr:");
    // dprint(PRINT_SONG_DEBUG, red);
    // dprint(PRINT_SONG_DEBUG, "\tg:");
    // dprintln(PRINT_SONG_DEBUG, green);
    // dprint(PRINT_SONG_DEBUG, "\tb:");
    // dprintln(PRINT_SONG_DEBUG, blue);
    if ((front_mic_active == false || rear_mic_active == false) && num_channels == 2) {
      if (front_mic_active == true && i == 0) {
        neos[0].colorWipe(red, green, 0);
        neos[1].colorWipe(red, green, 0);
      } else if (rear_mic_active == true && i == 1) {
        neos[0].colorWipe(red, green, 0);
        neos[1].colorWipe(red, green, 0);
      }
    } else {
      neos[i].colorWipe(red, green, 0);
    }
  }
}

void updateClick() {
  for (int i = 0; i < num_channels; i++) {
    /*
    double feature = 0.0;
    double threshold = 0.0;
    if (CLICK_FEATURE == PEAK_DELTA) {
      feature = fc[i + 2].getPeakPosDelta();
      threshold = CLICK_PEAK_DELTA_THRESH;
    } else if (CLICK_FEATURE == RMS_DELTA) {
      feature = fc[i + 2].getRMSPosDelta();
      threshold = CLICK_RMS_DELTA_THRESH;
    } else if (CLICK_FEATURE == SPECTRAL_FLUX) {
      feature = fc[2].getSpectralFlux() * SPECTRAL_FLUX_SCALER;
      threshold = CLICK_SPECTRAL_FLUX_THRESH;
      // Serial.print("spectral flux: ");
      // Serial.println(feature);
    }
    */
    double flux = fc[i + 2].getSpectralFlux();
    double rms = fc[i+2].getRMS();
    double feature = rms * flux;
    double threshold = 1.0;

      Serial.print("feature:\t");
      Serial.print(feature, 12);
      Serial.print("\tspectral flux:\t");
      Serial.print(flux, 12);
      Serial.print("\trms:\t");
      Serial.println(rms, 12);
      
    // Serial.print(rms_pos_delta 
    if (feature > threshold) {
      dprint(PRINT_CLICK_DEBUG, "click feature is above threshold: ");
      dprint(PRINT_CLICK_DEBUG, feature);
      dprint(PRINT_CLICK_DEBUG, " - ");
      dprint(PRINT_CLICK_DEBUG, threshold);
      if (neos[i].flashOn()) {
        num_flashes[i]++;
        total_flashes[i]++;
        fpm[i] = num_flashes[i] / fpm_timer;
        if (INDEPENDENT_FLASHES == false && i == 0 && ENCLOSURE_TYPE != GROUND_ENCLOSURE) {
          if (neos[1].flashOn()) {
            num_flashes[1]++;
            total_flashes[1]++;
            fpm[1] = num_flashes[1] / fpm_timer;
          }
        }
        if (INDEPENDENT_FLASHES == false && i == 1) {
          if (neos[0].flashOn()) {
            num_flashes[0]++;
            total_flashes[0]++;
            fpm[0] = num_flashes[0] / fpm_timer;
          }
        }
      }
    }
  }
      for (unsigned int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++) {
        neos[i].update();
      }
}

void updateAutogain() {
#if (AUTOGAIN_ACTIVE)
  auto_gain[0].updateExternal((neos[0].getOnRatio() + neos[1].getOnRatio()) * 0.5);
  auto_gain[1].updateExternal((neos[0].fpm + neos[1].fpm) * 0.5);
#endif
}

void updateMode() {
  // updateClick();
  updateSong();
}

#endif // __MODE_CICADA_H__
