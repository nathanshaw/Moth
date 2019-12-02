#ifndef __MODEPITCH_H__
#define __MODEPITCH_H__

#include <Audio.h>
#include <Arduino.h>
#include <PrintUtils.h>
#include "AudioEngine/AudioEngine.h"
#include "NeopixelManager/NeopixelManager.h"
#include <WS2812Serial.h>
#include "Configuration.h"
#include "ColorConverter.h"
#include "DatalogManager/DatalogManager.h"

uint8_t rgb[2][3];
double hsb[2][3] = {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}};

double brightness_scalers[2];
double global_brightness_scaler = 0.6;

elapsedMillis print_color_timer;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/////////////////////////////// NeoP ////////////////////////////////////

WS2812Serial leds(NUM_LED, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);
NeoGroup neos[2] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 10, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

FeatureCollector fc[2] = {FeatureCollector("front"), FeatureCollector("rear")};

RGBConverter converter;

AudioInputI2S            i2s;           //xy=70,94.00000095367432
AudioAmplifier           input_amp_f;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad_f;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms_f;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect_f;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak_f;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256       fft256_f;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency freq_f;      //xy=584.0000305175781,201.00000476837158
AudioAmplifier           input_amp_r;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad_r;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms_r;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect_r;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak_r;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256       fft256_r;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency freq_r;      //xy=584.0000305175781,201.00000476837158
AudioOutputUSB           usb;           //xy=1307.33353805542,1409.3331747055054

AudioConnection          patchCord1(i2s, 0, input_amp_f, 0);
AudioConnection          patchCord2(input_amp_f, 0, biquad_f, 0);
AudioConnection          patchCord5(biquad_f, 0, tone_detect_f, 0);
AudioConnection          patchCord6(biquad_f, 0, peak_f, 0);
AudioConnection          patchCord7(biquad_f, 0, fft256_f, 0);
AudioConnection          patchCord8(biquad_f, 0, rms_f, 0);
AudioConnection          patchCord9(biquad_f, 0, freq_f, 0);
AudioConnection          patchCord10(biquad_f, 0, usb, 0);

AudioConnection          patchCord11(i2s, 1, input_amp_r, 0);
AudioConnection          patchCord12(input_amp_r, 0, biquad_r, 0);
AudioConnection          patchCord15(biquad_r, 0, tone_detect_r, 0);
AudioConnection          patchCord16(biquad_r, 0, peak_r, 0);
AudioConnection          patchCord17(biquad_r, 0, fft256_r, 0);
AudioConnection          patchCord18(biquad_r, 0, rms_r, 0);
AudioConnection          patchCord19(biquad_r, 0, freq_r, 0);
AudioConnection          patchCord13(biquad_r, 0, usb, 1);

void linkFeatureCollector() {
  //front
  Serial.println("Linking Feature Collector for Front and Rear");
  if (RMS_ACTIVE) {
    fc[0].linkRMS(&rms_f);
    fc[1].linkRMS(&rms_r);
  };
  if (PEAK_ACTIVE) {
    fc[0].linkPeak(&peak_f);
    fc[1].linkPeak(&peak_r);
  };
  if (FFT_ACTIVE) {
    fc[0].linkFFT(&fft256_f);
    fc[1].linkFFT(&fft256_r);
  };
  if (TONE_ACTIVE) {
    fc[0].linkTone(&tone_detect_f);
    fc[1].linkTone(&tone_detect_r);
  };
  if (FREQ_ACTIVE) {
    fc[0].linkFreq(&freq_f);
    fc[1].linkFreq(&freq_r);
  };
}



void mothSetup() {
  AudioMemory(AUDIO_MEMORY);
  Serial.begin(57600);
  delay(5000);
  Serial.println("Setup Loop has started");
  leds.begin();
  Serial.println("LEDS have been initalised");
  delay(250);
  // create either front and back led group, or just one for both
  neos[0].colorWipe(120, 70, 0); // turn off the LEDs
  neos[1].colorWipe(120, 70, 0); // turn off the LEDs
  Serial.println("Leds turned yellow for setup loop\n");
  delay(2000);
  /*
    if (JUMPERS_POPULATED) {
    printMinorDivide();
    Serial.println("Checking Hardware Jumpers");
    readJumpers();
    } else {
    printMajorDivide("Jumpers are not populated, not printing values");
    }
  */
  if (PRINT_EEPROM_CONTENTS  > 0) {
    delay(1000);
    // TODO, add the datalogger and print the log contents
    // printEEPROMContents();
  } else {
    Serial.println("Not printing the EEPROM Datalog Contents");
  }
  Serial.println("Running Use Specific Setup Loop...");
  Serial.println("starting moth setup loop");
  printMinorDivide();

  // left
  input_amp_f.gain(INPUT_START_GAIN);
  biquad_f.setHighpass(0, BQL_THRESH, BQL_Q);
  biquad_f.setHighpass(1, BQL_THRESH, BQL_Q);
  biquad_f.setHighpass(2, BQL_THRESH, BQL_Q);
  biquad_f.setLowShelf(3, BQL_THRESH , -24);
  // right
  input_amp_r.gain(INPUT_START_GAIN);
  biquad_r.setHighpass(0, BQR_THRESH, BQR_Q);
  biquad_r.setHighpass(1, BQR_THRESH, BQR_Q);
  biquad_r.setHighpass(2, BQR_THRESH, BQR_Q);
  biquad_r.setLowShelf(3, BQR_THRESH, -24);

  // audio features
  if (FREQ_ACTIVE) {
    freq_f.begin(FREQ_UNCERTANITY_ALLOWED);
    freq_r.begin(FREQ_UNCERTANITY_ALLOWED);
  }

  // setup the feature collector
  Serial.println("Starting to link Feature Collector");
  linkFeatureCollector();
  printMinorDivide();
  Serial.println("Finished Audio Setup Loop");
  printDivide();

  Serial.println("Testing Microphones");
  printTeensyDivide();
  for (int i = 0; i < num_channels; i++) {
    fc[i].testMicrophone();;
  }
  /*
    if (data_logging_active) {
    writeSetupConfigsToEEPROM();
    }
    if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_managers[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_managers[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    lux_managers[0].calibrate(LUX_CALIBRATION_TIME);
    lux_managers[1].calibrate(LUX_CALIBRATION_TIME);
    }
  */
  printMajorDivide("Setup Loop Finished");
}
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

