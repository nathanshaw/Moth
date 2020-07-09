#ifndef __MODE_H__
#define __MODE_H__

#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include "Configuration_cicada.h"
#include "NeopixelManager/NeopixelManager.h"
#include "LuxManager/LuxManager.h"
#include "AudioEngine/AudioEngine.h"
#include "AudioEngine/FFTManager1024.h"
#include <Audio.h>

//////////////////////////////// Global Variables /////////////////////////
double color_feature_min = 1.00;
double color_feature_max = 0.0;

elapsedMillis feature_reset_tmr;
const unsigned long feature_reset_time = (1000 * 2.5);// every 2.5 minute?

double brightness_feature_min[2] = {1.0, 1.0};
double brightness_feature_max[2] = {0.0, 0.0};

double current_brightness[2] = {1.0, 1.0};
double last_brightness[2] = {1.0, 1.0};

double current_color = 0.5;
double last_color = 0.5;

double last_feature[2];
double current_feature[2];

//////////////////////////////// Click ////////////////////////////////////
elapsedMillis click_feature_reset_tmr;
const unsigned long click_feature_reset_time = (1000 * 60 * 1);// every 5 minute?

double last_cent_neg_delta = 0.0;
double last_flux = 0.0;
double last_range_rms = 0.0;

double min_rrms = 1.0;
double max_rrms = 0.1;
double min_flux = 1.0;
double max_flux = 0.1;
double min_cent_negd = 1.0;
double max_cent_negd = 0.0;
double min_click_feature = 1.0;
double max_click_feature = 0.0;

double click_flux = 0.0;
double click_rrms = 0.0;
double click_cent = 0.0;

//////////////////////////////// Global Objects /////////////////////////
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);

