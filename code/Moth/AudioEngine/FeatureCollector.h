#ifndef __FEATURE_COLLECTOR_H__
#define __FEATURE_COLLECTOR_H__

class FeatureCollector {
  public:
    FeatureCollector(String _id);
    double getPosDelta(double, double);
    bool testMicrophone();
    void printFeatures();
    String getName() {
      return id;
    };
    //////////////// Gain Tracking /////////////////////////
    double gain = 1.0;
    double min_gain = 1.0;
    double max_gain = 1.0;
    void updateGain(double g);

    void linkAmplifier(AudioAmplifier * amp) { 
        if (audio_amp_add_idx < 4) {
            Serial.print("Linked an audio amplifier ");Serial.print(audio_amp_add_idx);printTab();
            amp_ana[audio_amp_add_idx] = amp;
            gain_tracking_active = true;
            audio_amp_add_idx = audio_amp_add_idx + 1;
            Serial.println(audio_amp_add_idx);
        }
        else {
                Serial.println("ERROR, can't link audio amplifier, there are not enough available slots");
        }
    }

    //////////////// RMS /////////////////////////
    void linkRMS(AudioAnalyzeRMS *r) {
      rms_ana = r;
      rms_active = true;
    };
    double getRMS();
    double getRMSPosDelta();
    void   printRMSVals();

    //////////////// Peak /////////////////////////
    void linkPeak(AudioAnalyzePeak *r) {
      peak_ana = r;
      peak_active = true;
    };
    double getPeak();
    double getPeakPosDelta() {return peak_pos_delta;};
    double getPeakAvg();
    void   resetPeakAvgLog();
    void   printPeakVals();
    double peak_pos_delta;

    //////////////// Tone /////////////////////////
    void linkTone(AudioAnalyzeToneDetect *r) {
      tone_ana = r;
      tone_active = true;
    };
    double getToneLevel();
    void   printToneVals();

    //////////////// Freq /////////////////////////
    void linkFreq(AudioAnalyzeNoteFrequency *r) {
      freq_ana = r;
      freq_active = true;
    };
    double getFreq();
    void printFreqVals();

    //////////////// FFT /////////////////////////
    void linkFFT(AudioAnalyzeFFT256 *r) {
      fft_ana = r;
      fft_active = true;
    };
    void printFFTVals();
    double getFFTRange(uint8_t s, uint8_t e);
    int getHighestEnergyBin() {
      return highest_energy_idx;
    };

    //////////////// General ///////////////////////
    void update();
    bool isActive() {return microphone_active;};

  private:
    String id = "";
    bool microphone_active = true;

    //////////////// Gain Tracking ///////////////
    AudioAmplifier *amp_ana[4];
    uint8_t audio_amp_add_idx = 0;
    bool gain_tracking_active = false;
    // TODO, make it so linking of gains and tracking is all dones through fc
    // AudioAmplifier *gain_ana[4];

    //////////////// RMS /////////////////////////
    AudioAnalyzeRMS *rms_ana;
    bool rms_active = false;
    void calculateRMS();
    double rms_val;
    double rms_pos_delta;

    //////////////// Peak /////////////////////////
    AudioAnalyzePeak *peak_ana;
    bool peak_active = false;
    void calculatePeak();
    double peak_val;
    double peak_totals = 0;
    unsigned long peak_readings = 0;
    elapsedMillis last_peak_reset;

    //////////////// Tone /////////////////////////
    AudioAnalyzeToneDetect *tone_ana;
    bool tone_active = false;
    void calculateTone();
    double tone_level = 0;

    //////////////// Freq /////////////////////////
    AudioAnalyzeNoteFrequency *freq_ana;
    bool freq_active = false;
    double freq_val;
    double freq_prob;
    void calculateFreq();

    //////////////// FFT /////////////////////////
    AudioAnalyzeFFT256 *fft_ana;
    bool fft_active = false;
    double fft_vals[128];
    void calculateFFT();
    void calculateScaledFFT();
    double fft_tot_energy;
    int highest_energy_idx;
};

FeatureCollector::FeatureCollector(String _id) {
  id = _id;
}

void FeatureCollector::updateGain(double g) {
        gain = g;
        if (gain > max_gain) {
            max_gain = gain;
        }
        if (gain < min_gain) {
            min_gain = gain;
        }
        for (int i =  0; i < audio_amp_add_idx; i++) {
            amp_ana[i]->gain(g);
    }
}

bool FeatureCollector::testMicrophone () {
  // go through and gather 10 features from each channel and make sure it is picking up audio
  if (!rms_active) {
      Serial.println("Sorry unable to test microphone as the RMS feature is not active");
      return false;
  }
  uint8_t readings = 0;
  double values = 0.0;
  unsigned long a_time = millis();
  Serial.print("Testing ");Serial.print(id);Serial.println(" Microphone");
  while (readings < 10 && millis() < a_time + 2000) {
    if (rms_ana->available()) {
      values += rms_ana->read();
      readings++;
      Serial.print(".");
      delay(20);
    }
  }
  if (values > 0) {
    Serial.println();
    Serial.print(id);
    Serial.println(" Microphone is good");
    microphone_active = true;
    return true;
  } else {
    Serial.println("\nERROR, ");
    Serial.print(id);Serial.println(" Microphone does not work");
    printDivideLn();
    microphone_active = false;
    return false;
  }
}

//////////////// Update Functions ///////////////////////////////
void FeatureCollector::calculateFFT() {
  if (fft_active && fft_ana->available()) {
    Serial.println("FFT Available");
    fft_tot_energy = 0;
    int highest = -1;
    for (int i = 0; i < 128; i++) {
      fft_vals[i] = fft_ana->read(i);
      fft_tot_energy += fft_vals[i];
    }
    highest_energy_idx = highest;
    printFFTVals();
  }
}

