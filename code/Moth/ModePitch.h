#ifndef __MODEPITCH_H__
#define __MODEPITCH_H__

#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include "Configuration_pitch.h"
#include "NeopixelManager/NeopixelManager.h"
#include "LuxManager/LuxManager.h"
#include "AudioEngine/AudioEngine.h"
#include <Audio.h>
#include <SerialFlash.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define COLOR_PRINT_RATE 1000

double brightness_scalers[2];
double global_brightness_scaler = 0.6;

elapsedMillis print_color_timer;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/////////////////////////////// NeoP ////////////////////////////////////

WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);

NeoGroup neos[2] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 9, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_SENSORS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

FeatureCollector fc = FeatureCollector("All");

// GUItool: begin automatically generated code
AudioInputI2S            i2s;            //xy=77,123
AudioMixer4              mixer;          //xy=205,137
AudioFilterBiquad        biquad;         //xy=330.00000381469727,138.00000190734863
AudioAmplifier           input_amp;      //xy=459.00000381469727,137.00001525878906
AudioAnalyzePeak         peak;           //xy=650.0000076293945,117.00000190734863
AudioAnalyzeRMS          rms;            //xy=650.0000076293945,151.00000190734863
AudioOutputUSB           usb;            //xy=651.000057220459,184.00000381469727
AudioAnalyzeFFT256       fft256;         //xy=652.0000152587891,215.00000190734863
AudioConnection          patchCord1(i2s, 0, mixer, 0);
AudioConnection          patchCord2(i2s, 1, mixer, 1);
AudioConnection          patchCord3(mixer, biquad);
AudioConnection          patchCord4(biquad, input_amp);
AudioConnection          patchCord5(input_amp, peak);
AudioConnection          patchCord6(input_amp, rms);
AudioConnection          patchCord7(input_amp, 0, usb, 0);
AudioConnection          patchCord8(input_amp, 0, usb, 1);
AudioConnection          patchCord9(input_amp, fft256);
// GUItool: end automatically generated code




void setupDLManager() {
  Serial.println("WARNING - DLManager is not properly coded");
}

void linkFeatureCollector() {
  //front
  Serial.println("Linking Feature Collector for Front and Rear");
  if (RMS_FEATURE_ACTIVE) {
    fc.linkRMS(&rms);
  };
  if (PEAK_FEATURE_ACTIVE) {
    fc.linkPeak(&peak);
  };
  if (FFT_FEATURE_ACTIVE) {
    fc.linkFFT(&fft256, FFT_LOWEST_BIN, FFT_HIGHEST_BIN);
  };
  /*
  if (TONE_FEATURE_ACTIVE) {
    fc.linkTone(&tone_detect);
  };
  if (FREQ_FEATURE_ACTIVE) {
    fc.linkFreq(&freq);
  };
  */
}

void audioSetup() {
  // left
  AudioMemory(AUDIO_MEMORY);
  mixer.gain(0, 0.5);
  mixer.gain(1, 0.5);
  mixer.gain(2, 0.0);
  mixer.gain(3, 0.0);
  input_amp.gain(INPUT_START_GAIN);
  biquad.setHighpass(0, BQ_THRESH, BQ_Q);
  biquad.setHighpass(1, BQ_THRESH, BQ_Q);
  biquad.setHighpass(2, BQ_THRESH + 50, BQ_Q);
  biquad.setHighpass(3, BQ_THRESH - 50, BQ_Q);
  // biquad.setLowShelf(3, BQ_THRESH, BQ_SHELF);
  
  // setup the feature collector
  Serial.println("Starting to link Feature Collector");
  linkFeatureCollector();
  printMinorDivide();
  Serial.println("Finished Audio Setup Loop");
  printDivide();
}
/*
void mainSetup() {

  Serial.println("Testing Microphones");
  printTeensyDivide();
  fc.testMicrophone();
  
  if (data_logging_active) {
    Serial.println("WARNING - DATALOGGING IS NOT CURRENTLY IMPLEMENTED FOR THIS MODE");
  }
  if (LUX_SENSORS_ACTIVE) {
    neos[0].colorWipe(0, 0, 0);
    neos[1].colorWipe(0, 0, 0);
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
  }
  printMajorDivide("Setup Loop Finished");
}
*/
/*
  void setRGBfromHSB(double h, double s, double b) {
  converter.HsvToRgb(h, s, b, red[0], green[0], blue[0]);
  printColors();
  for (int i; i < 2; i++) {
    if (HUE_FEATURE == FEATURE_FFT) {
        hsb[i][0] =
    } else {
      Serial.print("Sorry unable to update RGB from HSB due to HUE_FEATURE not being implemented");
    }
    if (BRIGHTNESS_FEATURE == FEATURE_PEAK) {
      // map the peak levels to the brightness
    } else {
      Serial.print("Sorry unable to update RGB from HSB due to HUE_FEATURE not being implemented");
    }
    if (SATURATION_FEATURE == FEATURE_FFT) {

    } else {
      Serial.print("Sorry unable to update RGB from HSB due to SATURATION_FEATURE not being implemented");
    }
  }
  }*/

