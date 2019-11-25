
// lower pitches will be blue
// followed by purple
// green
// then yellow
// orange
// red
// white will be noise

// the brightness will be mapped by taking the rms or peak of the overall signal
#ifndef MODE_PITCH_H
#define MODE_PITCH_H
#include <Audio.h>

AudioInputI2S            i2s1;           //xy=70,94.00000095367432
AudioAmplifier           input_amp;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256      fft256;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency note_freq;      //xy=584.0000305175781,201.00000476837158
AudioOutputUSB           usb1;           //xy=1307.33353805542,1409.3331747055054
AudioConnection          patchCord1(i2s1, 0, input_amp, 0);
AudioConnection          patchCord2(input_amp, 0, biquad, 0);
AudioConnection          patchCord5(biquad, 0, tone_detect, 0);
AudioConnection          patchCord6(biquad, 0, peak, 0);
AudioConnection          patchCord7(input_amp, 0, fft256, 0);
AudioConnection          patchCord8(biquad, 0, rms, 0);
AudioConnection          patchCord9(biquad, 0, note_freq, 0);
AudioConnection          patchCord10(biquad, 0, usb1, 0);

double fft_vals[128]; 

void mapFFTToColor() {
  // determine which bin has the most energy
  
}


double getBinRangeEnergy(int s, int e) {
  double tot;
  if (fft256.available()) {
    for (int i = s; i < e; i++) {
      tot += fft256.read(i);
      
    }
  }
  return tot;
}

/*
int getIndexWithHighestVal(double &vals) {
  double m = 0.0;
  int index = 0;  for (int i = 0; i < sizeof(vals)/sizeof(vals[0]); i++) {
    if (vals[i] > m) {
      m = vals[i];
      index = i;
    }
  }
  return index;
}
*/
void setup() {
  AudioMemory(30);  
}

void loop() {
  if (fft256.available() ) {
    for (int i = 0; i < 128; i++) {
      double r = fft256.read(i);
      if (r > 0.0) {Serial.println(fft256.read(i));};
    }
  }
}
#endif// 
