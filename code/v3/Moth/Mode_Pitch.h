// This section of code is for when the Moth is in pitch mode instead of cicada mode
// This mode will map the pitch content of the sounds it hears to different colors
// it will use the FFT 255 and map the bins to HSB instead of RGB

// this file will build the firmware just for this purpose with the intent of later creating a class which
// then is initalised dynamically depending on the position of a jumper on the PCB...

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
/*
AudioInputI2S            i2s1;           //xy=70,94.00000095367432
AudioAmplifier           input_amp;           //xy=224,88.00000095367432
AudioFilterBiquad        biquad;        //xy=386.00000381469727,88.00000095367432
AudioAnalyzeRMS          rms;           //xy=570.0000267028809,169.0000057220459
AudioAnalyzeToneDetect   tone_detect;          //xy=574.0000305175781,73.00000286102295
AudioAnalyzePeak         peak;          //xy=574.0000305175781,105.00000286102295
AudioAnalyzeFFT256      fft256;      //xy=580.0000305175781,137.00000381469727
AudioAnalyzeNoteFrequency note_freq;      //xy=584.0000305175781,201.00000476837158
AudioConnection          patchCord1(i2s1, 0, input_amp, 0);
AudioConnection          patchCord2(input_amp, biquad);
AudioConnection          patchCord5(biquad, tone_detect);
AudioConnection          patchCord6(biquad, peak);
AudioConnection          patchCord7(biquad, fft256);
AudioConnection          patchCord8(biquad, rms);
AudioConnection          patchCord9(biquad, note_freq);


double fft_vals[128]; 

void mapFFTToColor() {
  // determine which bin has the most energy
  
}
*/

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
#endif // MODE_PITCH_H
