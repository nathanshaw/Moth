#ifndef __MODE_H__
#define __MODE_H__

#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include <NeopixelManager.h>
#include <LuxManager.h>

#if HV_MAJOR > 2
#include "WeatherManager/WeatherManager.h"
#endif // HV_MAJOR

#include "AudioEngine/AutoGain.h"
#include "AudioEngine/FFTManager1024.h"
#include <Audio.h>

//////////////////////////////// Global Objects /////////////////////////
WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);

NeoGroup neos = NeoGroup(&leds, 0, NUM_LED - 1, "All Neos", MIN_FLASH_TIME, MAX_FLASH_TIME);

// lux managers to keep track of the VEML readings
LuxManager lux_manager = LuxManager(lux_min_reading_delay, lux_max_reading_delay, LUX_MAPPING_SCHEMA);
// weather manager keeps track of temp and humidity sensors.
#if HV_MAJOR > 2
WeatherManager weather_manager = WeatherManager(HUMID_EXTREME_THRESH, TEMP_EXTRME_THRESH, TEMP_HISTORESIS);
#endif // HV_MAJOR

FeatureCollector fc = FeatureCollector("All");

DLManager datalog_manager = DLManager((String)"Datalog Manager");
FFTManager1024 fft_features = FFTManager1024("Input FFT");

AutoGain auto_gain = AutoGain(&fc, &fft_features, MIN_GAIN, MAX_GAIN, MAX_GAIN_ADJ);

////////////////////////// Audio Objects //////////////////////////////////////////
AudioInputI2S            i2s1;           //xy=55,291.8571424484253
AudioFilterBiquad        biquad2;        //xy=217.00389099121094,302.0039281845093
AudioFilterBiquad        biquad1;        //xy=219.00390625,270.00391578674316
AudioAmplifier           amp2;           //xy=378.79129791259766,302.57704162597656
AudioAmplifier           amp1;           //xy=380.2198715209961,264.0055875778198
AudioAnalyzePeak         peak2;          //xy=517.0039100646973,316.003924369812
AudioAnalyzePeak         peak1;          //xy=521.00390625,221.0039176940918
AudioAnalyzeFFT1024      input_fft;      //xy=521.3627586364746,251.71987438201904
AudioAnalyzeRMS          rms1;            //xy=650.0000076293945,151.00000190734863
AudioAnalyzeRMS          rms2;            //xy=650.0000076293945,151.00000190734863

AudioConnection          patchCord10(amp1, rms1);
AudioConnection          patchCord11(amp2, rms2);

#if AUDIO_USB > 0
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

