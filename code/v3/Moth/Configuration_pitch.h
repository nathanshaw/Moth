#ifndef __CONFIGURATION_PITCH_H__
#define __CONFIGURATION_PITCH_H__

#define INPUT_START_GAIN      12.0

#define BQL_Q                 0.8
#define BQL_THRESH            200
// logic to link left and right values? todo?
#define BQR_Q                 0.8
#define BQR_THRESH            200

#define PRINT_RMS_VALS        0
#define PRINT_FREQ_VALS       0
#define PRINT_TONE_VALS       0
#define PRINT_FFT_VALS        1

#define FFT_ACTIVE            1
#define PEAK_ACTIVE           0
#define RMS_ACTIVE            0
#define TONE_ACTIVE           0
#define FREQ_ACTIVE           0

#define FREQ_UNCERTANITY_ALLOWED 0.99

#endif // __CONFIGURATION_PITCH_H__