void FeatureCollector::calculateScaledFFT() {
  if (fft_active && fft_ana->available()) {
    // Serial.println("FFT Available");
    fft_tot_energy = 0;
    int highest = -1;
    double highest_val = -1.0;
    for (int i = 0; i < 128; i++) {
      fft_vals[i] = fft_ana->read(i);
      fft_tot_energy += fft_vals[i];
      if (fft_vals[i] > highest_val) {
        highest_val = fft_vals[i];
        highest = i;
      }
    }
    for (int i = 0; i < 128; i++) {
      fft_vals[i] = fft_vals[i] / fft_tot_energy;
    }
    highest_energy_idx = highest;
    printFFTVals();
  }
}

void FeatureCollector::calculateTone() {
  if (tone_active && tone_ana->available()) {
    tone_level = tone_ana->read();
    printToneVals();
  }
}

void FeatureCollector::calculateFreq() {
  if (freq_active  && freq_ana->available()) {
    freq_val = freq_ana->read();
    freq_prob = freq_ana->probability();
    printFreqVals();
  }
}

void FeatureCollector::calculatePeak() {
  if (peak_active && peak_ana->available()) {
    double last = peak_val;
    peak_val =  peak_ana->read() * PEAK_SCALER;
    peak_pos_delta = getPosDelta(last, peak_val);
    peak_totals += peak_val;
    peak_readings++;
  }
}

void FeatureCollector::resetPeakAvgLog() {
  if (last_peak_reset > PEAK_LOG_RESET_MIN) {
    peak_totals = 0.0;
    peak_readings = 0;
    last_peak_reset = 0;
  }
}

double FeatureCollector::getPosDelta(double last, double current) {
    double delta = 0;
    if (current > last) {
        delta = current - last;
    } 
    return delta;
}

void FeatureCollector::calculateRMS() {
  if (rms_active  && (rms_ana->available())) {
    double temp = rms_val;
    rms_val = rms_ana->read();
    rms_pos_delta = getPosDelta(temp, rms_val);
  }
}

///////////////////// Getter functions ///////////////////////////////
double FeatureCollector::getFFTRange(uint8_t s, uint8_t e) {
  if (fft_active) {
    return fft_ana->read(s, e);
  }
  return -1.0;
}

double FeatureCollector::getRMS() {
  if (rms_active) {
    return rms_val;
  }
  Serial.println("ERROR  - RMS IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1.0;
}

double FeatureCollector::getPeak() {
  if (peak_active) {
    return peak_val;
  }
  Serial.println("ERROR  - Peak IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1.0;

}

double FeatureCollector::getPeakAvg() {
  if (peak_readings > 0 && peak_totals > 0) {
    return ((double)peak_totals / (double)peak_readings);
  }
  return peak_val;
}

double FeatureCollector::getFreq() {
  if (freq_active) {
    return freq_val;
  }
  Serial.println("ERROR  - Freq IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1.0;

}

double FeatureCollector::getToneLevel() {
  if (tone_active) {
    return tone_level;
  }
  Serial.println("ERROR  - Peak IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
  return -1;

}

//////////////////////////////// Print Functions /////////////////////////////////////////
void FeatureCollector::printFeatures() {
  // printMajorDivide((String)(id + " Features "));
  if (rms_active && PRINT_RMS_VALS) {
    printRMSVals();
  };
  if (peak_active && PRINT_PEAK_VALS) {
    printPeakVals();
  };
  if (freq_active && PRINT_FREQ_VALS) {
    printFreqVals();
  };
  if (tone_active && PRINT_TONE_VALS) {
    printToneVals();
  };
  if (fft_active && PRINT_FFT_VALS) {
    printFFTVals();
  };
  // printDivide();
}

void FeatureCollector::printToneVals() {
  if (tone_active > 0) {
    Serial.print(id);
    Serial.print(" tone_level :\t");
    Serial.println(tone_level);
  }
}

void FeatureCollector::printFFTVals() {
  if (fft_active) {
    Serial.print(id); Serial.println(" FFT vals");
    uint8_t w = 8;
    for (int l  = 0; l < w; l++) {
      Serial.println();
      Serial.print(l); Serial.print("\t");
      for (int i = l; i < 128; i = i + w) {
        if (i != l) {
          Serial.print(", ");
        };
        Serial.print(fft_vals[i]);
      }
    }
    Serial.println();
    Serial.print("Bin with highest energy: "); Serial.println(highest_energy_idx);
  }
}

void FeatureCollector::printFreqVals() {
  if (freq_active > 0) {
    Serial.print(id); Serial.print("\tnote_freq prob/freq:\t");
    Serial.print(freq_prob); printTab();
    Serial.println(freq_val);
  }
}

void FeatureCollector::printRMSVals() {
  if (rms_active > 0) {
    Serial.print(id); Serial.print(" RMS vals\t");
    Serial.print(rms_val);printTab();
    Serial.print("delta\t");Serial.println(rms_pos_delta);
  }
}


void FeatureCollector::printPeakVals() {
  if (peak_active > 0) {
    Serial.print(id); Serial.print(" Peak vals\t");
    Serial.print(peak_val);printTab();
    Serial.print("delta\t");Serial.println(peak_pos_delta);
  }
}

/////////////////////////////////// UPDATE / INIT //////////////////////////////////////
void FeatureCollector::update() {
    if (microphone_active == true) {
      if (USE_SCALED_FFT) {
        calculateScaledFFT();
      }
      else {
        calculateFFT();
      }
      calculateTone();
      calculatePeak();
      calculateRMS();
      calculateFreq();
      // printFeatures();
    }
    else { 
      //Serial.print(id);Serial.println(" Sorry the microphone does not work, not updating the feature collector");
    }
}
#endif
