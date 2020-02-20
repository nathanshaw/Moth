#ifndef __CONFIGURATION_CICADAS_H__
#define __CONFIGURATION_CICADAS_H__

#include "Configuration.h"

// include RMS and/or PEAK in the feature collectors?
#define PEAK_FEATURE_ACTIVE             1
#define RMS_FEATURE_ACTIVE              1
#define FFT_FEATURE_ACTIVE              1

///////////////////////////////////////////////////////////// SONG /////////////////////////////////////////////////////////////////
// if this is set to true the flashes will only contain the Flash color instea
// if this is set to false then the flash color will be added to the normal color
#define FLASH_DOMINATES                 false
// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
int SONG_FEATURE =                      PEAK_RAW;
int SONG_COLOR_FEATURE =                SPECTRAL_CENTROID;

#define SONG_PEAK_THRESH          0.25
#define SONG_RMS_THRESH           0.25

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG               0.002
#define MAX_SONG_PEAK_AVG               0.2

// this is the gain that the input amps will be set at at the start of the installation
#define STARTING_SONG_GAIN              6.0
#define STARTING_CLICK_GAIN             12.0

#define SONG_BQ1_THRESH                 4000
#define SONG_BQ1_Q                      0.85
#define SONG_BQ1_DB                     -12
#define SONG_BQ2_THRESH                 16000
#define SONG_BQ2_Q                      0.85
#define SONG_BQ2_DB                     -12

#define MIN_SONG_GAIN                   (STARTING_SONG_GAIN * 0.5)
#define MAX_SONG_GAIN                   (STARTING_SONG_GAIN * 1.5)

///////// Click Settings
// what feature will be used to determine if a click has been found
// "rms_delta" will use that feature along with CLICK_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with CLICK_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds
#define CALCULATE_FLUX                  true
#define CALCULATE_CENTROID              true
#define CALCULATE_ROFF                  true

int CLICK_FEATURE =                     PEAK_DELTA;
#define CLICK_RMS_DELTA_THRESH          0.05
#define CLICK_PEAK_DELTA_THRESH         0.20
#define CLICK_SPECTRAL_FLUX_THRESH      100.0
#define SPECTRAL_FLUX_SCALER            1

// these values are used to ensure that the light do not turn on when there are low 
// amplitudes in the sonic environment
#define RMS_LOW_THRESH                  0.1
#define PEAK_LOW_THRESH                 0.1

// One click per ten minutes
#define MIN_FPM_THRESH                  0.1
#define LOW_FPM_THRESH                  1.0
#define HIGH_FPM_THRESH                 40
#define MAX_FPM_THRESH                  60.0

#define MIN_ON_RATIO_THRESH             0.10
#define LOW_ON_RATIO_THRESH             0.40
#define HIGH_ON_RATIO_THRESH            0.60
#define MAX_ON_RATIO_THRESH             0.90

#define MIN_CLICK_GAIN                  (STARTING_CLICK_GAIN * 0.5)
#define MAX_CLICK_GAIN                  (STARTING_CLICK_GAIN * 1.5)

#define CLICK_BQ1_THRESH                1200
#define CLICK_BQ1_Q                     0.95
#define CLICK_BQ1_DB                    -24
#define CLICK_BQ2_THRESH                3500
#define CLICK_BQ2_Q                     0.95
#define CLICK_BQ2_DB                    -24

// keeping track of clicks //////
long total_clicks_detected[2] =         {0, 0}; // number of clicks which has occurred since boot
long num_past_clicks[2];            // number of clicks since last auto-gain adjustment
long num_cpm_clicks[2];

#endif // MODE_CICADA CODE
