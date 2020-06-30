#ifndef __CONFIGURATION_CICADAS_H__
#define __CONFIGURATION_CICADAS_H__

#include "Configuration.h"

// include RMS and/or PEAK in the feature collectors?
#define PEAK_FEATURE_ACTIVE             1
#define RMS_FEATURE_ACTIVE              1
#define FFT_FEATURE_ACTIVE              1

double MASTER_SENSITIVITY_SCALER =      1.0;

double CLICK_THRESH =                   0.325;
#define SMOOTH_CENTROID                 true

// what factor will the new values be scaled by compared to the old values 
// when calculating the song brightness target vs current levels a 1.0 will turn off
// the low filtering so only the new values will be used while 0.5 will result in the
// average of the old and new value to be used, a higher value will be a quicker responce
// the max value is 1.0 and the min value is 0.0
#define BRIGHTNESS_LP_LEVEL             (double)0.85
#define COLOR_LP_LEVEL                  (double)0.65

///////////////////////////////////////////////////////////// SONG /////////////////////////////////////////////////////////////////
// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
int SONG_FEATURE =                      PEAK_RAW;
int SONG_COLOR_FEATURE =                SPECTRAL_CENTROID;

// these were 0.25
#define SONG_PEAK_THRESH                0.15
#define SONG_RMS_THRESH                 0.15

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG               0.002
#define MAX_SONG_PEAK_AVG               0.2

// this is the gain that the input amps will be set at at the start of the installation
// #define STARTING_SONG_GAIN              8.0
// #define STARTING_CLICK_GAIN             16.0

// 30.0 is good for testing when no enclosure is present, but a higher value should be used when an enclosure is present
#define STARTING_GAIN                   30.0

#define SONG_BQ1_THRESH                 4000
#define SONG_BQ1_Q                      0.85
#define SONG_BQ1_DB                     -12
#define SONG_BQ2_THRESH                 16000
#define SONG_BQ2_Q                      0.85
#define SONG_BQ2_DB                     -12

// #define MIN_SONG_GAIN                   (STARTING_SONG_GAIN * 0.5)
// #define MAX_SONG_GAIN                   (STARTING_SONG_GAIN * 1.5)

///////// Click Settings
// what feature will be used to determine if a click has been found
// "rms_delta" will use that feature along with CLICK_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with CLICK_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
#define CALCULATE_FLUX                  true
#define CALCULATE_CENTROID              true
#define CALCULATE_ROFF                  false

int CLICK_FEATURE =                     PEAK_DELTA;
#define CLICK_RMS_DELTA_THRESH          0.05
#define CLICK_PEAK_DELTA_THRESH         0.17
#define CLICK_SPECTRAL_FLUX_THRESH      100.0
#define SPECTRAL_FLUX_SCALER            1

// these values are used to ensure that the light do not turn on when there are low 
// amplitudes in the sonic environment
#define RMS_LOW_THRESH                  0.1
#define PEAK_LOW_THRESH                 0.15
/*
 * TODO
 */
// one click per minute
#define MIN_FPM_THRESH                  1 
#define LOW_FPM_THRESH                  1.0
#define HIGH_FPM_THRESH                 40
#define MAX_FPM_THRESH                  60.0

/*
 *  TODO
 */
#define ON_RATIO_FEEDBACK               1
#define MIN_ON_RATIO_THRESH             0.05
#define LOW_ON_RATIO_THRESH             0.40
#define HIGH_ON_RATIO_THRESH            0.60
#define MAX_ON_RATIO_THRESH             0.90

/*
 * TODO
 */
#define CLICK_BQ1_THRESH                1200
#define CLICK_BQ1_Q                     0.85
#define CLICK_BQ1_DB                    -12
#define CLICK_BQ2_THRESH                3000
#define CLICK_BQ2_Q                     0.85
#define CLICK_BQ2_DB                    -12

#endif // MODE_CICADA CODE
