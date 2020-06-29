#ifndef __MODE_CICADA_H__
#define __MODE_CICADA_H__
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
  // fc 0-1 are for the song front/rear
  fc[0].linkPeak(&peak1, global_peak_scaler, PRINT_PEAK_VALS);
  fc[1].linkPeak(&peak2, global_peak_scaler, PRINT_PEAK_VALS);

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
  amp1.gain(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
  amp2.gain(STARTING_GAIN * ENC_ATTENUATION_FACTOR);
  // set gain level? automatically?
  // todo make this adapt to when microphones are broken on one or more side...
  printDivide();
}

double calculateSongBrightness(uint8_t i) {
  // how much energy is stored in the range of 4000 - 16000 compared to  the entire spectrum?
  // take the average of two-octave chunks from 1-4k and 250 - 1k
  double remaining_spect_avg = 0.5 * (fft_features.getFFTRangeByFreq(250, 1000) + fft_features.getFFTRangeByFreq(1000, 4000));
  double target_brightness = fft_features.getFFTRangeByFreq(4000, 16000) - remaining_spect_avg;
  if (target_brightness < 0.0) {
    target_brightness = 0.0;
    brightness_feature_min[i] = 0.0;
  }
  /*
  if (target_brightness < brightness_feature_min[i]) {
    if (i == 0 && PRINT_SONG_BRIGHTNESS) {
      dprint(PRINT_SONG_BRIGHTNESS, "target_B is less than feature_min: ");
      dprint(PRINT_SONG_BRIGHTNESS, target_brightness, 5);
      dprint(PRINT_SONG_BRIGHTNESS, " < ");
      dprint(PRINT_SONG_BRIGHTNESS, brightness_feature_min[i], 5);
    }
    brightness_feature_min[i] = (target_brightness * 0.15) + (brightness_feature_min[i] * 0.85);
    if (i == 0 && PRINT_SONG_BRIGHTNESS) {
      dprint(PRINT_SONG_BRIGHTNESS, " updated brightness_min and target_brightness to: ");
      dprintln(PRINT_SONG_BRIGHTNESS, brightness_feature_min[i], 5);
    }
    target_brightness = brightness_feature_min[i];
  }
  */
  if (target_brightness > brightness_feature_max[i]) {
    if (i == 0 && PRINT_SONG_BRIGHTNESS) {
      dprint(PRINT_SONG_BRIGHTNESS, "target_B is more than feature_max: ");
      dprint(PRINT_SONG_BRIGHTNESS, target_brightness, 5);
      dprint(PRINT_SONG_BRIGHTNESS, " > ");
      dprintln(PRINT_SONG_BRIGHTNESS, brightness_feature_max[i], 5);
    }
    brightness_feature_max[i] = (target_brightness * BRIGHTNESS_LP_LEVEL) + (brightness_feature_max[i] * (1.0 - BRIGHTNESS_LP_LEVEL));
    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max[i] > 1.0) {
      brightness_feature_max[i] = 1.0;
    }
    if (i == 0 && PRINT_SONG_BRIGHTNESS) {
      dprint(PRINT_SONG_BRIGHTNESS, " updated brightness_max and target_brightness to: ");
      dprintln(PRINT_SONG_BRIGHTNESS, brightness_feature_max[i], 5);
    }
    target_brightness = brightness_feature_max[i];
  }
  dprint(PRINT_SONG_BRIGHTNESS, "channel ");
  dprint(PRINT_SONG_BRIGHTNESS, i);
  dprint(PRINT_SONG_BRIGHTNESS, " target: ");
  dprint(PRINT_SONG_BRIGHTNESS, target_brightness);
  dprint(PRINT_SONG_BRIGHTNESS, "\tmin: ");
  dprint(PRINT_SONG_BRIGHTNESS, brightness_feature_min[i]);
  dprint(PRINT_SONG_BRIGHTNESS, "\tmax: ");
  dprint(PRINT_SONG_BRIGHTNESS, brightness_feature_max[i]);  
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
  dprint(PRINT_SONG_BRIGHTNESS, " adjusted: ");
  dprintln(PRINT_SONG_BRIGHTNESS, target_brightness);
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
  double target_color = 0.0;        // 0.0 - 1.0
  uint8_t red, green, blue;

  /////////////////// Color ////////////////////////////////////////
  target_color = calculateSongColor();
  last_color = current_color;
  // current_color = (target_color * 0.5) + (last_color * 0.5);// * COLOR_LP_LEVEL);
  current_color = (target_color * COLOR_LP_LEVEL) + (last_color * (1.0 - COLOR_LP_LEVEL));
  // current_color[i] = current_color[i] + (last_color[i] * (1.0 - COLOR_LP_LEVEL));
  dprint(PRINT_SONG_COLOR, "target_color: ");
  dprint(PRINT_SONG_COLOR, target_color);
  dprint(PRINT_SONG_COLOR, "\tlast_color: ");
  dprint(PRINT_SONG_COLOR, last_color);
  dprint(PRINT_SONG_COLOR, "\tcurrent_color: ");
  dprint(PRINT_SONG_COLOR, current_color);

  ////////////////// Calculate Actual Values ///////////////////////
  red = ((1.0 - current_color) * SONG_RED_LOW) + (current_color * SONG_RED_HIGH);
  green = ((1.0 - current_color) * SONG_GREEN_LOW) + (current_color * SONG_GREEN_HIGH);
  blue = ((1.0 - current_color) * SONG_BLUE_LOW) + (current_color * SONG_BLUE_HIGH);

  double target_brightness = 0.0;
  for (int i = 0; i < num_channels; i++) {
    /////////////////// Brightness ///////////////////////////////////
    double __brightness = calculateSongBrightness(i);
    if (__brightness > target_brightness) {
      target_brightness = __brightness;
    }
  }
  last_brightness[0] = current_brightness[0];
  current_brightness[0] = (target_brightness * BRIGHTNESS_LP_LEVEL) + (last_brightness[0] * (1.0 - BRIGHTNESS_LP_LEVEL));

  dprint(PRINT_SONG_BRIGHTNESS, "last/current_brightness[");
  dprint(PRINT_SONG_BRIGHTNESS, 0);
  dprint(PRINT_SONG_BRIGHTNESS, "]: ");
  dprint(PRINT_SONG_BRIGHTNESS, last_brightness[0]);
  dprint(PRINT_SONG_BRIGHTNESS, " => ");
  dprintln(PRINT_SONG_BRIGHTNESS, current_brightness[0]);

  red = (uint8_t)((double)red * current_brightness[0]);
  green = (uint8_t)((double)green * current_brightness[0]);
  blue = (uint8_t)((double)blue * current_brightness[0]);

  dprint(PRINT_SONG_COLOR, " r: ");
  dprint(PRINT_SONG_COLOR, red);
  dprint(PRINT_SONG_COLOR, " g: ");
  dprint(PRINT_SONG_COLOR, green);
  dprint(PRINT_SONG_COLOR, " b: ");
  dprintln(PRINT_SONG_COLOR, blue);

  for (int i = 0; i < num_channels; i++) {
    neos[i].colorWipe(red, green, blue, 1.0);
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
    dprintln(PRINT_SONG_DEBUG, "reset song feature min and max for cent and brightness ");
    feature_reset_tmr = 0;
  }
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
  dprint(PRINT_CLICK_FLUX, "click flux: ");
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
  double range_rms = fft_features.getFFTRangeByFreq(1000, 2000);
  range_rms = updateScalers(range_rms, min_rrms, max_rrms, 0.5);
  last_range_rms = click_rrms;
  click_rrms = ((range_rms - min_rrms) / (max_rrms - min_rrms));

  //////////////////////////// Feature Calculation /////////////////////
  double feature = (click_flux) * click_cent * click_rrms * 50;
  feature = updateScalers(feature, min_click_feature, max_click_feature, 0.5);
  if (PRINT_CLICK_FEATURES == true) {
    Serial.print("flux / range / feature   :\t");
    Serial.print((click_cent), 8);
    Serial.print("\t");
    Serial.print(click_flux, 8);
    Serial.print("\t");
    Serial.print(click_rrms, 8);
    Serial.print("\t");
    Serial.print(feature, 8);
    // dprintln(PRINT_CLICK_DEBUG, feature);
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
    /*dprint(PRINT_CLICK_DEBUG, "click feature is above threshold: ");
      dprint(PRINT_CLICK_DEBUG, current_feature[i]);
      dprint(PRINT_CLICK_DEBUG, " - ");
      dprint(PRINT_CLICK_DEBUG, threshold);
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
    dprintln(PRINT_SONG_DEBUG, "reset click feature min and max for rrms, flux and cent_negd ");
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

void updateMode() {
  if (lux_manager.getExtremeLux() == true) {
    dprintln(PRINT_LUX_DEBUG, "WARNING ------------ updateMode() returning due extreme lux conditions, not updating click or song...");
    return;
  }
  updateSong();
  updateClick();
  // Serial.print("audio memory max: ");
  // Serial.print(AudioMemoryUsageMax());
}


#endif // __MODE_CICADA_H__
