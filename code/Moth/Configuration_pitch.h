#ifndef __CONFIGURATION_PITCH_H__
#define __CONFIGURATION_PITCH_H__

///////////////////////////////////////////////////////////// PITCH MODE /////////////////////////////////////////////////////////////////
#define STARTING_PITCH_GAIN                4.0
#define PITCH_AUTOGAIN_MIN                 2.0
#define PITCH_AUTOGAIN_MAX                 8.0

// Settings to cover the BiQuad Filter
#define BQ_Q                               0.5
// this can't be lower than 400 or else bad things happen =(
#define BQ_THRESH                          400
#define BQ_SHELF                           -12

/////////////////////////////// Feature Collector /////////////////////////////////
// Which Audio features will be activated?
#define FFT_FEATURE_ACTIVE                  1
#define PEAK_FEATURE_ACTIVE                 1
#define RMS_FEATURE_ACTIVE                  1
// these features are not currently implemented
// #define TONE_FEATURE_ACTIVE                 1
// #define FREQ_FEATURE_ACTIVE                 1

/////////////////////////////// Audio Features ////////////////////////////////////
// all the different features that are available to use through the feature collector for 
// varioud mappings.
#define FEATURE_RMS                         (1)
#define FEATURE_RMS_POS_DELTA               (11)
#define FEATURE_RMS_AVG                     (12)
#define FEATURE_PEAK                        (2)
#define FEATURE_PEAK_POS_DELTA              (22)
#define FEATURE_PEAK_AVG                    (23)
#define FEATURE_FREQ                        (3)
#define FEATURE_TONE                        (4)
#define FEATURE_FFT                         (5)
#define FEATURE_FFT_ENERGY                  (6)
#define FEATURE_FFT_RELATIVE_ENERGY         (7)
#define FEATURE_FFT_MAX_BIN                 (8)
#define FEATURE_FFT_BIN_RANGE               (9)
#define FEATURE_STRONG_FFT                  (10)

//////////////////////////// FFT //
// When calculating things such as which bin has the most energy and so on, 
// what bin is considered the "1st?" and which one is the last?
// todo what frequency does this correspond to?
#define FFT_LOWEST_BIN              1
// todo this needs to be calculated better?
#define FFT_NUM_BINS                511
#define FFT_HIGHEST_BIN             80
// when using the Freq function generator, what amount of uncertanity is allowed?
// #define FREQ_UNCERTANITY_ALLOWED    0.15

/////////////////////////////// Color Mapping /////////////////////////////////////
// when calculating the hue for the NeoPixel leds, what feature do you want to use?
// look under Audio Features for the Available Features
#define HUE_FEATURE                         (FEATURE_FFT_MAX_BIN)
#define BRIGHTNESS_FEATURE                  (FEATURE_PEAK_AVG)
#define SATURATION_FEATURE                  (FEATURE_FFT_RELATIVE_ENERGY)

// These are different color mapping modes
#define COLOR_MAPPING_RGB                     0
#define COLOR_MAPPING_HSB                     1

// For the neopixels will the color mapping exist within the RGB or HSB domain?
#define COLOR_MAP_MODE              (COLOR_MAPPING_HSB)

#define  MODE_SINGLE_RANGE          0
#define  MODE_ALL_BINS              1
#define  MODE_BIN_ENERGY            2

// #define HUE_MAPPING

//////////////////////////// Printing and Datalogging ///////////////////////////////////////


#endif // __CONFIGURATION_PITCH_H__
