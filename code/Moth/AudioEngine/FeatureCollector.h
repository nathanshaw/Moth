#ifndef __FEATURE_COLLECTOR_H__
#define __FEATURE_COLLECTOR_H__

class FeatureCollector {
    public:
        FeatureCollector(String _id);
        double getPosDelta(double, double);
        /////////////////// Microphone Testing /////////////////
        bool testMicrophone();
        bool testMicrophoneRMS();
        bool testMicrophonePeak();

        /////////////////// Printing ///////////////////////////
        void printFeatures();
        String getName() {
            return id;
        };

        //////////////// Gain Tracking /////////////////////////
        double gain = 1.0;
        double min_gain = 1.0;
        double max_gain = 1.0;
        void updateGain(double g);
        bool ampActive() {return amp_active;};

        void linkAmplifier(AudioAmplifier * amp, double low, double high) { 
            gain_lower_limit = low;
            gain_upper_limit = high;
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
        void linkRMS(AudioAnalyzeRMS *r, double s) {
            rms_ana = r;
            rms_active = true;
            rms_scaler = s;
        };

        double getRMS();
        double getRMSPosDelta(){return rms_pos_delta;};
        double getRMSAvg();
        void   resetRMSAvgLog();
        void   resetRmsAvgLog();
        void   printRMSVals();

        //////////////// Peak /////////////////////////
        void linkPeak(AudioAnalyzePeak *r, double s) {
            peak_ana = r;
            peak_active = true;
            peak_scaler = s;
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
        void   printFreqVals();

        //////////////// FFT /////////////////////////
        void linkFFT(AudioAnalyzeFFT256*r, uint16_t l, uint16_t h, double s, bool scale_bin_v, bool c, bool f) {
            fft_ana = r;
            fft_active = true;
            max_bin = h;
            min_bin = l;
            fft_num_bins = h - l;
            fft_scaler = s;
            scale_bin_values = scale_bin_v;
            calculate_centroid = c;
            calculate_flux = f;
        };
        // will the magnitudes within min-bin and max_bin sum to 1.0?
        bool scale_bin_values;

        void   printFFTVals();
        double getFFTRange(uint16_t s, uint16_t e);
        int    getHighestEnergyBin() {
            return highest_energy_idx;
        };
        int    getHighestEnergyBin(int start, int end);
        double getRelativeEnergy(uint16_t);
        double getFFTTotalEnergy();
        double getRelativeBinPos() {return relative_bin_pos;};
        double getCentroid();
        double getSpectralFlux();

        //////////////// General ///////////////////////
        void update();
        bool isActive() {return microphone_active;};
        uint32_t getBinsMidFreq256(int bin);

    private:
        String id = "";
        bool microphone_active = true;
        elapsedMillis last_update_timer;

        //////////////// Gain Tracking ///////////////
        AudioAmplifier *amp_ana[4];
        bool amp_active = false;
        uint8_t audio_amp_add_idx = 0;
        bool gain_tracking_active = false;
        double gain_lower_limit;
        double gain_upper_limit;
        // TODO, make it so linking of gains and tracking is all dones through fc
        // AudioAmplifier *gain_ana[4];

        //////////////// RMS /////////////////////////
        AudioAnalyzeRMS *rms_ana;
        bool rms_active = false;
        void calculateRMS();
        double rms_val;
        double rms_totals;
        unsigned long rms_readings;
        double rms_scaler = 1.0;
        double rms_pos_delta;
        elapsedMillis last_rms_reset;

        //////////////// Peak /////////////////////////
        AudioAnalyzePeak *peak_ana;
        bool peak_active = false;
        void calculatePeak();
        double peak_val;
        double peak_scaler = 1.0;
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
        AudioAnalyzeFFT256*fft_ana;
        uint16_t fft_num_bins = 0;
        bool fft_active = false;
        double fft_vals[127]; // 511 is the largest possible size todo need to figure out how to make this array dynamically sizable
        double last_fft_vals[127]; // 511 is the largest possible size todo need to figure out how to make this array dynamically sizable
        double scaled_fft_vals[127]; // 511 is the largest possible size todo need to figure out how to make this array dynamically sizable
        double last_scaled_fft_vals[127]; // 511 is the largest possible size todo need to figure out how to make this array dynamically sizable
        double fft_scaler = 1.0;
        void calculateFFT();
        double fft_tot_energy;
        int highest_energy_idx;
        double relative_bin_pos = 0.0;
        uint16_t max_bin;// what is the highest index bin that we care about?
        uint16_t min_bin;// what is the lowest index bin that we care about?
        bool calculate_centroid = false;
        void setCalculateCentroid(bool v) {calculate_centroid = v;};
        void setCalculateFlux(bool v) {calculate_flux= v;};
        bool calculate_flux = false;
        double centroid;
        double flux;
};

FeatureCollector::FeatureCollector(String _id) {
    id = _id;
}
int FeatureCollector::getHighestEnergyBin(int start, int end) {
    int highest = -1;
    double h_val = 0.0;
    for (int i = start; i <= end ; i++) {
        if (fft_vals[i] > h_val){
            highest = i;
            h_val = fft_vals[i];
        }
    }
    return highest;
}

void FeatureCollector::updateGain(double g) {
    gain = g;
    if (gain > max_gain) {
        max_gain = gain;
    }
    if (gain < min_gain) {
        min_gain = gain;
    }
    if (gain > gain_upper_limit) {
        gain = gain_upper_limit;
    }
    else if (gain < gain_lower_limit) {
        gain = gain_lower_limit;
    }
    for (int i =  0; i < audio_amp_add_idx; i++) {
        amp_ana[i]->gain(g);
    }
}

bool FeatureCollector::testMicrophoneRMS() {
    // go through and gather 10 features from each channel and make sure it is picking up audio
    uint8_t readings = 0;
    double values = 0.0;
    unsigned long a_time = millis();
    Serial.print("Testing ");Serial.print(id);Serial.println(" Microphone using RMS");
    while (readings < 10 && millis() < a_time + MICROPHONE_TEST_DURATION) {
        if (rms_ana->available()) {
            values += rms_ana->read();
            readings++;
            Serial.print(". ");
            delay(20);
        }
    }
    if (values > 0) {
        Serial.println();
        Serial.print(id);
        Serial.println(" Microphone is good");
        microphone_active = true;
        return true;
    } 
    Serial.println("\nERROR, ");
    Serial.print(id);Serial.println(" Microphone does not work");
    printDivideLn();
    microphone_active = false;
    return false;
}

bool FeatureCollector::testMicrophonePeak() {
    // go through and gather 10 features from each channel and make sure it is picking up audio
    uint8_t readings = 0;
    double values = 0.0;
    unsigned long a_time = millis();
    printDivide();
    Serial.print("Testing ");Serial.print(id);Serial.println(" Microphone using Peak");
    while (readings < 10 && millis() < a_time + MICROPHONE_TEST_DURATION) {
        if (peak_ana->available()) {
            values += peak_ana->read();
            readings++;
            Serial.print(". ");
            delay(20);
        }
    }
    if (values > 0) {
        Serial.println();
        Serial.print(id);
        Serial.println(" Microphone is good");
        microphone_active = true;
        return true;
    } 
    Serial.println("\nERROR, ");
    Serial.print(id);Serial.println(" Microphone does not work");
    printMinorDivideLn();
    microphone_active = false;
    return false;
}

bool FeatureCollector::testMicrophone () {
    if (rms_active) {
        return testMicrophoneRMS();
    } else if (peak_active) {
        return testMicrophonePeak();
    } else {
        Serial.println("Sorry unable to test microphone as neither the RMS or Peak feature is active");
        return false;
    }
}


//////////////// Update Functions ///////////////////////////////

double FeatureCollector::getSpectralFlux() {
    double f = 0.0;
    for (int i = 0; i < 127; i++) {
        f += pow((scaled_fft_vals[i] - last_scaled_fft_vals[i]), 2);
    }
    return f;
}

double FeatureCollector::getCentroid() {
    double cent = 0.0;
    double mags = 0.0;
    for (int i = min_bin; i < max_bin; i++) {
        // take the magnitude of all the bins
        // and multiply if by the mid frequency of the bin
        // then all it to the total cent value
        mags += fft_vals[i] * getBinsMidFreq256(i);
    }
    return mags;
}

void FeatureCollector::calculateFFT() {
    if (fft_active && fft_ana->available()) {
        dprintln(PRINT_FFT_DEBUG, "FFT Available");
        fft_tot_energy = 0.0;
        int highest = -1;
        double highest_val = -1.0;
        for (int i = 0; i < 127; i++) {
            last_fft_vals[i] = fft_vals[i];
            fft_vals[i] = fft_ana->read(i) * fft_scaler;
            fft_tot_energy += fft_vals[i];
            if (fft_vals[i] >= highest_val) {
                highest_val = fft_vals[i];
                highest = i;
            }
        }
        if (scale_bin_values == true) {
            for (int i = min_bin; i < max_bin; i++) {
                dprintln(PRINT_FFT_DEBUG, "Scaling FFT Magnitudes to the sum of 1.0 over bin range");
                if (fft_tot_energy > 0) {
                    last_scaled_fft_vals[i] = scaled_fft_vals[i];
                    scaled_fft_vals[i] = fft_vals[i] / fft_tot_energy;
                }
            }
        }
        if (highest != -1) {
            highest_energy_idx = highest;
        }
        if (PRINT_FFT_DEBUG) {
          printFFTVals();
        }
        relative_bin_pos = (double)(highest_energy_idx - min_bin) / (double)(max_bin - min_bin);
        if (calculate_centroid == true) {centroid = getCentroid();};
        if (calculate_flux == true) {flux = getSpectralFlux();};
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
    bool avail = peak_ana->available();
    if (peak_active && avail) {
        double last = peak_val;
        peak_val =  peak_ana->read();
        dprint(PRINT_PEAK_DEBUG, id);
        dprint(PRINT_PEAK_DEBUG, " Peaks (normal, scaled, pos_delta):\t");
        dprint(PRINT_PEAK_DEBUG, peak_val);
        peak_val *= peak_scaler;
        dprint(PRINT_PEAK_DEBUG, "\t");
        dprint(PRINT_PEAK_DEBUG, peak_val);
        peak_pos_delta = getPosDelta(last, peak_val);
        dprint(PRINT_PEAK_DEBUG, "\t");
        dprintln(PRINT_PEAK_DEBUG, peak_pos_delta);
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

void FeatureCollector::calculateRMS() {
    if (rms_active  && (rms_ana->available())) {
        double temp = rms_val;
        rms_val = rms_ana->read();
        rms_val *= rms_scaler;
        rms_pos_delta = getPosDelta(temp, rms_val);
        rms_totals += rms_val;
        rms_readings++;
    }
}

void FeatureCollector::resetRMSAvgLog() {
    if (last_rms_reset > RMS_LOG_RESET_MIN) {
        rms_totals = 0.0;
        rms_readings = 0;
        last_rms_reset = 0;
    }
}

double FeatureCollector::getPosDelta(double last, double current) {
    double delta = 0;
    if (current > last) {
        delta = current - last;
    }
    return delta;
}

///////////////////// Getter functions ///////////////////////////////
double FeatureCollector::getFFTRange(uint16_t s, uint16_t e) {
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

double FeatureCollector::getRMSAvg() {
    if (rms_readings > 0 && rms_totals > 0) {
        return ((double)rms_totals / (double)rms_readings);
    }
    return rms_val;
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

double FeatureCollector::getFFTTotalEnergy() {
    if (fft_active) {
        return fft_tot_energy;
    }
    Serial.println("ERROR  - FFT IS NOT AN ACTIVE AUDIO FEATURE : "); Serial.println(id);
    return -1.0;
}

//////////////////////////////// Print Functions /////////////////////////////////////////
void FeatureCollector::printFeatures() {
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
}

void FeatureCollector::printToneVals() {
    if (tone_active > 0) {
        Serial.print(id);
        Serial.print(" tone_level :\t");
        Serial.println(tone_level);
    }
}

double FeatureCollector::getRelativeEnergy(uint16_t idx) {
    if (fft_tot_energy > 0) {
        double val = 0.0;
        /*
        Serial.print(fft_vals[idx]);
        Serial.print(" / ");
        Serial.print(fft_tot_energy);
        Serial.print("\t");
        */
        val = fft_vals[idx] / fft_tot_energy;
        // Serial.println(val);
        return val;
    }
    return 0.0;
}

void printFreqRangeOfBin(int idx, int max_idx) {
    Serial.print(idx * 172);
    Serial.print(" - ");
    Serial.println((idx + 1) * 172);
}

uint32_t FeatureCollector::getBinsMidFreq256(int bin) {
    return (uint32_t)(bin * 172 + 86);
}

void FeatureCollector::printFFTVals() {
    if (fft_active) {
        /*
        if (USE_SCALED_FFT) {
            Serial.print("Scaled ");
        }
        Serial.print(id); Serial.print(" FFT vals");
        uint8_t w = 8;
        for (int l  = 0; l < w; l++) {
            Serial.println();
            Serial.print(l+min_bin); Serial.print("\t");
            for (int i = l + min_bin; i < max_bin; i = i + w) {
                if (i != l) {
                    Serial.print(", ");
                    Serial.print(i);
                    Serial.print(":");
                };
                Serial.print(fft_vals[i]);
            }
        }
        */
        Serial.println();
        Serial.print("Bin with highest energy: "); Serial.print(highest_energy_idx);Serial.print(" = ");Serial.println(fft_vals[highest_energy_idx]);
        printFreqRangeOfBin(highest_energy_idx, max_bin);
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
        Serial.print("delta\t");Serial.print(rms_pos_delta);
        Serial.print(" average\t");Serial.println(getRMSAvg());
    }
}

void FeatureCollector::printPeakVals() {
    if (peak_active > 0) {
        Serial.print(id); Serial.print(" Peak vals\t");
        Serial.print(peak_val);printTab();
        Serial.print("delta\t");Serial.print(peak_pos_delta);
        Serial.print(" average\t");Serial.println(getPeakAvg());
    }
}

/////////////////////////////////// UPDATE / INIT //////////////////////////////////////
void FeatureCollector::update() {
    if (microphone_active == true) {
        if (last_update_timer > FC_UPDATE_RATE) {
            last_update_timer = 0;
            calculateRMS();
            calculatePeak();
            calculateFFT();
            calculateTone();
            calculateFreq();
            printFeatures();
        }
    }
    else {
        if (last_update_timer > 3000){
            Serial.print(id);
            Serial.println(" sorry the microphone does not work, not updating the feature collector");
            last_update_timer = 0;
        }
    }
}
#endif
