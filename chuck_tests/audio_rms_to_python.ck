adc => PoleZero dcBlock => Gain inGain;
0.99 => dcBlock.blockZero;

inGain -> FFT fft =^ RMS rms => blackhole;

256 => fft.size;
128 => int WIN_SIZE;
Windowing.hann(WIN_SIZE) => fft.window;
UAnaBlob blobRMS;

OscSend oscSend;
oscSend.setHost("localhost", 6450);

0 => float rmsWeightedAverage;

fun void calcRMS() {
    rms.upchuck() @=> blobRMS;
    blobRMS * 1000 => rmsWeightedAverage;
    oscSend.startMsg("/rms, f");
    oscSend.addFloat(rmsWeightedAverage);
    <<<"sending song rms message : ", rmsWeightedAverage>>>;
}

while (true) {
    calcRMS();
    fft.size()::samp => now;
}