bool getColorFromFFTSingleRange(FeatureCollector *f, uint8_t s, uint8_t e) {
  double tot, frac;
  frac = f->getFFTRange(s, e);
  tot = f->getFFTRange(FFT_LOWEST_BIN, 128);
  frac = frac / tot;
  // RGBConverter::HsvToRgb(frac, 0.5, 1, 0, red, green, blue);
  return 1;
}

bool getHueFromTone(FeatureCollector *f) {
  Serial.print("WARNING - getColorFromTone is not currently implemented\t");
  Serial.println(f->getToneLevel());
  return true;
}


double getHueFromFFTAllBins(FeatureCollector *f) {
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
    return (double) map(f->getHighestEnergyBin(), 0, 128, 0, 1000) / 1000.0;
    // dprint(PRINT_FFT_VALS, "FFT - All Bins - HSB - Hue:\t"); dprintln(PRINT_FFT_VALS, h);
  } else {
    Serial.println("ERROR - the COLOR_MAP_MODE is not currently implemented");
    return 0.0;
  }
}

double calculateBrightness(FeatureCollector *f) {
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
  else if (BRIGHTNESS_FEATURE == FEATURE_FFT_ENERGY){
    b = f->getFFTTotalEnergy();
  } else {
    Serial.println("ERROR - calculateBrightness() does not accept that  BRIGHTNESS_FEATURE");
  }
  return b;
}

double calculateSaturation(FeatureCollector *f) {
  double sat = 0.0;
  if (SATURATION_FEATURE == FEATURE_PEAK_AVG) {
    sat = f->getPeakAvg();
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
    sat = f->getRelativeEnergy(f->getHighestEnergyBin()) * 10.0;
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

double calculateHue(FeatureCollector *f) {
  double hue = 0.0;
  switch (HUE_FEATURE) {
  case FEATURE_FFT_BIN_RANGE:
      hue = getColorFromFFTSingleRange(f, 3, 20);
      break;
  case FEATURE_FFT:
      hue = getHueFromFFTAllBins(f);
      break;
  case FEATURE_FFT_MAX_BIN:
      // calculate the bin with the most energy,
      // Serial.print("Highest energy bin is: ");Serial.println(f->getHighestEnergyBin(FFT_LOWEST_BIN, FFT_HIGHEST_BIN));
      // map the bin  index to a hue value
      hue = (double) (f->getHighestEnergyBin(FFT_LOWEST_BIN, FFT_HIGHEST_BIN) - FFT_LOWEST_BIN) / FFT_HIGHEST_BIN;
      // Serial.print("max bin hue is : ");Serial.println(hue); 
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

void updateNeos() {
  if (COLOR_MAP_MODE == COLOR_MAPPING_HSB) {
    uint8_t inactive = 0;
          // calculate HSB
      // the brightness should be the loudness (overall amp of bins)
      // the saturation should be the relatitive loudness of the primary bin
      // the hue should be the bin number (With higher frequencies corresponding to reds and yellows)
      double s = calculateSaturation(&fc);
      double b = calculateBrightness(&fc);
      double h = calculateHue(&fc);
    for (int chan = 0; chan < num_channels; chan++) {
      if (fc.isActive() == true) {
        // now colorWipe the LEDs with the HSB value
        neos[chan].colorWipeHSB(h, s, b);
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

void updateFeatureCollectors() {
  // update the feature collectors
  #if NUM_FEATURE_COLLECTORS == 1
    fc.update();
  #endif
  #if NUM_FEATURE_COLLECTORS > 1
  for (int i = 0; i < NUM_FEATURE_COLLECTORS; i++) {
    fc[i].update();
  }
  #endif
}

void printColors() {
  if (print_color_timer > COLOR_PRINT_RATE) {
    fc.printFFTVals();
    print_color_timer = 0;
    for (int i = 0; i < NUM_NEO_GROUPS; i++)  {
      neos[i].printColors();
    }
  }
}

void updateLuxManagers() {
  // update the feature collectors
  for (int i = 0; i < NUM_LUX_MANAGERS; i++) {
    lux_managers[i].update();
  }
}

void mainLoop() {
  updateLuxManagers();
  updateFeatureCollectors();
  updateNeos();
  printColors();
  // datalog_manager.update();
}

void updateJumpers() {
  Serial.println("WARNING -- UPDATE JUMPERS IS UNIMPLEMTED FOR THIS MODE");
}
#endif // mode_pitch_h
