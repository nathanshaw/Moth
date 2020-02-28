#ifndef __MODE_CICADA_H__
#define __MODE_CICADA_H__
#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include "Configuration_cicada_new.h"
#include "NeopixelManager/NeopixelManager.h"
#include "LuxManager/LuxManager.h"
#include "AudioEngine/AudioEngine.h"
#include "AudioEngine/FFTManager1024.h"
#include <Audio.h>

//////////////////////////////// Global Variables /////////////////////////
double color_feature_min[2] = {9999999.99, 999999999.99};
double color_feature_max[2] = {0.0, 0.0};

elapsedMillis feature_reset_tmr;
const unsigned long feature_reset_time = (1000 * 150);// every 2.5 minute?

double brightness_feature_min[2] = {99.99, 99.99};
double brightness_feature_max[2] = {0.0, 0.0};

double current_brightness[2] = {1.0, 1.0};
double last_brightness[2] = {1.0, 1.0};

double current_color[2] = {1.0, 1.0};
double last_color[2] = {1.0, 1.0};

//////////////////////////////// Global Objects /////////////////////////
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);

NeoGroup neos[2] = {
  NeoGroup(&leds, 0, (NUM_LED / 2) - 1, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, NUM_LED / 2, NUM_LED - 1, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_MANAGERS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

FeatureCollector fc[2] = {FeatureCollector("front"), FeatureCollector("rear")};

DLManager datalog_manager = DLManager((String)"Datalog Manager");
FFTManager1024 fft_features = FFTManager1024("Input FFT");

////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S            i2s1;           //xy=55,291.8571424484253
AudioFilterBiquad        biquad2;        //xy=217.00389099121094,302.0039281845093
AudioFilterBiquad        biquad1;        //xy=219.00390625,270.00391578674316
AudioAmplifier           amp2;           //xy=378.79129791259766,302.57704162597656
AudioAmplifier           amp1;           //xy=380.2198715209961,264.0055875778198
AudioAnalyzePeak         peak2;          //xy=517.0039100646973,316.003924369812
AudioOutputUSB           usb1;           //xy=519.142822265625,284.71433544158936
AudioAnalyzePeak         peak1;          //xy=521.00390625,221.0039176940918
AudioAnalyzeFFT1024      input_fft;      //xy=521.3627586364746,251.71987438201904
AudioConnection          patchCord1(i2s1, 0, biquad1, 0);
AudioConnection          patchCord2(i2s1, 1, biquad2, 0);
AudioConnection          patchCord3(biquad2, amp2);
AudioConnection          patchCord4(biquad1, amp1);
AudioConnection          patchCord5(amp2, 0, usb1, 1);
AudioConnection          patchCord6(amp2, peak2);
AudioConnection          patchCord7(amp1, 0, usb1, 0);
AudioConnection          patchCord8(amp1, input_fft);
AudioConnection          patchCord9(amp1, peak1);
////////////////////////////////////////////////////////////////////////////////////

void linkFeatureCollectors() {
  // fc[0].linkAmplifier(&amp1, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  // fc[1].linkAmplifier(&amp2, MIN_SONG_GAIN * MASTER_GAIN_SCALER, MAX_SONG_GAIN * MASTER_GAIN_SCALER);
  // fc 0-1 are for the song front/rear
  fc[0].linkPeak(&peak1, global_peak_scaler, PRINT_PEAK_VALS);
  fc[1].linkPeak(&peak2, global_peak_scaler, PRINT_PEAK_VALS);

  fft_features.linkFFT(&input_fft);
  Serial.println("Linked FFT to FFTManager");
  fft_features.setFFTScaler(global_fft_scaler);
  fft_features.setCentroidActive(true);
  Serial.println("Started calculating Centroid in the FFTManager");
  fft_features.setFluxActive(true);
  Serial.println("Started calculating FLUX in the FFTManager");
  // fc 0-1 are for the song front/rear
  // this equates to about 4k - 16k, perhaps I shoul
  // the last bool is for flux
  // the second to last bool is for centroid
  // fc[0].linkFFT(&input_fft, 23, 93, (double)global_fft_scaler, SCALE_FFT_BIN_RANGE, true, false);
  // fc[0].autoPrintCentroid(PRINT_CENTROID_VALS);
  // fc[1].linkFFT(&input_fft, 23, 93, (double)global_fft_scaler, SCALE_FFT_BIN_RANGE, true, false);
}

void setupAudio() {
  ////////////// Audio ////////////
  AudioMemory(AUDIO_MEMORY);
  linkFeatureCollectors();
  biquad1.setHighpass(0, 80, 0.85);
  biquad1.setHighpass(1, 80, 0.85);
  biquad1.setHighpass(2, 80, 0.85);
  biquad1.setLowShelf(3, 80, CLICK_BQ1_DB);
  biquad2.setHighpass(0, 80, 0.85);
  biquad2.setHighpass(1, 80, 0.85);
  biquad2.setHighpass(2, 80, 0.85);
  biquad2.setLowShelf(3, 80, CLICK_BQ1_DB);
  amp1.gain(STARTING_GAIN * MASTER_GAIN_SCALER);
  amp2.gain(STARTING_GAIN * MASTER_GAIN_SCALER);
  // set gain level? automatically?
  // todo make this adapt to when microphones are broken on one or more side...
  printDivide();
}

double calculateSongBrightness() {
  double b = fft_features.getFFTRangeByFreq(4000, 16000);
  return b;
}


double calculateSongColor(int i) {
  double cent = fft_features.getCentroid();       // right now we are only polling the first FC for its centroid to use to color both sides
  if (cent < color_feature_min[i]) {
    color_feature_min[i] = cent;
  }
  if (cent > color_feature_max[i]) {
    color_feature_max[i] = cent;
  }
  cent = (cent - color_feature_min[i]) / (color_feature_max[i] - color_feature_min[i]);
  return cent;
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
      /*
        datalog_manager.addStaticLog("Front Total Flashes Detected  : ",
                                   STATICLOG_FLASHES_TIMER, &total_flashes[0]);
        datalog_manager.addStaticLog("Rear Total Flashes Detected   : ",
                                   STATICLOG_FLASHES_TIMER, &total_flashes[1]);
      */
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

    /*
      if (AUTOLOG_FLASHES_F > 0) {
      datalog_manager.addAutolog("Front Led Flash Number Log ", AUTOLOG_FLASHES_TIMER, &total_flashes[0]);
      }
      if (AUTOLOG_FLASHES_R > 0) {
      datalog_manager.addAutolog("Rear Led Flash Number Log ", AUTOLOG_FLASHES_TIMER, &total_flashes[1]);
      }
    */

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

void updateGoertzel() {
  // TODO right now just get it calculating and printing the value for the two ranges I have already
  // calculate the magnitude of each range we are interested in
  // subtract the ranges we are not interested from the value in the range we are interested in
  // check to see if the remaining value is greater than the threshold we set
}


void updateSongNew() {
  /////////////////// LOCAL SCALER RESET //////////////////////////
  if (feature_reset_tmr > feature_reset_time) {
    for (int t = 0; t < num_channels; t++) {
      color_feature_min[t] = color_feature_min[t] * 1.05;
      color_feature_max[t] = color_feature_max[t] * 0.95;
      brightness_feature_min[t] = brightness_feature_min[t] * 1.05;
      brightness_feature_max[t] = brightness_feature_max[t] * 0.95;
    }
    dprintln(PRINT_SONG_DEBUG, "reset song feature min and max for cent and brightness ");
    feature_reset_tmr = 0;
  }

  for (int i = 0; i < num_channels; i++) {
    /////////////////////////////////////////////////////////////////
    double target_brightness = 0.0;   // 0.0 - 1.0
    double target_color = 0.0;        // 0.0 - 1.0
    uint8_t red, green, blue;
    Serial.println();
    Serial.print("channel ");
    Serial.print(i);
    /////////////////// Brightness ///////////////////////////////////
    target_brightness = calculateSongBrightness();
    Serial.print("target_brightness(1): ");
    Serial.print(target_brightness);
    Serial.print(" ");
    if (target_brightness < brightness_feature_min[i]) {
      brightness_feature_min[i] = target_brightness;
    }
    if (target_brightness > brightness_feature_max[i]) {
      brightness_feature_max[i] = target_brightness;
    }
    Serial.print(" brightness_feature_min/max: ");
    Serial.print(brightness_feature_min[i]);
    Serial.print(" / ");
    Serial.println(brightness_feature_max[i]);
    if (target_brightness != 0.0) {
      target_brightness = (target_brightness - brightness_feature_min[i]) / (brightness_feature_max[i] - brightness_feature_min[i]);
    }
    Serial.print("target_brightness(2): ");
    Serial.print(target_brightness);
    Serial.print(" ");
    // target_brightness *= (MAX_BRIGHTNESS - MIN_BRIGHTNESS);
    // target_brightness += MIN_BRIGHTNESS;
    last_brightness[i] = current_brightness[i];
    
    dprint(PRINT_SONG_DEBUG, "current_brightness[");
    dprint(PRINT_SONG_DEBUG, i);
    dprint(PRINT_SONG_DEBUG, "]: ");
    dprint(PRINT_SONG_DEBUG, current_brightness[i]);
    current_brightness[i] = target_brightness;
    // current_brightness[i] = current_brightness[i] + (last_brightness[i] * (1.0 - BRIGHTNESS_LP_LEVEL));
    dprint(PRINT_SONG_DEBUG, " => ");
    dprint(PRINT_SONG_DEBUG, current_brightness[i]);
    
    /////////////////// Color ////////////////////////////////////////
    target_color = calculateSongColor(i);
    last_color[i] = current_color[i];
    current_color[i] = (target_color * COLOR_LP_LEVEL) + (last_color[i] * (1.0 - COLOR_LP_LEVEL));

    ////////////////// Calculate Actual Values ///////////////////////
    red = ((1.0 - target_color) * SONG_RED_LOW) + (target_color * SONG_RED_HIGH);
    green = ((1.0 - target_color) * SONG_GREEN_LOW) + (target_color * SONG_GREEN_HIGH);
    blue = ((1.0 - target_color) * SONG_BLUE_LOW) + (target_color * SONG_BLUE_HIGH);

    red *= current_brightness[i];
    green *= current_brightness[i];
    blue *= current_brightness[i];


    dprint(PRINT_SONG_DEBUG, " r: ");
    dprint(PRINT_SONG_DEBUG, red);
    dprint(PRINT_SONG_DEBUG, " g: ");
    dprint(PRINT_SONG_DEBUG, green);
    dprint(PRINT_SONG_DEBUG, " b: ");
    dprintln(PRINT_SONG_DEBUG, blue);

    neos[i].colorWipe(red, green, blue, lux_managers[i].getBrightnessScaler());
  }
}

/*
  void updateSong() {
  for (int i = 0; i < num_channels; i++) {
    uint8_t target_brightness = 0;
    uint16_t red = 0;
    uint16_t green = 0;
    uint16_t blue = 0;
    /////////////////// LOCAL SCALER RESET //////////////////////////
    if (feature_reset_tmr > feature_reset_time) {
      for (int t = 0; t < num_channels; t++) {
        color_feature_min[t] = color_feature_min[t] * 1.05;
        color_feature_max[t] = color_feature_max[t] * 0.95;
        brightness_feature_min[t] = brightness_feature_min[t] + 5;
        brightness_feature_max[t] = brightness_feature_max[t] - 5;
      }
      dprintln(PRINT_SONG_DEBUG, "reset song feature min and max for cent and brightness ");
      feature_reset_tmr = 0;
    }

    /////////////////// SONG BRIGHTNESS FEATURE /////////////////////
    dprint(PRINT_SONG_DEBUG, "ch ");
    dprint(PRINT_SONG_DEBUG, i);
    dprint(PRINT_SONG_DEBUG, "target brightness:\t");
    if (SONG_FEATURE == PEAK_RAW) {
      target_brightness = calculatePeakWeighted(&fc[i]);
    } else if (SONG_FEATURE == RMS_RAW) {
      target_brightness = calculateRMSWeighted(&fc[i]);
    }
    dprint(PRINT_SONG_DEBUG, target_brightness);
    if (target_brightness < brightness_feature_min[i]) {
      brightness_feature_min[i] = target_brightness;
    } else if (target_brightness > brightness_feature_max[i]) {
      brightness_feature_max[i] = target_brightness;
    }
    dprint(PRINT_SONG_DEBUG, "\tmin/max: ");
    dprint(PRINT_SONG_DEBUG, brightness_feature_min[i]);
    dprint(PRINT_SONG_DEBUG, "/");
    dprint(PRINT_SONG_DEBUG, brightness_feature_max[i]);
    dprint(PRINT_SONG_DEBUG, "\t");
    // only update to the new scaler after a second has passed so some values exist
    target_brightness = map(target_brightness, brightness_feature_min[i], brightness_feature_max[i], 0, 255);
    // target_brightness = target_brightness - brightness_feature_min[i];
    // target_brightness = ((uint8_t)(double)target_brightness / (double)(brightness_feature_max[i] - brightness_feature_min[i]) * (double)MAX_BRIGHTNESS);
    // (uint8_t)((double)(current_brightness[i] - brightness_feature_min[i]) / (double)(brightness_feature_max[i] - brightness_feature_min[i]) * (double)MAX_BRIGHTNESS);
    dprint(PRINT_SONG_DEBUG, "\tscaled_target/current: ");
    dprint(PRINT_SONG_DEBUG, target_brightness);
    dprint(PRINT_SONG_DEBUG, " / ");
    dprint(PRINT_SONG_DEBUG, current_brightness[i]);
    last_brightness[i] = current_brightness[i];
    current_brightness[i] = (uint8_t)((double)(target_brightness + current_brightness[i]) * 0.5);
    dprint(PRINT_SONG_DEBUG, " new current => ");
    dprintln(PRINT_SONG_DEBUG, current_brightness[i]);

    ///////////////// SONG_COLOR_FEATURE ////////////////////////////
    if (SONG_COLOR_FEATURE == SPECTRAL_CENTROID) {
      double cent = fft_features.getCentroid();       // right now we are only polling the first FC for its centroid to use to color both sides
      dprint(PRINT_SONG_DEBUG, "ch ");
      dprint(PRINT_SONG_DEBUG, i);
      dprint(PRINT_SONG_DEBUG, " color:\t");
      dprint(PRINT_SONG_DEBUG, cent);
      dprint(PRINT_SONG_DEBUG, "\tmin/max:\t");
      if (cent < color_feature_min[i]) {
        color_feature_min[i] = cent;
      }
      if (cent > color_feature_max[i]) {
        color_feature_max[i] = cent;
      }
      dprint(PRINT_SONG_DEBUG, color_feature_min[i]);
      dprint(PRINT_SONG_DEBUG, "/");
      dprint(PRINT_SONG_DEBUG, color_feature_min[i]);
      dprint(PRINT_SONG_DEBUG, "\t");

      cent = (cent - color_feature_min[i]) / (color_feature_max[i] - color_feature_min[i]);

      dprint(PRINT_SONG_DEBUG, cent);
      red = current_brightness[i] * cent ;
      green = current_brightness[i] * (1.0 - cent);
      dprint(PRINT_SONG_DEBUG, "\tr-g\t");
      dprint(PRINT_SONG_DEBUG, red);
      dprint(PRINT_SONG_DEBUG, " - ");
      dprintln(PRINT_SONG_DEBUG, green);
    }
    dprint(PRINT_SONG_DEBUG, "current_brightness[i] - ");
    dprint(PRINT_SONG_DEBUG, current_brightness[i]);
    dprint(PRINT_SONG_DEBUG, "\tr:");
    dprint(PRINT_SONG_DEBUG, red);
    dprint(PRINT_SONG_DEBUG, "\tg:");
    dprintln(PRINT_SONG_DEBUG, green);
    // dprint(PRINT_SONG_DEBUG, "\tb:");
    // dprintln(PRINT_SONG_DEBUG, blue);
    //
    if (stereo_audio == false || front_mic_active == false || rear_mic_active == false) {
      if (front_mic_active == true && i == 0) {
        neos[0].colorWipe(red, green, blue);
        neos[1].colorWipe(red, green, blue);
      } else if (rear_mic_active == true && i == 1) {
        neos[0].colorWipe(red, green, blue);
        neos[1].colorWipe(red, green, blue);
      }
    } else {
      neos[i].colorWipe(red, green, 0);
    }
  }
  }
*/

double last_feature[2];
double current_feature[2];

void updateClickNew() {
  // for a click in theory the spectral flux will be high, the
  // centroid will decrease since the last frame, and there should be an
  // increase of amplitude in the 1k - 3k freq range

  // for all these values we want a 1.0 to be a confident assesment
  // from that feature
  double flux = fft_features.getSpectralFlux();
  // we want the cent_delta to be
  double cent_delta = fft_features.getCentroid();
  double range_rms = fft_features.getFFTRangeByFreq(1000, 3000);
  double feature = flux * range_rms * cent_delta;
  dprint(PRINT_CLICK_DEBUG, "cent_delta   :\t");
  dprintln(PRINT_CLICK_DEBUG, cent_delta);
  dprint(PRINT_CLICK_DEBUG, "flux         :\t");
  dprintln(PRINT_CLICK_DEBUG, flux);
  dprint(PRINT_CLICK_DEBUG, "range_rms    :\t");
  dprintln(PRINT_CLICK_DEBUG, range_rms);
  dprint(PRINT_CLICK_DEBUG, "feature      :\t");
  dprintln(PRINT_CLICK_DEBUG, feature);
}

void updateClick() {
  double flux = fft_features.getSpectralFlux();
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
    // past_rms[i] = rms[i];
    double peak = fc[i + 2].getPeakPosDelta();
    current_feature[i] = peak * flux * 500;
    if (current_feature[i] != last_feature[i]) {
      last_feature[i] = current_feature[i];
      // Serial.print("last feature : ");
      // Serial.println(last_feature);
      // last_feature = 0;
      double threshold = CLICK_THRESH;
      /*
        if (i == 0 && current_feature[i] != 0) {
        Serial.print("feature      : ");
        Serial.println(current_feature[i], 12);
        Serial.print("flux: ");
        Serial.print(flux, 12);
        Serial.print("\tpeak: ");
        Serial.print(peak, 12);
        Serial.print("\tcent: ");
        Serial.println(fft_features.getCentroid());
        // Serial.println("----------------------");
        }
      */
      // Serial.print(rms_pos_delta
      if (current_feature[i] > threshold) {
        // Serial.println("____________________ CLICK ________________________ 3.0");
        dprint(PRINT_CLICK_DEBUG, "click feature is above threshold: ");
        dprint(PRINT_CLICK_DEBUG, current_feature[i]);
        dprint(PRINT_CLICK_DEBUG, " - ");
        dprint(PRINT_CLICK_DEBUG, threshold);
        if (neos[i].flashOn()) {
          // num_flashes[i]++;
          // total_flashes[i]++;
          // fpm[i] = num_flashes[i] / fpm_timer;
          if (INDEPENDENT_FLASHES == false && i == 0 && ENCLOSURE_TYPE != GROUND_ENCLOSURE) {
            if (neos[1].flashOn()) {
              // num_flashes[1]++;
              // total_flashes[1]++;
              // fpm[1] = num_flashes[1] / fpm_timer;
            }
          }
          if (INDEPENDENT_FLASHES == false && i == 1) {
            if (neos[0].flashOn()) {
              // num_flashes[0]++;
              // total_flashes[0]++;
              // fpm[0] = num_flashes[0] / fpm_timer;
            }
          }
        }
      }
    }
    for (unsigned int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++) {
      neos[i].update();
    }
  }
}


void updateAutogain() {
#if (AUTOGAIN_ACTIVE)
  auto_gain[0].updateExternal((neos[0].getOnRatio() + neos[1].getOnRatio()) * 0.5);
  auto_gain[1].updateExternal((neos[0].fpm + neos[1].fpm) * 0.5);
  return;
#endif
}

void updateMode() {
  updateSongNew();
  Serial.print("audio memory max: ");
  Serial.print(AudioMemoryUsageMax());
}

#endif // __MODE_CICADA_H__
