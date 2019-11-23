/*#ifndef __CICADA_GAIN_CONTROL_H__
#define __CICADA_GAIN_CONTROL_H__
#include "MothConfig.h"
// #include "Mode_Cicada.h"
*/
elapsedMillis last_auto_gain_adjustment; // the time in which the last auto_gain_was_calculated

void checkClickAutoGain() {
  bool update_gain = false;                                                   // will we update the gain at the end of the function?
  for (int i = 0; i < num_channels; i++) {
    dprint(PRINT_AUTO_GAIN, "num_past_clicks: ");
    dprintln(PRINT_AUTO_GAIN, (String)num_past_clicks[i]);
    
    // first check is to see if there has been too many/few clicks detected
    double clicks_per_minute = ((double)num_past_clicks[i] * 60000) / (double)last_auto_gain_adjustment;
    double cost;                                                              // our cost variable
    dprint(PRINT_AUTO_GAIN, "clicks_per_minute: ");
    // printDouble(clicks_per_minute, 1000000);
    dprintln(PRINT_AUTO_GAIN, clicks_per_minute);
       
    if (clicks_per_minute == 0) {                                             // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
      click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT;
      click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);
      update_gain = true;
    }
    // then check if there are too few clicks_per_minute
    else if (clicks_per_minute < MIN_CLICKS_PER_MINUTE) {
      // there are too few clicks, need to increase the gain to compensate
      // first we calculate the factor by which the clicks are off
      // the higher the number the more it is off
      cost = 1.0 - (clicks_per_minute / MIN_CLICKS_PER_MINUTE);              // max amount that can be adjusted     
      click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);                    // make sure that the click_gain is not more than the max_click_gain
      update_gain = true;
    }
    // then check to see if there are too many clicks per-minute
    else if (clicks_per_minute > MAX_CLICKS_PER_MINUTE) {
      cost = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute);              // determine a cost function... the higher the value the more the cost
      click_gain[i] -= click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;           // adjust the click_gain
      click_gain[i] = max(click_gain[i], MIN_CLICK_GAIN);                    // make sure that the gain is not set too low
      update_gain = true;
    }
    num_past_clicks[i] = 0;
  }
  if (update_gain = true) {  
      updateClickGain(click_gain, click_gain_min, click_gain_max);          // update the click gain in the three gain stages
  }
}

void autoGainAdjust() {
  // TODO update this for both front and rear
  /* The purpose of this function is to determine the gain level for both the "song" and the
       "click" bands. The function will run for a number of milliseconds equal to the variable len
      passed into it. (usually around 5 or 10 seconds)

     It will poll the RMS and Peak for each band for this period of time and determine what the max
     and average values are.

     It will then adjust the master gain levels, click_gain and song_gain to meet target values
     target_click_rms and target_song_rms.

      This function should be run in the setup loop when the teensy is booting as well as a few times an
      hour to adjust the gain levels.

  */

  // dont run this logic unless the firmware has been running for one minute, any less time will result in erroneous values
  if (millis() < 60000) {
    return;
  }
  // if it has not been long enough since the last check then exit now
  if (last_auto_gain_adjustment < auto_gain_frequency) {
    return;
  };
  dprintln(PRINT_AUTO_GAIN, "-------------------- Auto Gain Start ---------------------------");

  // todo add back
  // resetOnOffRatioCounters();
  adjustSongGainLedOnRatio();
  
  checkClickAutoGain();
  checkSongAutoGain();

  last_auto_gain_adjustment = 0;
  dprintln(PRINT_AUTO_GAIN, " ------------------------------------------------- ");
}

// #endif // __CICADA_GAIN_CONTROL_H__
