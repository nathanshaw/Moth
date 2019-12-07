#ifndef __CONFIGURATION_CICADAS_H__
#define __CONFIGURATION_CICADAS_H__

#include "Configuration.h"

// include RMS and/or PEAK in the feature collector?
#define PEAK_FEATURE_ACTIVE                 1
#define RMS_FEATURE_ACTIVE                  1

///////////////////////////////////////////////////////////// CICADA MODE /////////////////////////////////////////////////////////////////
// TODO - add bin magnitude as a feature
// which audio feature to use to test
// "peak" will look at the audio "peak" value
// "rms" will look at the audio "rms" value
#define SONG_FEATURE PEAK_DELTA

// TODO need to determine what are good values for these
#define MIN_SONG_PEAK_AVG 0.005
#define MAX_SONG_PEAK_AVG 0.20

#define STARTING_SONG_GAIN 4.0

#define SONG_BQ1_THRESH 13500
#define SONG_BQ1_Q 0.85
#define SONG_BQ1_DB -12
#define SONG_BQ2_THRESH 14000
#define SONG_BQ2_Q 0.85
#define SONG_BQ2_DB -12

///////// Click Settings
// what feature will be used to determine if a click has been found
// "rms_delta" will use that feature along with CLICK_RMS_DELTA_THRESH
// "peak_delta" will use that feature along with CLICK_PEAK_DELTA_THRESH
// "all" will use all available features with their corresponding thresholds

#define CLICK_FEATURE RMS_DELTA
#define CLICK_RMS_DELTA_THRESH 0.3
#define CLICK_PEAK_DELTA_THRESH 0.3

// One click per ten minutes
#define MIN_FPM_THRESH              0.1
#define LOW_FPM_THRESH              1.0
#define HIGH_FPM_THRESH             40
#define MAX_FPM_THRESH              60.0

// One click per ten minutes
#define MIN_ON_RATIO_THRESH              0.05
#define LOW_ON_RATIO_THRESH              0.30
#define HIGH_ON_RATIO_THRESH             0.90
#define MAX_ON_RATIO_THRESH              0.98

#define STARTING_CLICK_GAIN 4.0

#define MIN_CLICK_GAIN 0.5
#define MAX_CLICK_GAIN 24.0

#define MIN_SONG_GAIN 0.5
#define MAX_SONG_GAIN 20

#define CLICK_BQ1_THRESH 1200
#define CLICK_BQ1_Q 0.95
#define CLICK_BQ1_DB -24
#define CLICK_BQ2_THRESH 2500
#define CLICK_BQ2_Q 0.95
#define CLICK_BQ2_DB -24

// keeping track of clicks //////
long total_clicks_detected[2] = {0, 0}; // number of clicks which has occurred since boot
long num_past_clicks[2];            // number of clicks since last auto-gain adjustment
long num_cpm_clicks[2];

////////////////////////////////////////////////////////////////////////////
//////////////////////// Cicada Mode       /////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// if false, a click detected on either side results in a LED flash on both sides
// if true, a click detected on one side will only result in a flash on that side
bool INDEPENDENT_CLICKS = true;
bool combine_lux_readings = false;
bool gain_adjust_active = true;

#endif // MODE_CICADA CODE
