#ifndef __CONFIGURATION_PITCH_H__
#define __CONFIGURATION_PITCH_H__

///////////////////////////////////////////////////////////// PITCH MODE /////////////////////////////////////////////////////////////////
#define INPUT_START_GAIN                    20.0

// logic to link left and right values? todo?
#define BQL_Q                               0.8
#define BQL_THRESH                          200
#define BQR_Q                               0.8
#define BQR_THRESH                          200

#define FFT_FEATURE_ACTIVE                  1
#define PEAK_FEATURE_ACTIVE                 1
#define RMS_FEATURE_ACTIVE                  1
#define TONE_FEATURE_ACTIVE                 0
#define FREQ_FEATURE_ACTIVE                 0

#define USE_HSB_MAPPING                     1

// all the different features that are available to use through the feature collector for 
// varioud mappings.
#define FEATURE_RMS                         (1)
#define FEATURE_PEAK                        (2)
#define FEATURE_FREQ                        (3)
#define FEATURE_TONE                        (4)
#define FEATURE_FFT                         (5)
#define FEATURE_FFT_ENERGY                  (6)
#define FEATURE_FFT_RELATIVE_ENERGY         (7)
#define FEATURE_FFT_MAX_BIN                 (8)
#define FEATURE_FFT_BIN_RANGE               (9)

// when calculating the hue for the NeoPixel leds, what feature do you want to use?
#define HUE_FEATURE                 (FEATURE_FFT_MAX_BIN)
#define BRIGHTNESS_FEATURE          (FEATURE_FFT_ENERGY)
#define SATURATION_FEATURE          (FEATURE_FFT_RELATIVE_ENERGY)

// These are different color mapping modes
#define NEO_MAPPING_RGB             0
#define NEO_MAPPING_HSB             1

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#define COLOR_MAP_MODE              (NEO_MAPPING_HSB)

#define  MODE_SINGLE_RANGE          0
#define  MODE_ALL_BINS              1
#define  MODE_BIN_ENERGY            2

// #define HUE_MAPPING

//////////////////////////// FFT ///////////////////////////////////////
// When calculating things such as which bin has the most energy and so on, 
// what bin is considered the "1st?" and which one is the last?
// todo what frequency does this correspond to?
#define FFT_LOWEST_BIN              3
// todo this needs to be calculated better?
#define FFT_HIGHEST_BIN             40

// when using the Freq function generator, what amount of uncertanity is allowed?
#define FREQ_UNCERTANITY_ALLOWED    0.15

//////////////////////////// Printing and Datalogging ///////////////////////////////////////


#endif // __CONFIGURATION_PITCH_H__