bool getColorFromTone(FeatureCollector *f, uint8_t chan) {
  Serial.print("WARNING - getColorFromTone is not currently implemented\t");
  Serial.println(f->getToneLevel());
  return true;
}

bool getColorFromFFTAllBins(FeatureCollector *f, uint8_t chan) {
  if (COLOR_MAP_MODE == MODE_RGB) {
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
  else if (COLOR_MAP_MODE == MODE_HSB) {
    double h = (double) map(f->getHighestEnergyBin(), 0, 128, 0, 1000) / 1000.0;
    // dprint(PRINT_FFT_VALS, "FFT - All Bins - HSB - Hue:\t"); dprintln(PRINT_FFT_VALS, h);
    hsb[chan][0] = h;
    return 1;
  } else {
    Serial.println("ERROR - the COLOR_MAP_MODE is not currently implemented");
    return 0;
  }
  return 1;
}


void printColors() {
  for (int i = 0; i < 2; i++)  {
    //Serial.print(f->getName());Serial.print("\t");
    Serial.print(i);
    Serial.print("  red  "); Serial.print(rgb[i][0]);
    Serial.print("\tgreen  "); Serial.print(rgb[i][1]);
    Serial.print("\tblue  ");
    Serial.print(rgb[i][2]);
    Serial.print("  =  "); Serial.print(rgb[i][0] + rgb[i][1] + rgb[i][0]);
    Serial.print("\thue "); Serial.print(hsb[i][0]);
    Serial.print("\tsat "); Serial.print(hsb[i][1]);
    Serial.print("\tbgt "); Serial.println(hsb[i][2]);
  }
}

void calculateBrightness() {
  if (BRIGHTNESS_FEATURE == FEATURE_PEAK) {
    for (int i  = 0;  i < 2; i++) {
      hsb[i][2] = fc[i].getPeakAvg();
      if (hsb[i][2] > 1.0) {
        hsb[i][2] =  1.0;
      }
      fc[i].resetPeakAvgLog();
    }
  } else {
    Serial.print("ERROR - calculateBrightness does not accept that  BRIGHTNESS_FEATURE");
  }
}

void calculateSaturation() {
  if (SATURATION_FEATURE == FEATURE_PEAK) {
    for (int i = 0;  i < 2; i++) {
      hsb[i][1] = fc[i].getPeakAvg();
      if (hsb[i][1] > 1.0) {
        hsb[i][1] =  1.0;
      }
      // Serial.print("sat set to  : ");Serial.println(hsb[i][1]);
      fc[i].resetPeakAvgLog();
    }
  } else {
    Serial.print("ERROR - calculateBrightness does not accept that  BRIGHTNESS_FEATURE");
  }
}

void calculateHue() {
  for (int i = 0; i <  2; i++) {
    if (HUE_FEATURE == FEATURE_FFT) {
      if (FFT_MODE == MODE_SINGLE_RANGE) {
        getColorFromFFTSingleRange(&fc[i], 3, 20);
      } else if (FFT_MODE == MODE_ALL_BINS) {
        getColorFromFFTAllBins(&fc[i], i);
      }
    }
    else if (HUE_FEATURE == FEATURE_TONE)
      getColorFromTone(&fc[i], i);
  }
}

void updateNeos() {
  if (COLOR_MAP_MODE == MODE_HSB) {
    calculateSaturation();
    calculateBrightness();
    calculateHue();
    uint8_t inactive = 0;
    for (int chan = 0; chan < num_channels; chan++) {
      if (fc[chan].isActive() == true) {
        uint8_t r, g, b;
        r = rgb[chan][0];
        g = rgb[chan][1];
        b = rgb[chan][2];
        converter.HsvToRgb(hsb[chan][0], hsb[chan][1], hsb[chan][2], r, g, b);
      } else {
        inactive++;
      };
    }
    if (inactive > num_channels) {
      Serial.println("ERROR - not able to updateNeos() as there is no active audio channels");
      return;
    } else if (inactive > 0) {
      if (fc[0].isActive() == false) {
        converter.HsvToRgb(hsb[1][0], hsb[1][1], hsb[1][2], rgb[0][0], rgb[0][1], rgb[0][2]);
      } else if (fc[1].isActive() == false) {
        converter.HsvToRgb(hsb[0][0], hsb[0][1], hsb[0][2], rgb[1][0], rgb[1][1], rgb[1][2]);
      }
    }
  }
  else {
    Serial.println("ERROR = that color mode is not implemented in update neos");
  }

  // now actually update the Neopixels
  for (int i = 0; i < 2; i++) {
    neos[i].colorWipe(rgb[i][0], rgb[i][1], rgb[i][2]);
  }
}

void mothLoop() {
  // update the feature collectors
  for (int i = 0; i < 2; i++) {
    fc[i].update();
    updateNeos();
  }
  if (print_color_timer > 2000) {
    print_color_timer = 0;
    printColors();
  }
}

void updateJumpers() {
  Serial.println("WARNING -- UPDATE JUMPERS IS UNIMPLEMTED FOR THIS MODE");
}

#endif // mode_pitch_h
