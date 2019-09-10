#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=55,153.00000190734863
AudioAmplifier           input_amp_r;           //xy=224.00001525878906,152.00000190734863
AudioAmplifier           input_amp_l;           //xy=225.00000381469727,119.00000190734863
AudioAnalyzeRMS          rms_input_l;           //xy=228,187.00000190734863
AudioAnalyzeRMS          rms_input_r;           //xy=228.00000190734863,219.00000095367432
AudioFilterBiquad        click_biquad_l;        //xy=408.00000762939453,77.00000095367432
AudioFilterBiquad        click_biquad_r;        //xy=408.0000114440918,110.00000286102295
AudioFilterBiquad        song_biquad_l;        //xy=410.00000762939453,184.00000286102295
AudioFilterBiquad        song_biquad_r;        //xy=410.0000114440918,216.0000057220459
AudioAnalyzeRMS          song_rms_l;           //xy=567.3999824523926,186.00000190734863
AudioAnalyzeRMS          song_rms_r;           //xy=567.4000205993652,217.0000057220459
AudioAnalyzeRMS          click_rms_r;           //xy=621.4000205993652,52.000000953674316
AudioAnalyzeRMS          click_rms_l;           //xy=622.3999824523926,20
AudioAnalyzeFFT1024      click_fft1024_l;      //xy=630.8889083862305,83.33333969116211
AudioAnalyzeFFT1024      click_fft1024_r;      //xy=632.0000076293945,114.4444522857666
AudioConnection          patchCord1(i2s1, 0, input_amp_l, 0);
AudioConnection          patchCord2(i2s1, 0, rms_input_l, 0);
AudioConnection          patchCord3(i2s1, 1, input_amp_r, 0);
AudioConnection          patchCord4(i2s1, 1, rms_input_r, 0);
AudioConnection          patchCord5(input_amp_r, click_biquad_r);
AudioConnection          patchCord6(input_amp_r, song_biquad_r);
AudioConnection          patchCord7(input_amp_l, click_biquad_l);
AudioConnection          patchCord8(input_amp_l, song_biquad_l);
AudioConnection          patchCord9(click_biquad_l, click_rms_l);
AudioConnection          patchCord10(click_biquad_l, click_fft1024_l);
AudioConnection          patchCord11(click_biquad_r, click_rms_r);
AudioConnection          patchCord12(click_biquad_r, click_fft1024_r);
AudioConnection          patchCord13(song_biquad_l, song_rms_l);
AudioConnection          patchCord14(song_biquad_r, song_rms_r);
AudioControlSGTL5000     sgtl5000_1;     //xy=195,65.00000190734863
// GUItool: end automatically generated code



void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
