#ifndef __MODEPITCH_H__
#define __MODEPITCH_H__

#include <Audio.h>
#include <Arduino.h>
#include <PrintUtils.h>
#include "AudioEngine.h"
#include "Neos.h"
#include <WS2812Serial.h>
#include "Configuration.h"

uint8_t red[2];
uint8_t green[2];
uint8_t blue[2];

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

void pitchSetup() {
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
  if (PRINT_EEPROM_CONTENTS  > 0) {
    delay(1000);
    printEEPROMContents();
  } else {
    Serial.println("Not printing the EEPROM Datalog Contents");
  }
  */
  Serial.println("Running Use Specific Setup Loop...");
  pitchSetup();
  /*
  Serial.println("Testing Microphones");
  printTeensyDivide();
  testMicrophones();

  if (data_logging_active) {
    writeSetupConfigsToEEPROM();
  }
  if (LUX_SENSORS_ACTIVE) {
    Serial.println("turning off LEDs for Lux Calibration");
    // todo make this proper
    lux_sensors[0].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS); // todo add this to config_adv? todo
    lux_sensors[1].startSensor(VEML7700_GAIN_1, VEML7700_IT_25MS);
    lux_sensors[0].calibrate(LUX_CALIBRATION_TIME);
    lux_sensors[1].calibrate(LUX_CALIBRATION_TIME);
  }
  */
  printMajorDivide("Setup Loop Finished");
}

bool getColorFromFFT(FeatureCollector *f, uint8_t &red, uint8_t &green, uint8_t &blue) {
  double red_d, green_d, blue_d, tot;
  red_d   = f->getFFTRange(3, 7);
  green_d = f->getFFTRange(7, 20);
  blue_d  = f->getFFTRange(20, 128);
  tot   = red_d + green_d + blue_d;
  red_d   = red_d / tot;
  green_d = green_d / tot;
  blue_d  = blue_d / tot;
  red = (uint8_t)(red_d * (255.0));
  green = (uint8_t)(green_d * 255.0);
  blue = (uint8_t)(blue_d * 255.0);
}

void printColors() {
  Serial.println("printing the colors");
  for (int i = 0; i < 2; i++)  {
    //Serial.print(f->getName());Serial.print("\t");
    Serial.print("red\t"); Serial.print(red[i]);
    Serial.print("\tgreen\t"); Serial.print(green[i]);
    Serial.print("\tblue\t");
    Serial.println(blue[i]);
  }
}

elapsedMillis print_color_timer;

void mothLoop() {
  // update the feature collectors
  for (int i = 0; i < 2; i++) {
    fc[i].update();
    getColorFromFFT(&fc[i], red[i], green[i], blue[i]);
    neos[i].colorWipe(red[i], green[i], blue[i]);
  }
  if (print_color_timer > 200) {
    print_color_timer = 0;
    printColors();
  }
}

void updateJumpers() {
    Serial.println("WARNING -- UPDATE JUMPERS IS UNIMPLEMTED FOR THIS MODE");
}

#endif // mode_pitch_h