void setupAudio() {
  ////////////// Audio ////////////
  Serial.print("Setting up Audio Parameters");
  AudioMemory(AUDIO_MEMORY);
  Serial.print("Audio Memory has been set to: ");
  Serial.println(AUDIO_MEMORY);
  /////////////////////////////////////////////////////////////////////
  // fc 0-1 are for the song front/rear
  if (PEAK_FEATURE_ACTIVE) {
    fc.linkPeak(&peak1, P_PEAK_VALS);
    fc.linkPeak(&peak2, P_PEAK_VALS);
  }
  if (RMS_FEATURE_ACTIVE) {
    fc.linkRMS(&rms1, P_RMS_VALS);
    fc.linkRMS(&rms2, P_RMS_VALS);
  }
  if (FFT_FEATURES_ACTIVE) {
    fft_features.linkFFT(&input_fft, true);
    Serial.println("Linked FFT to FFTManager");
    if (FIRMWARE_MODE == CICADA_MODE) {
      fft_features.setupCentroid(true, 4000, 16000);
    } else if (FIRMWARE_MODE == PITCH_MODE) {
      fft_features.setupCentroid(true, 120, 16000);
    }
    Serial.println("Started calculating Centroid in the FFTManager");
    fft_features.setFluxActive(true);
    Serial.println("Started calculating FLUX in the FFTManager");
  }
  Serial.println("Feature collectors have been linked");
  /////////////////////////////////////////////////////////////////////
  biquad1.setHighpass(0, 30, 0.85);
  biquad1.setHighpass(1, 50, 0.85);
  biquad1.setHighpass(2, 60, 0.85);
  biquad1.setLowShelf(3, 80, ONSET_BQ1_DB);
  Serial.println("Biquad filter 1 has been configured");
  biquad2.setHighpass(0, 8000, 0.85);
  biquad2.setHighpass(1, 8000, 0.85);
  biquad2.setHighpass(2, 8000, 0.85);
  biquad2.setLowShelf(3, 8000, ONSET_BQ1_DB);
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

double calculateSongBrightness() {
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
    brightness_feature_min = 0.0;
  }
  
    if (target_brightness < brightness_feature_min) {
    if (P_BRIGHTNESS) {
      dprint(P_BRIGHTNESS, "target_B is less than feature_min: ");
      dprint(P_BRIGHTNESS, target_brightness, 5);
      dprint(P_BRIGHTNESS, " < ");
      dprint(P_BRIGHTNESS, brightness_feature_min, 5);
    }
    brightness_feature_min = (target_brightness * 0.15) + (brightness_feature_min * 0.85);
    if ( P_BRIGHTNESS) {
      dprint(P_BRIGHTNESS, " updated brightness_min and target_brightness to: ");
      dprintln(P_BRIGHTNESS, brightness_feature_min, 5);
    }
    target_brightness = brightness_feature_min;
    }
  
  if (target_brightness > brightness_feature_max) {
    dprint(P_BRIGHTNESS, "target_B is more than feature_max: ");
    dprint(P_BRIGHTNESS, target_brightness, 5);
    dprint(P_BRIGHTNESS, " > ");
    dprintln(P_BRIGHTNESS, brightness_feature_max, 5);

    brightness_feature_max = (target_brightness * BRIGHTNESS_LP_LEVEL) + (brightness_feature_max * (1.0 - BRIGHTNESS_LP_LEVEL));

    // to ensure that loud clipping events do not skew things too much
    if (brightness_feature_max > 1.0) {
      brightness_feature_max = 1.0;
    }

    dprint(P_BRIGHTNESS, " updated brightness_max and target_brightness to: ");
    dprintln(P_BRIGHTNESS, brightness_feature_max, 5);

    target_brightness = brightness_feature_max;
  }
  dprint(P_BRIGHTNESS, " target: ");
  dprint(P_BRIGHTNESS, target_brightness);
  dprint(P_BRIGHTNESS, "\tmin: ");
  dprint(P_BRIGHTNESS, brightness_feature_min);
  dprint(P_BRIGHTNESS, "\tmax: ");
  dprint(P_BRIGHTNESS, brightness_feature_max);
  // to ensure the unit is not always on
  // instead of comparing the brightness to the cuttoff_threshold
  // we subtract the cuttoff_threshold from both the target and the max
  target_brightness = (target_brightness - brightness_feature_min) / (brightness_feature_max - brightness_feature_min);
  target_brightness = target_brightness - BRIGHTNESS_CUTTOFF_THRESHOLD;
  if (target_brightness < 0.0) {
    target_brightness = 0.0;
  } else if (target_brightness > 1.0) {
    target_brightness = 1.0;
  }
  dprint(P_BRIGHTNESS, " adjusted: ");
  dprintln(P_BRIGHTNESS, target_brightness);
#if P_CALCULATE_BRIGHTNESS_LENGTH > 0
  Serial.print("calculateSongBrightness() function call length: ");
  Serial.println(millis() - _start);
#endif
  return target_brightness;
}