NeoGroup neos[2] = {
  NeoGroup(&leds, 0, (NUM_LED / 2) - 1, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, NUM_LED / 2, NUM_LED - 1, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay);

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
AudioAnalyzePeak         peak1;          //xy=521.00390625,221.0039176940918
AudioAnalyzeFFT1024      input_fft;      //xy=521.3627586364746,251.71987438201904
AudioAnalyzeRMS          rms;            //xy=650.0000076293945,151.00000190734863
AudioConnection          patchCord10(amp1, rms);

#if AUDIO_USB_DEBUG > 0
AudioOutputUSB           usb1;           //xy=519.142822265625,284.71433544158936
AudioConnection          patchCord5(amp2, 0, usb1, 1);
AudioConnection          patchCord7(amp1, 0, usb1, 0);
#endif // DEBUG_USB_AUDIO

AudioConnection          patchCord1(i2s1, 0, biquad1, 0);
AudioConnection          patchCord2(i2s1, 1, biquad2, 0);
AudioConnection          patchCord3(biquad2, amp2);
AudioConnection          patchCord4(biquad1, amp1);
AudioConnection          patchCord6(amp2, peak2);
AudioConnection          patchCord8(amp1, input_fft);
AudioConnection          patchCord9(amp1, peak1);
////////////////////////////////////////////////////////////////////////////////////

void linkFeatureCollectors() {
  // fc 0-1 are for the song front/rear
  fc[0].linkPeak(&peak1, global_peak_scaler, P_PEAK_VALS);
  fc[1].linkPeak(&peak2, global_peak_scaler, P_PEAK_VALS);

  fft_features.linkFFT(&input_fft, true);
  Serial.println("Linked FFT to FFTManager");
  //  fft_features.setFFTScaler(global_fft_scaler);
  fft_features.setupCentroid(true, 4000, 16000);
  Serial.println("Started calculating Centroid in the FFTManager");
  fft_features.setFluxActive(true);
  Serial.println("Started calculating FLUX in the FFTManager");
}

void setupAudio() {
  ////////////// Audio ////////////
  Serial.print("Setting up Audio Parameters");
  AudioMemory(AUDIO_MEMORY);
  Serial.print("Audio Memory has been set to: ");
  Serial.println(AUDIO_MEMORY);
  linkFeatureCollectors();
  Serial.println("Feature collectors have been linked");
  biquad1.setHighpass(0, 80, 0.85);
  biquad1.setHighpass(1, 80, 0.85);
  biquad1.setHighpass(2, 80, 0.85);
  biquad1.setLowShelf(3, 80, CLICK_BQ1_DB);
  Serial.println("Biquad filter 1 has been configured");
  biquad2.setHighpass(0, 80, 0.85);
  biquad2.setHighpass(1, 80, 0.85);
  biquad2.setHighpass(2, 80, 0.85);
  biquad2.setLowShelf(3, 80, CLICK_BQ1_DB);
  Serial.println("Biquad filter 2 has been configured");
  amp1.gain(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
  amp2.gain(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
  Serial.print("Set amp1 and amp2 gains to: ");
  Serial.println(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
  // set gain level? automatically?
  // todo make this adapt to when microphones are broken on one or more side...
  Serial.println("Exiting setupAudio()");
  printDivide();
}

double calculateSongBrightness(uint8_t i) {
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  // take the average of two-octave chunks from 1-4k and 250 - 1k
#if P_CALCULATE_BRIGHTNESS_LENGTH > 0
  uint32_t _start = millis();
#endif

  // 250 - 1k Hz is idx 5-23
  // 1k -4k Hz is idx 23-93
  // 4k - 16k Hz is idx 93 - 372
  double remaining_spect_avg = 0.5 * (fft_features.getFFTRangeByIdx(5, 23) + fft_features.getFFTRangeByIdx(23, 93));
  double target_brightness = fft_features.getFFTRangeByIdx(93, 372) - remaining_spect_avg;
  if (target_brightness < 0.0) {
    target_brightness = 0.0;
    brightness_feature_min[i] = 0.0;
  }
  /*
    if (target_brightness < brightness_feature_min[i]) {
    if (i == 0 && P_SONG_BRIGHTNESS) {
      dprint(P_SONG_BRIGHTNESS, "target_B is less than feature_min: ");
      dprint(P_SONG_BRIGHTNESS, target_brightness, 5);
      dprint(P_SONG_BRIGHTNESS, " < ");
      dprint(P_SONG_BRIGHTNESS, brightness_feature_min[i], 5);
    }
    brightness_feature_min[i] = (target_brightness * 0.15) + (brightness_feature_min[i] * 0.85);
    if (i == 0 && P_SONG_BRIGHTNESS) {
      dprint(P_SONG_BRIGHTNESS, " updated brightness_min and target_brightness to: ");
      dprintln(P_SONG_BRIGHTNESS, brightness_feature_min[i], 5);
    }
    target_brightness = brightness_feature_min[i];
    }
  */
  if (target_brightness > brightness_feature_max[i]) {
    if (i == 0 && P_SONG_BRIGHTNESS) {
      dprint(P_SONG_BRIGHTNESS, "target_B is more than feature_max: ");
      dprint(P_SONG_BRIGHTNESS, target_brightness, 5);
      dprint(P_SONG_BRIGHTNESS, " > ");
      dprintln(P_SONG_BRIGHTNESS, brightness_feature_max[i], 5);
    }
    brightness_feature_max[i] = (target_brightness * BRIGHTNESS_LP_LEVEL) + (brightness_feature_max[i] * (1.0 - BRIGHTNESS_LP_LEVEL));
    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max[i] > 1.0) {
      brightness_feature_max[i] = 1.0;
    }
    if (i == 0 && P_SONG_BRIGHTNESS) {
      dprint(P_SONG_BRIGHTNESS, " updated brightness_max and target_brightness to: ");
      dprintln(P_SONG_BRIGHTNESS, brightness_feature_max[i], 5);
    }
    target_brightness = brightness_feature_max[i];
  }
  dprint(P_SONG_BRIGHTNESS, "channel ");
  dprint(P_SONG_BRIGHTNESS, i);
  dprint(P_SONG_BRIGHTNESS, " target: ");
  dprint(P_SONG_BRIGHTNESS, target_brightness);
  dprint(P_SONG_BRIGHTNESS, "\tmin: ");
  dprint(P_SONG_BRIGHTNESS, brightness_feature_min[i]);
  dprint(P_SONG_BRIGHTNESS, "\tmax: ");
  dprint(P_SONG_BRIGHTNESS, brightness_feature_max[i]);
  // to ensure the unit is not always on
  // instead of comparing the brightness to the cuttoff_threshold
  // we subtract the cuttoff_threshold from both the target and the max
  target_brightness = (target_brightness - brightness_feature_min[i]) / (brightness_feature_max[i] - brightness_feature_min[i]);
  target_brightness = target_brightness - BRIGHTNESS_CUTTOFF_THRESHOLD;
  if (target_brightness < 0.0) {
    target_brightness = 0.0;
  } else if (target_brightness > 1.0) {
    target_brightness = 1.0;
  }
  dprint(P_SONG_BRIGHTNESS, " adjusted: ");
  dprintln(P_SONG_BRIGHTNESS, target_brightness);
#if P_CALCULATE_BRIGHTNESS_LENGTH > 0
  Serial.print("calculateSongBrightness() function call length: ");
  Serial.println(millis() - _start);
#endif
  return target_brightness;
}


double calculateSongColor() {
  double cent = fft_features.getCentroid();       // right now we are only polling the first FC for its centroid to use to color both sides
  if (cent < color_feature_min) {
    color_feature_min = (color_feature_min * 0.94) + (cent * 0.06);
    cent = color_feature_min;
  }
  if (cent > color_feature_max) {
    color_feature_max = (color_feature_max * 0.94) + (cent * 0.06);
    cent = color_feature_max;
  }
  cent = (cent - color_feature_min) / (color_feature_max - color_feature_min);
  return cent;
}

void updateSong() {
#if P_NUM_SONG_UPDATES == 1
  song_updates++;
  uint32_t m = song_update_timer;
  if (song_update_timer > 1000) {
    Serial.print("song updates in last second: ");
    Serial.println(song_updates);
    song_updates = 0;
    song_update_timer = 0;
  }
#endif // P_NUM_SONG_UPDATES
  double target_color = 0.0;        // 0.0 - 1.0
  uint8_t red, green, blue;

  /////////////////// Color ////////////////////////////////////////
  target_color = calculateSongColor();
  last_color = current_color;
  current_color = (target_color * COLOR_LP_LEVEL) + (last_color * (1.0 - COLOR_LP_LEVEL));

#if P_SONG_COLOR > 0
  dprint(P_SONG_COLOR, "target_color: ");
  dprint(P_SONG_COLOR, target_color);
  dprint(P_SONG_COLOR, "\tlast_color: ");
  dprint(P_SONG_COLOR, last_color);
  dprint(P_SONG_COLOR, "\tcurrent_color: ");
  dprint(P_SONG_COLOR, current_color);
#endif

  ////////////////// Calculate Actual Values ///////////////////////
  red = ((1.0 - current_color) * SONG_RED_LOW) + (current_color * SONG_RED_HIGH);
  green = ((1.0 - current_color) * SONG_GREEN_LOW) + (current_color * SONG_GREEN_HIGH);
  blue = ((1.0 - current_color) * SONG_BLUE_LOW) + (current_color * SONG_BLUE_HIGH);

  double target_brightness = 0.0;
  for (int i = 0; i < num_channels; i++) {
    /////////////////// Brightness ///////////////////////////////////
    double __brightness = 0.0;
    if (target_brightness == 0.0) {
      __brightness = calculateSongBrightness(i);
    } else if (STEREO_FEEDBACK == true) {
      __brightness = calculateSongBrightness(i);
    }
    if (__brightness > target_brightness) {
      target_brightness = __brightness;
    }
  }
  last_brightness[0] = current_brightness[0];
  current_brightness[0] = (target_brightness * BRIGHTNESS_LP_LEVEL) + (last_brightness[0] * (1.0 - BRIGHTNESS_LP_LEVEL));

#if P_SONG_BRIGHTNESS > 0
  dprint(P_SONG_BRIGHTNESS, "last/current_brightness[");
  dprint(P_SONG_BRIGHTNESS, 0);
  dprint(P_SONG_BRIGHTNESS, "]: ");
  dprint(P_SONG_BRIGHTNESS, last_brightness[0]);
  dprint(P_SONG_BRIGHTNESS, " => ");
  dprintln(P_SONG_BRIGHTNESS, current_brightness[0]);
#endif

  // red = (uint8_t)((double)red * current_brightness[0]);
  // green = (uint8_t)((double)green * current_brightness[0]);
  // blue = (uint8_t)((double)blue * current_brightness[0]);

#if P_SONG_COLOR > 0
  dprint(P_SONG_COLOR, " r: ");
  dprint(P_SONG_COLOR, red);
  dprint(P_SONG_COLOR, " g: ");
  dprint(P_SONG_COLOR, green);
  dprint(P_SONG_COLOR, " b: ");
  dprintln(P_SONG_COLOR, blue);
#endif

  for (int i = 0; i < num_channels; i++) {
    // adding thee target_brightness multiplier will weight the overall brightness in favor
    // of sounds which have more higher frequency content.
    neos[i].colorWipe(red, green, blue, current_brightness[0] * (1.0 + (target_brightness * 0.25)));
  }
  /////////////////// LOCAL SCALER RESET //////////////////////////
  // this needs to be after the rest of the logic in this function
  // when it was before it was adjusting the seed values to do odd things
  if (millis() > ((1000 * 60) + BOOT_DELAY) && feature_reset_tmr > feature_reset_time) {
    color_feature_min += ((color_feature_max - color_feature_min) * 0.05);
    color_feature_max -= ((color_feature_max - color_feature_min) * 0.05);
    for (int t = 0; t < num_channels; t++) {
      brightness_feature_min[t] += ((brightness_feature_max[t] - brightness_feature_min[t]) * 0.05);
      brightness_feature_max[t] -= ((brightness_feature_max[t] - brightness_feature_min[t]) * 0.05);
    }
    dprintln(P_SONG_DEBUG, "reset song feature min and max for cent and brightness ");
    feature_reset_tmr = 0;
  }
#if P_UPDATE_SONG_LENGTH == 1
  Serial.print("updateSong() function call length: ");
  Serial.println(song_update_timer - m);
#endif // P_NUM_SONG_UPDATES
}

double updateScalers(double val, double & min, double & max, double rate) {
  double nval = val;
  if (val < min) {
    min = (min * rate) + (val * (1.0 - rate));
    nval = min;
  }
  if (val > max) {
    max = (max * rate) + (val * (1.0 - rate));
    nval = max;
  }
  return nval;
}

void updateClick() {
  // for a click in theory the spectral flux will be high, the
  // centroid will decrease since the last frame, and there should be an
  // increase of amplitude in the 1k - 3k freq range

  // for all these values we want a 1.0 to be a confident assesment
  // from that feature

  /////////////////////////// Spectral Flux //////////////////////////
  double flux = fft_features.getSpectralFlux();
  flux = updateScalers(flux, min_flux, max_flux, 0.5);
  last_flux = click_flux;
  click_flux = (flux / (max_flux));
  dprint(P_CLICK_FLUX, "click flux: ");
  dprintln(click_flux);

  //////////////////////////// Cent Neg Delta ////////////////////////

  double new_cent_neg_delta = fft_features.getCentroidNegDelta();
  new_cent_neg_delta = updateScalers(new_cent_neg_delta, min_cent_negd, max_cent_negd, 0.5);
  last_cent_neg_delta = click_cent;

  // Serial.print("click_cent/min/max: ");
  // Serial.print((new_cent_neg_delta), 8);
  click_cent = (new_cent_neg_delta / max_cent_negd);
  /*Serial.print(" - ");
    Serial.print(min_cent_negd, 8);
    Serial.print(" - ");
    Serial.print(max_cent_negd, 8);
    Serial.print(" => ");
    Serial.println(click_cent, 8);
  */
  //////////////////////////// Energy between 1k - 3k //////////////////
  // using the bins instead of the hard coded frequencies saves some cycles
  // double range_rms = fft_features.getFFTRangeByFreq(1000, 2000);
  double range_rms = fft_features.getFFTRangeByIdx(23, 46);
  range_rms = updateScalers(range_rms, min_rrms, max_rrms, 0.5);
  last_range_rms = click_rrms;
  click_rrms = ((range_rms - min_rrms) / (max_rrms - min_rrms));

  //////////////////////////// Feature Calculation /////////////////////
  double feature = (click_flux) * click_cent * click_rrms * 50;
  feature = updateScalers(feature, min_click_feature, max_click_feature, 0.5);
  if (P_CLICK_FEATURES == true) {
    Serial.print("flux / range / feature   :\t");
    Serial.print((click_cent), 8);
    Serial.print("\t");
    Serial.print(click_flux, 8);
    Serial.print("\t");
    Serial.print(click_rrms, 8);
    Serial.print("\t");
    Serial.print(feature, 8);
    // dprintln(P_CLICK_DEBUG, feature);
    Serial.print("\t");
    Serial.println(feature, 8);
  }
  uint8_t red, green, blue;
  ////////////////// Calculate Actual Values ///////////////////////
  red = (current_color * CLICK_RED) * lux_manager.getBrightnessScaler();
  green = (current_color * CLICK_GREEN) * lux_manager.getBrightnessScaler();
  blue = (current_color * CLICK_BLUE) * lux_manager.getBrightnessScaler();

  if (feature >= 1.0) {
    // Serial.println("____________________ CLICK ________________________ 3.0");
    /*dprint(P_CLICK_DEBUG, "click feature is above threshold: ");
      dprint(P_CLICK_DEBUG, current_feature[i]);
      dprint(P_CLICK_DEBUG, " - ");
      dprint(P_CLICK_DEBUG, threshold);
    */
    for (int i = 0; i < num_channels; i++) {
      neos[i].colorWipeAdd(red, green, blue, lux_manager.getBrightnessScaler() * MASTER_SENSITIVITY_SCALER);
    }
  }

  /////////////////// LOCAL SCALER RESET //////////////////////////
  // this needs to be after the rest of the logic in this function
  // when it was before it was adjusting the seed values to do odd things
  if (millis() > ((1000 * 60) + BOOT_DELAY) && click_feature_reset_tmr > click_feature_reset_time) {
    min_rrms += ((max_rrms - min_rrms) * 0.05);
    max_rrms -= ((max_rrms - min_rrms) * 0.05);
    min_flux += ((max_flux - min_flux) * 0.05);
    max_flux -= ((max_flux - min_flux) * 0.05);
    min_cent_negd += ((max_cent_negd - min_cent_negd) * 0.05);
    max_cent_negd -= ((max_cent_negd - min_cent_negd) * 0.05);
    dprintln(P_SONG_DEBUG, "reset click feature min and max for rrms, flux and cent_negd ");
    click_feature_reset_tmr = 0;
  }
}

void updateAutogain() {
#if (AUTOGAIN_ACTIVE)
  auto_gain[0].updateExternal((neos[0].getOnRatio() + neos[1].getOnRatio()) * 0.5);
  auto_gain[1].updateExternal((neos[0].fpm + neos[1].fpm) * 0.5);
  return;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Start of PITCH functions ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// PITCH MODE /////////////////////////////////////////////////////////////////
#define STARTING_PITCH_GAIN                4.0
#define PITCH_AUTOGAIN_MIN                 2.0
#define PITCH_AUTOGAIN_MAX                 8.0

// Settings to cover the BiQuad Filter
#define BQ_Q                               0.5
// this can't be lower than 400 or else bad things happen =(
#define BQ_THRESH                          400
#define BQ_SHELF                           -12

/////////////////////////////// Feature Collector /////////////////////////////////
// Which Audio features will be activated?
#define FFT_FEATURE_ACTIVE                  1
#define PEAK_FEATURE_ACTIVE                 1
#define RMS_FEATURE_ACTIVE                  1
// these features are not currently implemented
// #define TONE_FEATURE_ACTIVE                 1
// #define FREQ_FEATURE_ACTIVE                 1

/////////////////////////////// Audio Features ////////////////////////////////////
// all the different features that are available to use through the feature collector for
// varioud mappings.
#define FEATURE_RMS                         (1)
#define FEATURE_RMS_POS_DELTA               (11)
#define FEATURE_RMS_AVG                     (12)
#define FEATURE_PEAK                        (2)
#define FEATURE_PEAK_POS_DELTA              (22)
#define FEATURE_PEAK_AVG                    (23)
#define FEATURE_FREQ                        (3)
#define FEATURE_TONE                        (4)
#define FEATURE_FFT                         (5)
#define FEATURE_FFT_ENERGY                  (6)
#define FEATURE_FFT_RELATIVE_ENERGY         (7)
#define FEATURE_FFT_MAX_BIN                 (8)
#define FEATURE_FFT_BIN_RANGE               (9)
#define FEATURE_STRONG_FFT                  (10)

//////////////////////////// FFT //
// When calculating things such as which bin has the most energy and so on,
// what bin is considered the "1st?" and which one is the last?
// todo what frequency does this correspond to?
#define FFT_LOWEST_BIN              1
// todo this needs to be calculated better?
#define FFT_NUM_BINS                511
#define FFT_HIGHEST_BIN             80
// when using the Freq function generator, what amount of uncertanity is allowed?
// #define FREQ_UNCERTANITY_ALLOWED    0.15

/////////////////////////////// Color Mapping /////////////////////////////////////
// when calculating the hue for the NeoPixel leds, what feature do you want to use?
// look under Audio Features for the Available Features
#define HUE_FEATURE                         (FEATURE_FFT_MAX_BIN)
#define BRIGHTNESS_FEATURE                  (FEATURE_PEAK_AVG)
#define SATURATION_FEATURE                  (FEATURE_PEAK_AVG)

// These are different color mapping modes
#define COLOR_MAPPING_RGB                     0
#define COLOR_MAPPING_HSB                     1

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#define COLOR_MAP_MODE              (COLOR_MAPPING_HSB)

#define  MODE_SINGLE_RANGE          0
#define  MODE_ALL_BINS              1
#define  MODE_BIN_ENERGY            2

bool getColorFromFFTSingleRange(FFTManager1024 *f, uint8_t s, uint8_t e) {
  double tot, frac;
  frac = f->getFFTRangeByIdx(s, e);
  tot = f->getFFTRangeByIdx(FFT_LOWEST_BIN, 128);
  frac = frac / tot;
  // RGBConverter::HsvToRgb(frac, 0.5, 1, 0, red, green, blue);
  return 1;
}

bool getHueFromTone(FeatureCollector *f) {
  Serial.print("WARNING - getColorFromTone is not currently implemented\t");
  // Serial.println(f->getToneLevel());
  return true;
}


double getHueFromFFTAllBins(FFTManager1024 *f) {
  /*if (COLOR_MAP_MODE == COLOR_MAPPING_RGB) {
    double red_d, green_d, blue_d, tot;
    red_d   = f->getFFTRange(FFT_LOWEST_BIN, 7);
    green_d = f->getFFTRange(7, 20);
    blue_d  = f->getFFTRange(20, 128);
    tot   = (red_d + green_d + blue_d);
    red_d   = red_d / tot;
    green_d = green_d / tot;
    blue_d  = blue_d / tot;
    rgb[chan][0] = red_d * MAX_BRIGHTNESS;
    rgb[chan][1] = green_d * MAX_BRIGHTNESS;
    rgb[chan][2] = blue_d * MAX_BRIGHTNESS * global_brightness_scaler;
    }
    else*/
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    return (double) map(f->getHighestEnergyIdx(), 0, 512, 0, 1000) / 1000.0;
    // dprint(P_FFT_VALS, "FFT - All Bins - HSB - Hue:\t"); dprintln(P_FFT_VALS, h);
  } else {
    Serial.println("ERROR - the COLOR_MAP_MODE is not currently implemented");
    return 0.0;
  }
}

double calculateBrightness(FeatureCollector *f, FFTManager1024 *_fft) {
  double b;
  if (BRIGHTNESS_FEATURE == FEATURE_PEAK_AVG) {
    b = f->getPeakAvg();
    if (b > 1.0) {
      b =  1.0;
    }
    f->resetPeakAvgLog();
  } else if (BRIGHTNESS_FEATURE == FEATURE_RMS_AVG) {
    b = f->getRMSAvg();
    if (b > 1.0) {
      b =  1.0;
    }
    f->resetRMSAvgLog();
  }
  else if (BRIGHTNESS_FEATURE == FEATURE_RMS) {
    b = f->getRMS();
    if (b > 1.0) {
      b =  1.0;
    }
  }
  else if (BRIGHTNESS_FEATURE == FEATURE_FFT_ENERGY) {
    b = _fft->getFFTTotalEnergy();
  } else  if (BRIGHTNESS_FEATURE == FEATURE_STRONG_FFT) {
    // range index is what the highest energy bin is within the range we care about
    uint16_t range_idx = _fft->getHighestEnergyIdx();
    uint16_t tot_idx = _fft->getHighestEnergyIdx();
    if (range_idx != tot_idx) {
      b = 0.0;
    } else {
      b = _fft->getFFTTotalEnergy();
    }
  } else {
    Serial.println("ERROR - calculateBrightness() does not accept that  BRIGHTNESS_FEATURE");
  }
  return b;
}

double calculateSaturation(FeatureCollector *f, FFTManager1024 *_fft) {
  double sat = 0.0;
  if (SATURATION_FEATURE == FEATURE_PEAK_AVG) {
    sat = f->getPeakAvg();
    Serial.println(sat);
    if (sat > 1.0) {
      sat =  1.0;
    }
    // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
    f->resetPeakAvgLog();
  } else if (SATURATION_FEATURE == FEATURE_RMS_AVG) {
    sat = f->getRMSAvg();
    if (sat > 1.0) {
      sat =  1.0;
    }
    // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
    f->resetRMSAvgLog();
  }
  else if (SATURATION_FEATURE == FEATURE_FFT_RELATIVE_ENERGY) {
    // get how much energy is stored in the max bin, get the amount of energy stored in all bins
    sat = _fft->getRelativeEnergy(_fft->getHighestEnergyIdx()) * 1000.0;
    if (sat > 1.0) {
      sat = 1.0;
    }
    // Serial.print("relative energy in highest energy bin: ");Serial.println(sat);
  }
  else {
    Serial.print("ERROR - calculateSaturation() does not accept that  SATURATION_FEATURE");
  }
  return sat;
}

double last_hue = 0.0;
double hue = 0.0;

double calculateHue(FeatureCollector *f, FFTManager1024 *_fft) {
  last_hue = (last_hue * 0.95) + (hue * 0.05);
  hue = 0.0;
  switch (HUE_FEATURE) {
    case FEATURE_FFT_BIN_RANGE:
      hue = getColorFromFFTSingleRange(_fft, 3, 20);
      break;
    case FEATURE_FFT:
      hue = getHueFromFFTAllBins(_fft);
      break;
    case FEATURE_FFT_MAX_BIN:
      // calculate the bin with the most energy,
      // Serial.print("Highest energy bin is: ");Serial.println(f->getHighestEnergyBin(FFT_LOWEST_BIN, FFT_HIGHEST_BIN));
      // map the bin  index to a hue value
      hue = (double) (_fft->getHighestEnergyIdx(FFT_LOWEST_BIN, FFT_HIGHEST_BIN) - FFT_LOWEST_BIN) / FFT_HIGHEST_BIN;
      // Serial.print("max bin hu e is : ");Serial.println(hue);
      break;
    case FEATURE_TONE:
      hue = getHueFromTone(f);
      break;
    case FEATURE_PEAK_AVG:
      hue = f->getPeakAvg();
      f->resetPeakAvgLog();
      break;
    case FEATURE_PEAK:
      hue = f->getPeak();
      break;
    case FEATURE_RMS_AVG:
      hue = f->getRMSAvg();
      break;
    case FEATURE_RMS:
      hue = f->getRMS();
      break;
    case DEFAULT:
      Serial.println("ERROR - calculateHue() does not accept that HUE_FEATURE");
      break;
  }
  return hue;
}

void updateNeosForPitch() {
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    uint8_t inactive = 0;
    // calculate HSB
    // the brightness should be the loudness (overall amp of bins)
    // the saturation should be the relatitive loudness of the primary bin
    // the hue should be the bin number (With higher frequencies corresponding to reds and yellows)
    double s = calculateSaturation(&fc[0], &fft_features);
    double b = calculateBrightness(&fc[0], &fft_features);
    double h = calculateHue(&fc[0], &fft_features);
    Serial.print("h: ");
    Serial.print(h);
    Serial.print("\ts: ");
    Serial.print(s);
    Serial.print("\tb");
    Serial.println(b);
    for (int chan = 0; chan < num_channels; chan++) {
      if (fc[0].isActive() == true) {
        // now colorWipe the LEDs with the HSB value
        // if (h > 0) {
        neos[chan].colorWipeHSB(h, s, b);
        /*} else {
          neos[chan].colorWipeHSB(0, 0, 0);
          }*/
      } else {
        inactive++;
      }
    }
    if (inactive > num_channels) {
      Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
      return;
    }
  }
  else {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }
}

void printColors() {
  fft_features.printFFTVals();
  for (int i = 0; i < NUM_NEO_GROUPS; i++)  {
    neos[i].printColors();
  }
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// END of PITCH ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void updateMode() {
  if (lux_manager.getExtremeLux() == true) {
    dprintln(P_LUX_DEBUG, "WARNING ------------ updateMode() returning due extreme lux conditions, not updating click or song...");
    return;
  }

  if (FIRMWARE_MODE == CICADA_MODE) {
    updateSong();
#if CLICK_ACTIVE > 0
    updateClick();
#endif // CLICK_ACTIVE
  }
  else if (FIRMWARE_MODE == PITCH_MODE) {
    updateNeosForPitch();
    printColors();
  }

#if P_AUDIO_MEMORY_MAX > 0
  Serial.print("audio memory max: ");
  Serial.print(AudioMemoryUsageMax());
#endif
}


#endif // __MODE_H__
