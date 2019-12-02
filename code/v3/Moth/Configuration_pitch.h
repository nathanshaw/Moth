#ifndef __CONFIGURATION_PITCH_H__
#define __CONFIGURATION_PITCH_H__

///////////////////////////////////////////////////////////// PITCH MODE /////////////////////////////////////////////////////////////////
#define INPUT_START_GAIN            20.0

// logic to link left and right values? todo?
#define BQL_Q                       0.8
#define BQL_THRESH                  200
#define BQR_Q                       0.8
#define BQR_THRESH                  200

#define FFT_ACTIVE                  0
#define PEAK_ACTIVE                 1
#define RMS_ACTIVE                  1
#define TONE_ACTIVE                 0
#define FREQ_ACTIVE                 1

#define USE_HSB_MAPPING             1

#define FEATURE_RMS                 1
#define FEATURE_PEAK                2
#define FEATURE_FREQ                3
#define FEATURE_TONE                4
#define FEATURE_FFT                 5

#define HUE_FEATURE                 (FEATURE_TONE)
#define BRIGHTNESS_FEATURE          (FEATURE_PEAK)
#define SATURATION_FEATURE          (FEATURE_PEAK)

#define MODE_RGB                    0
#define MODE_HSB                    1

#define COLOR_MAP_MODE              (MODE_HSB)

#define  MODE_SINGLE_RANGE          0
#define  MODE_ALL_BINS              1

#define FFT_MODE                    (MODE_ALL_BINS)
// #define HUE_MAPPING
#define FFT_LOWEST_BIN              3

#define FREQ_UNCERTANITY_ALLOWED    0.15
#define USE_FFT_RANGE_FOR_COLOR     1

#endif // __CONFIGURATION_PITCH_H__