double calculateColorFromCentroid() {
  double cent = fft_features.getCentroid();       // right now we are only polling the first FC for its centroid to use to color both sides
  // this is basically some loc
  if (cent < color_feature_min) {
    dprint(P_HUE, "cent is less than color_feature_min changing to min: ");
    dprint(P_HUE, cent);
    dprint(P_HUE, " / ");
    dprintln(P_HUE, color_feature_min);
    color_feature_min = (color_feature_min * (1.0 - feature_reset_factor)) + (cent * feature_reset_factor);
    cent = color_feature_min;
  }
  if (cent > color_feature_max) {
    dprint(P_HUE, "cent is more than color_feature_max changing to max: ");
    dprint(P_HUE, cent);
    dprint(P_HUE, " / ");
    dprintln(P_HUE, color_feature_max);
    color_feature_max = (color_feature_max * (1.0 - feature_reset_factor)) + (cent * feature_reset_factor);
    cent = color_feature_max;
  }
  dprint(P_HUE, "hue before/after scaling:\t");
  dprint(P_HUE, cent);
  cent = (cent - color_feature_min) / (color_feature_max - color_feature_min);
  dprint(P_HUE, " / ");
  dprintln(P_HUE, cent);
  dprint(P_HUE, "hue min/max:\t\t\t");
  dprint(P_HUE, color_feature_min);
  dprint(P_HUE, " / ");
  dprintln(P_HUE, color_feature_max);
  /////////////////// LOCAL SCALER RESET //////////////////////////
  // this needs to be after the rest of the logic in this function
  // when it was before it was adjusting the seed values to do odd things
  if (millis() > ((1000 * 60) + BOOT_DELAY) && feature_reset_tmr > feature_reset_time) {
    color_feature_min += ((color_feature_max - color_feature_min) * feature_reset_factor);
    color_feature_max -= ((color_feature_max - color_feature_min) * feature_reset_factor);
    brightness_feature_min += ((brightness_feature_max - brightness_feature_min) * feature_reset_factor);
    brightness_feature_max -= ((brightness_feature_max - brightness_feature_min) * feature_reset_factor);
    hue_min += ((hue_max - hue_min) * feature_reset_factor);
    hue_max -= ((hue_max - hue_min) * feature_reset_factor);
    dprintMinorDivide(P_HUE);
    dprintln(P_HUE, "reset song feature min and max for cent and brightness ");
    dprint(P_HUE, "hue min/max:\t\t\t");
    dprint(P_HUE, color_feature_min);
    dprint(P_HUE, " / ");
    dprintln(P_HUE, color_feature_max);
    dprintMinorDivide(P_HUE);
    feature_reset_tmr = 0;
  }
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
  target_color = calculateColorFromCentroid();
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
  /////////////////// Brightness ///////////////////////////////////
  double __brightness = 0.0;
  if (target_brightness == 0.0) {
    __brightness = calculateSongBrightness();
  } else if (STEREO_FEEDBACK == true) {
    __brightness = calculateSongBrightness();
  }
  if (__brightness > target_brightness) {
    target_brightness = __brightness;
  }
  last_brightness = current_brightness;
  current_brightness = (target_brightness * BRIGHTNESS_LP_LEVEL) + (last_brightness * (1.0 - BRIGHTNESS_LP_LEVEL));

#if P_BRIGHTNESS > 0
  dprint(P_BRIGHTNESS, "last/current_brightness");
  dprint(P_BRIGHTNESS, last_brightness);
  dprint(P_BRIGHTNESS, " => ");
  dprintln(P_BRIGHTNESS, current_brightness);
#endif

  // red = (uint8_t)((double)red * current_brightness);
  // green = (uint8_t)((double)green * current_brightness);
  // blue = (uint8_t)((double)blue * current_brightness);

#if P_SONG_COLOR > 0
  dprint(P_SONG_COLOR, " r: ");
  dprint(P_SONG_COLOR, red);
  dprint(P_SONG_COLOR, " g: ");
  dprint(P_SONG_COLOR, green);
  dprint(P_SONG_COLOR, " b: ");
  dprintln(P_SONG_COLOR, blue);
#endif

  // adding thee target_brightness multiplier will weight the overall brightness in favor
  // of sounds which have more higher frequency content.
  neos.colorWipe(red, green, blue, current_brightness);// * (1.0 + (target_brightness * 0.25)));
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

void updateOnset() {
#if ONSET_ACTIVE > 0
  // for a onset in theory the spectral flux will be high, the
  // centroid will decrease since the last frame, and there should be an
  // increase of amplitude in the 1k - 3k freq range

  // for all these values we want a 1.0 to be a confident assesment
  // from that feature

  /////////////////////////// Spectral Flux //////////////////////////
  double flux = fft_features.getSpectralFlux();
  flux = updateScalers(flux, min_flux, max_flux, 0.5);
  last_flux = onset_flux;
  onset_flux = (flux / (max_flux));
  dprint(P_ONSET_FLUX, "onset flux: ");
  dprintln(P_ONSET_FLUX, onset_flux);

  //////////////////////////// Cent Neg Delta ////////////////////////
  double new_cent_degd = fft_features.getCentroidNegDelta();
  new_cent_degd = updateScalers(new_cent_degd, min_cent_negd, max_cent_negd, 0.5);
  last_cent_degd = onset_cent;

  // Serial.print("onset_cent/min/max: ");
  // Serial.print((new_cent_degd), 8);
  onset_cent = (new_cent_degd / max_cent_negd);

  //////////////////////////// Energy between 1k - 3k //////////////////
  // using the bins instead of the hard coded frequencies saves some cycles
  // double range_rms = fft_features.getFFTRangeByFreq(1000, 2000);
  double range_rms = fft_features.getFFTRangeByIdx(23, 46);
  range_rms = updateScalers(range_rms, min_rrms, max_rrms, 0.5);
  last_range_rms = onset_rrms;
  onset_rrms = ((range_rms - min_rrms) / (max_rrms - min_rrms));

  //////////////////////////// Feature Calculation /////////////////////
  double feature = (onset_flux) * onset_cent * onset_rrms * 50;
  feature = updateScalers(feature, min_onset_feature, max_onset_feature, 0.5);
  if (P_ONSET_FEATURES == true) {
    Serial.print("flux / range / feature   :\t");
    Serial.print((onset_cent), 8);
    Serial.print("\t");
    Serial.print(onset_flux, 8);
    Serial.print("\t");
    Serial.print(onset_rrms, 8);
    Serial.print("\t");
    Serial.print(feature, 8);
    // dprintln(P_ONSET, feature);
    Serial.print("\t");
    Serial.println(feature, 8);
  }
  uint8_t red, green, blue;
  ////////////////// Calculate Actual Values ///////////////////////
  red = (current_color * ONSET_RED) * lux_manager.getBrightnessScaler();
  green = (current_color * ONSET_GREEN) * lux_manager.getBrightnessScaler();
  blue = (current_color * ONSET_BLUE) * lux_manager.getBrightnessScaler();

  // note that normally 1.0 is the threshold for onsets
  if (feature >= ONSET_THRESH) {
    // Serial.println("____________________ ONSET ________________________ 3.0");
    /*dprint(P_ONSET, "onset feature is above threshold: ");
      dprint(P_ONSET, current_feature[i]);
      dprint(P_ONSET, " - ");
      dprint(P_ONSET, threshold);
    */
    neos.colorWipeAdd(red, green, blue, lux_manager.getBrightnessScaler() * user_brightness_scaler);
  }

  /////////////////// LOCAL SCALER RESET //////////////////////////
  // this needs to be after the rest of the logic in this function
  // when it was before it was adjusting the seed values to do odd things
  if (millis() > ((1000 * 60) + BOOT_DELAY) && onset_feature_reset_tmr > onset_feature_reset_time) {
    min_rrms += ((max_rrms - min_rrms) * 0.05);
    max_rrms -= ((max_rrms - min_rrms) * 0.05);
    min_flux += ((max_flux - min_flux) * 0.05);
    max_flux -= ((max_flux - min_flux) * 0.05);
    min_cent_negd += ((max_cent_negd - min_cent_negd) * 0.05);
    max_cent_negd -= ((max_cent_negd - min_cent_negd) * 0.05);
    dprintln(P_SONG, "reset onset feature min and max for rrms, flux and cent_negd ");
    onset_feature_reset_tmr = 0;
  }
#endif // ONSET_ACTIVE
}

void updateAutogain() {
#if (AUTOGAIN_ACTIVE)
  // these are the external values which are driving the auto-gain
  // in theory I think we want to use a combination of average RMS
  // and on/ratio to determine what the appropiate gain is for
  // the units 
  auto_gain.update();
  // after getting the on ratio and rms we need to then reset the values
  // neos.resetOnRatio();
  // fc.resetRMSAvg();
  return;
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Start of PITCH functions ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


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

double calculateHSBBrightness(FeatureCollector *f, FFTManager1024 *_fft) {
  double b;
  dprintMinorDivide(P_BRIGHTNESS);
  dprint(P_BRIGHTNESS, "calculating HSB Brightness: ");
  switch (BRIGHTNESS_FEATURE) {
    case (FEATURE_PEAK_AVG):
      dprintln(P_BRIGHTNESS, "feature is PEAK_AVG");
      b = f->getDominatePeakAvg();// - BRIGHTNESS_CUTTOFF_THRESHOLD;
      f->resetDominatePeakAvg();
      break;
    case (FEATURE_RMS_AVG):
      dprintln(P_BRIGHTNESS, "feature is RMS_AVG");
      b = f->getDominateRMSAvg();// - BRIGHTNESS_CUTTOFF_THRESHOLD;
      f->resetDominateRMSAvg();
      break;
    case (FEATURE_RMS):
      dprintln(P_BRIGHTNESS, "feature is RMS");
      b = f->getDominateRMS();// - BRIGHTNESS_CUTTOFF_THRESHOLD;
      break;
    case (FEATURE_FFT_ENERGY):
      dprintln(P_BRIGHTNESS, "feature is FFT_ENERGY");
      b = _fft->getFFTTotalEnergy();// - BRIGHTNESS_CUTTOFF_THRESHOLD;
      break;
    case (FEATURE_STRONG_FFT):
      // range index is what the highest energy bin is within the range we care about
      // b = _fft->getFFTTotalEnergy();
      Serial.println("WARNING - FEATURE_STONG_FFT is not currently implemented");
      break;
    default:
      Serial.println("ERROR - calculateHSBBrightness() does not accept that  BRIGHTNESS_FEATURE");
      break;
  }
  ///////////////////////// If user controls are in place to scale the brightness it is done now //////////////////////
  /////////////////////// Make sure that it is within bounds ////////////////////
  dprint(P_BRIGHTNESS, "brightness is: ");
  dprintln(P_BRIGHTNESS, b);
  if (b < 0) {
    dprint(P_BRIGHTNESS, " brightness too low, changing to 0.0");
    b = 0;
  } else if (b > 1.0) {
    b = 1.0;
    dprint(P_BRIGHTNESS, " brightness too high, changing to 1.0");
  }
  if (USER_BS_ACTIVE > 0) {
    dprint(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, "changing brightness due to user brightness_scaler | before: ");
    dprint(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, b);
    b = b * user_brightness_scaler;
    dprint(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, " after: ");
    dprintln(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, b);
  }

  //////////////////////// Scale down the brightness and make it more exponential for better results //////////////////
  if (SQUARE_BRIGHTNESS == true) {
    dprint(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, "changing brightness due to SQUARE_BRIGHTNESS | before: ");
    dprint(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, b);
    b = (b) * b;
    dprint(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, " after: ");
    dprintln(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, b);
  }
  dprintMinorDivide(P_BRIGHTNESS);
  return b;
}

double calculateSaturation(FeatureCollector *f, FFTManager1024 *_fft) {
  double sat = 0.0;
  switch (SATURATION_FEATURE) {
    case (FEATURE_PEAK_AVG):
      sat = f->getDominatePeakAvg();
      // Serial.println(sat);
      if (sat > 1.0) {
        sat =  1.0;
      }
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      f->resetDominatePeakAvg();
      break;
    case (FEATURE_RMS_AVG):
      sat = f->getDominateRMSAvg();
      if (sat > 1.0) {
        sat =  1.0;
      }
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      f->resetDominateRMSAvg();
      break;
    case (FEATURE_FFT_RELATIVE_ENERGY):
      // get how much energy is stored in the max bin, get the amount of energy stored in all bins
      sat = _fft->getRelativeEnergy(_fft->getHighestEnergyIdx()) * 1000.0;
      // Serial.print("relative energy in highest energy bin: ");Serial.println(sat);
      break;
    case (FEATURE_FLUX):
      sat = (_fft->getFlux() - 20) / 60;
      break;
    default:
      Serial.print("ERROR - calculateSaturation() does not accept that  SATURATION_FEATURE");
      break;
  }
  if (sat < sat_feature_min) {
    dprint(P_SATURATION, "sat is less than sat_feature_min changing to min: ");
    dprint(P_SATURATION, sat);
    dprint(P_SATURATION, " / ");
    dprintln(P_SATURATION, sat_feature_min);
    sat_feature_min = (sat_feature_min * 0.94) + (sat * 0.06);
    sat = sat_feature_min;
  }
  if (sat > sat_feature_max) {
    dprint(P_SATURATION, "sat is more than sat_feature_max changing to max: ");
    dprint(P_SATURATION, sat);
    dprint(P_SATURATION, " / ");
    dprintln(P_SATURATION, sat_feature_max);
    sat_feature_max = (sat_feature_max * 0.94) + (sat * 0.06);
    sat = sat_feature_max;
  }
  dprint(P_SATURATION, "hue before/after scaling: ");
  dprint(P_SATURATION, sat);
  sat = (sat - sat_feature_min) / (sat_feature_max - sat_feature_min);
  dprint(P_SATURATION, " / ");
  dprint(P_SATURATION, sat);
  dprint(P_SATURATION, "hue min/max: ");
  dprint(P_SATURATION, sat_feature_min);
  dprint(P_SATURATION, " / ");
  dprintln(P_SATURATION, sat_feature_max);
  if (sat > 1.0) {
    Serial.print("WARNING - saturation is above 1.0 at ");
    Serial.print(sat);
    Serial.println(" changing to 1.0");
    sat = 1.0;
  } else if (sat < 0.0) {
    Serial.print("WARNING - saturation is below 0.0 at ");
    Serial.print(sat);
    Serial.println(" changing to 0.0");
    sat = 0.0;
  }
  return sat;
}

double calculateHue(FeatureCollector *f, FFTManager1024 *_fft) {
  /* In theory, the Hue should be between 0.0 and 1.0 to be useful for the rest of the program

  */
  hue = 0.0;
  switch (HUE_FEATURE) {
    case FEATURE_FFT_BIN_RANGE:
      hue = getColorFromFFTSingleRange(_fft, 3, 20);
      break;
    case FEATURE_FFT:
      hue = (double) map(constrain(_fft->getHighestEnergyIdx(), 7, 255), 7, 255, 0, 1000) / 1000.0;
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
      hue = f->getDominatePeakAvg();
      f->resetDominatePeakAvg();
      break;
    case FEATURE_PEAK:
      hue = f->getDominatePeak();
      break;
    case FEATURE_RMS_AVG:
      hue = f->getDominateRMSAvg();
      break;
    case FEATURE_RMS:
      hue = f->getDominateRMS();
      break;
    case FEATURE_CENTROID:
      // the centroid will be a frequency between about 200 and 50000
      // first I need to move it to a more linear scale
      hue = calculateColorFromCentroid();
      Serial.print("scaled hue:\t\t");
      Serial.println(hue);
      // next I need to scale down to a value between 0.0 and 1.0
      break;
    case FEATURE_FLUX:
      hue = _fft->getFlux();
      break;
    default:
      Serial.println("ERROR - calculateHue() does not accept that HUE_FEATURE");
      break;
  }
  // hue = (hue - hue_min) / hue_max;
  if (hue < hue_min) {
    Serial.print("Warning, hue in calculateHue() is too low (");
    Serial.print(hue);
    Serial.print(") , setting to");
    hue_min = hue_min * (1.0 - feature_reset_factor) + (hue * feature_reset_factor);
    Serial.println(hue_min);    
    hue = hue_min;
  } else if (hue > 1.0) {
    Serial.print("Warning, hue in calculateHue() is too high (");
    Serial.print(hue);
    Serial.println(") , setting to ");
    hue_max = hue_max * (1.0 - feature_reset_factor) + (hue * feature_reset_factor);
    Serial.print(hue_max);
    hue = hue_max;
  }
  last_hue = hue;
  return hue;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Local Brightness Scaler /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/*
  void updateLBS(uint8_t feature) {
  if (lbs_timer > LBS_TIME_FRAME) {
    lbs_timer = 0;
    lbs_min = (uint8_t)((double)lbs_min * 1.1);
    lbs_max = (uint8_t)((double)lbs_max * 0.9);
    Serial.print("Reset the lbs timers");
  }
  if (feature > lbs_max) {
    lbs_max = feature;
  } else if (feature < lbs_min) {
    lbs_min = feature;
  } else {
    return;
  }
  // if we do not return then it means we updated the min or max and now
  // need to update the lbs_scaler_min_thresh and max thresb
  // double range  = lbs_max - lbs_min;
  // double lbs_scaler_min_thresh = lbs_min
  // double lbs_scaler_max_thresh =
  dprint(P_LBS, "old lbs min/max : ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  lbs_scaler_min_thresh = lbs_min + ((lbs_max - lbs_min) * LBS_LOW_TRUNCATE_THRESH);
  lbs_scaler_max_thresh = lbs_max + ((lbs_max - lbs_min) * LBS_HIGH_TRUNCATE_THRESH);
  dprint(P_LBS, "\tnew min/max : ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  }
*/
void updateLBS(double feature) {
  if (lbs_timer > LBS_TIME_FRAME) {
    lbs_timer = 0;
    lbs_min = lbs_min * 1.1;
    lbs_max = lbs_max * 0.9;
    dprint(P_LBS, "Reset the lbs timers and min/max values: ");
    dprint(P_LBS, lbs_min);
    dprint(P_LBS, "\t");
    dprintln(P_LBS, lbs_max);
  }
  if (feature > lbs_max) {
    lbs_max = feature;
    dprint(P_LBS, "\nold lbs_scaler_min/max_thresh : ");
    dprint(P_LBS, lbs_scaler_min_thresh);
    dprint(P_LBS, " / ");
    dprintln(P_LBS, lbs_scaler_max_thresh);
    lbs_scaler_max_thresh = lbs_max - ((lbs_max - lbs_min) * LBS_HIGH_TRUNCATE_THRESH);
  } else if (feature < lbs_min) {
    lbs_min = feature;
    dprint(P_LBS, "\nold lbs_scaler_min/max_thresh : ");
    dprint(P_LBS, lbs_scaler_min_thresh);
    dprint(P_LBS, " / ");
    dprintln(P_LBS, lbs_scaler_max_thresh);
    lbs_scaler_min_thresh = lbs_min + ((lbs_max - lbs_min) * LBS_LOW_TRUNCATE_THRESH);
  } else {
    return;
  }
  // if we do not return then it means we updated the min or max and now
  // need to update the lbs_scaler_min_thresh and max thresb
  dprint(P_LBS, "new lbs_scaler_min/max_thresh : ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
}

uint8_t applyLBS(uint8_t brightness) {
  dprint(P_LBS, "brightness (Before/After) lbs(uint8_t): ");
  dprint(P_LBS, brightness);
  updateLBS(brightness);
  // constrain the brightness to the low and high thresholds
  dprint(P_LBS, " / ");
  brightness = constrain(brightness,  lbs_scaler_min_thresh, lbs_scaler_max_thresh);
  brightness = map(brightness, lbs_scaler_min_thresh, lbs_scaler_max_thresh, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
  // dprint(P_LBS, " = ");
  dprint(P_LBS, brightness);
  dprint(P_LBS, "\tmin/max thresh: ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  return brightness;
}

double applyLBS(double brightness) {
  dprint(P_LBS, "brightness (Before/After) lbs(double):\t");
  dprint(P_LBS, brightness);
  updateLBS(brightness);
  // constrain the brightness to the low and high thresholds
  dprint(P_LBS, " / ");
  brightness = constrainf(brightness,  lbs_scaler_min_thresh, lbs_scaler_max_thresh);
  brightness = mapf(brightness, lbs_scaler_min_thresh, lbs_scaler_max_thresh, 0.0, 1.0);
  // dprint(P_LBS, " = ");
  dprintln(P_LBS, brightness);
  dprint(P_LBS, "\t\t\tmin/max thresh: ");
  dprint(P_LBS, lbs_scaler_min_thresh);
  dprint(P_LBS, " / ");
  dprintln(P_LBS, lbs_scaler_max_thresh);
  return brightness;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
void updateNeosForPitch() {
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    // calculate HSB
    // the brightness should be the loudness (overall amp of bins)
    // the saturation should be the relatitive loudness of the primary bin
    // the hue should be the bin number (With higher frequencies corresponding to reds and yellows)
    double s = calculateSaturation(&fc, &fft_features);
    // user brightness scaler is applied in this function
    double b = calculateHSBBrightness(&fc, &fft_features);
    if (LBS_ACTIVE == true) {
      // WARNING, does not currently work
      b = applyLBS(b); // will scale to maximise the full dynamic range of the feedback
    }
    /////////////////////// Make sure that it is within bounds ////////////////////
    if (b < BRIGHTNESS_CUTTOFF_THRESHOLD) {
      dprintln(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, "brightness too low, changing to 0.0");
      b = 0;
    } else if (b > 1.0) {
      b = 1.0;
      dprintln(P_BRIGHTNESS_SCALER + P_BRIGHTNESS, "brightness too high, changing to 1.0");
    }
    double h = calculateHue(&fc, &fft_features);

    if (SMOOTH_HSB > 0.0) {
      dprint(P_SMOOTH_HSB, "smoothing hsb:\t");
      dprint(P_SMOOTH_HSB, h);
      dprint(P_SMOOTH_HSB, "\t");
      dprint(P_SMOOTH_HSB, s);
      dprint(P_SMOOTH_HSB, "\t");
      dprintln(P_SMOOTH_HSB, b);
      double _h = (h * (1.0 - SMOOTH_HSB)) + (last_hue * (SMOOTH_HSB));
      if (_h >= 0.0 && _h <= 1.0) {
        last_hue = h;
        h = _h;
      }
      double _s = (s * (1.0 - SMOOTH_HSB)) + (last_saturation * (SMOOTH_HSB));
      if (_s >= 0.0 && _s <= 1.0) {
        last_saturation = s;
        s = _s;
      }
      double _b = (b * (1.0 - SMOOTH_HSB)) + (last_brightness * (SMOOTH_HSB));
      if (_b >= 0.0 && _b <= 1.0) {
        last_brightness = b;
        b = _b;
      }
      last_brightness = b;
      dprint(P_SMOOTH_HSB, "\t\t");
      dprint(P_SMOOTH_HSB, h);
      dprint(P_SMOOTH_HSB, "\t");
      dprint(P_SMOOTH_HSB, s);
      dprint(P_SMOOTH_HSB, "\t");
      dprintln(P_SMOOTH_HSB, b);
    }

    if (P_HSB) {
      Serial.print("h: "); Serial.print(h);
      Serial.print("\ts: ");
      Serial.print(s);
      Serial.print("\tb: ");
      Serial.println(b);
    }
    if (fc.isActive() == true) {
      // now colorWipe the LEDs with the HSB value
      // if (h > 0) {
      neos.colorWipeHSB(h, s, b);
      /*} else {
        neos[chan].colorWipeHSB(0, 0, 0);
        }*/
    } else {
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
  if (P_NEO_COLORS) {
    neos.printColors();
  }
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// END of PITCH ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void updateMode() {
  if (FIRMWARE_MODE == CICADA_MODE) {
    updateSong();
    updateOnset();
  }
  else if (FIRMWARE_MODE == PITCH_MODE) {
    updateNeosForPitch();
    printColors();
  }
}


#endif // __MODE_H__
