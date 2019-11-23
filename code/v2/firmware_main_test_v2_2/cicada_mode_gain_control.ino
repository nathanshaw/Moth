void autoGainPrint(String data) {
  if (PRINT_AUTO_GAIN) {
    Serial.print(data);
  }
}

void autoGainPrintLn() {
  if (PRINT_AUTO_GAIN) {
    Serial.println();
  }
}

void autoGainPrintLn(String data) {
  if (PRINT_AUTO_GAIN) {
    Serial.println(data);
  }
}

#if (AUTO_GAIN == 1)


elapsedMillis last_auto_gain_adjustment; // the time in which the last auto_gain_was_calculated
// maximum amount of gain (as a proportion of the current gain) to be applied in the
// auto gain code. This value needs to be less than 1. 0.5 would mean that the gain can change
// by a factor of half its current gain. So, if the gain was 2.0 then it could be increased/decreased by 1.0
// with a max/min value of 1.0 / 3.0.
#define MIN_LED_ON_RATIO 0.3 
#define MAX_LED_ON_RATIO 0.95
void adjustSongGainLedOnRatio() {
  resetOnOffRatioCounters();
  for (int i = 0; i < num_channels; i++) {
    if (led_on_ratio[i] > MAX_LED_ON_RATIO) {
      double cost = 0.5;
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] -= change;
      // ensure that what we have is not less than the min
      song_gain[i] = max(song_gain[i], MIN_SONG_GAIN);
      autoGainPrint("led_on_ratio is too high (");autoGainPrint(led_on_ratio[i]);
      autoGainPrint(") lowering the song gain ");autoGainPrintLn(i);
      autoGainPrint(change);
      autoGainPrint(" ");
      updateSongGain();
    } else if (led_on_ratio[i] < MIN_LED_ON_RATIO) {
      double cost = 0.5;
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] += change;
      // ensure that what we have is not less than the min
      song_gain[i] = min(song_gain[i], MAX_SONG_GAIN);
      autoGainPrint("led_on_ratio is too low (");autoGainPrint(led_on_ratio[i]);
      autoGainPrint(") raising the song gain ");autoGainPrintLn(i);
      autoGainPrint(change);
      autoGainPrint(" ");
      updateSongGain();
    }
  }
}

void checkSongAutoGain() {
  adjustSongGainLedOnRatio();
  for (int i = 0; i < num_channels; i++) {
    ///////////////////////////////////////////////////////////////
    // second check is to see if the song gain needs to be adjusted
    ///////////////////////////////////////////////////////////////
    // calculate the average peak values since the last auto-gain adjust
    double avg_song_peak = total_song_peaks[i] / num_song_peaks[i];
    double cost; // our cost variable
    autoGainPrint("\n--------- song "); autoGainPrint(i); autoGainPrintLn(" -------------");
    autoGainPrint("total_song_peaks ");
    autoGainPrintLn(total_song_peaks[i]);
    autoGainPrint("num_song_peaks ");
    autoGainPrintLn(num_song_peaks[i]);
    // if the avg value is more than the max...
    if (avg_song_peak > MAX_SONG_PEAK_AVG) {
      // calculate cost between 0 and 1 with higher cost resulting in higher gain amplification
      cost = 1.0 - (MAX_SONG_PEAK_AVG / avg_song_peak);
      // calculate what the new song_gain will be
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] -= change;
      // ensure that what we have is not less than the min
      song_gain[i] = max(song_gain[i], MIN_SONG_GAIN);
      autoGainPrint("song gain decreased by ");
      autoGainPrint(change);
      autoGainPrint(" ");
      updateSongGain();
    }
    // if the average value is less than the min....
    else if (avg_song_peak < MIN_SONG_PEAK_AVG) {
      autoGainPrintLn();
      autoGainPrint("avg_song_peak lower than MIN_SONG_PEAK_AVG ");
      autoGainPrintLn(avg_song_peak);
      // calculate cost between 0 and 1 with higher cost resulting in higher gain attenuation
      cost = 1.0 - (MIN_SONG_PEAK_AVG / avg_song_peak);
      autoGainPrint("cost : ");
      autoGainPrintLn(cost);
      // calculate the new song gain
      double change = song_gain[i] * MAX_GAIN_ADJUSTMENT * cost;
      song_gain[i] += change;
      // ensure what we have is not less than the max...
      song_gain[i] = min(song_gain[i], MAX_SONG_GAIN);
      autoGainPrint("song gain increased by ");
      autoGainPrint(change);
      autoGainPrint(" ");
      autoGainPrintLn();
      updateSongGain();
    }

    // now check the avg on time? todo

    ///////////////////////////////////////////////////////////////
    // last thing to do is reset the last_auto_gain_adjustment timer
    ///////////////////////////////////////////////////////////////
    total_song_peaks[i] = 0;
    num_song_peaks[i] = 0;
  }
}

void checkClickAutoGain() {
  for (int i = 0; i < num_channels; i++) {
    autoGainPrint("num_past_clicks: ");
    autoGainPrintLn(num_past_clicks[i]);
    // first check is to see if there has been too many/few clicks detected
    double clicks_per_minute = ((double)num_past_clicks[i] * 60000) / (double)last_auto_gain_adjustment;
    double cost; // our cost variable
    autoGainPrint("clicks_per_minute: ");
    // printDouble(clicks_per_minute, 1000000);
    autoGainPrintLn(clicks_per_minute);
    // if we read 0 clicks since the last auto-gain-adjust then increase click gain by the max allowed.
    if (clicks_per_minute == 0) {
      click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT;
      click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);
      // update the click gain in the three gain stages
      updateClickGain();
    }
    // then check if there are too few clicks_per_minute
    else if (clicks_per_minute < MIN_CLICKS_PER_MINUTE) {
      // there are too few clicks, need to increase the gain to compensate
      // first we calculate the factor by which the clicks are off
      // the higher the number the more it is off
      cost = 1.0 - (clicks_per_minute / MIN_CLICKS_PER_MINUTE);

      // take the max amount that can be adjusted based off the current_gain and the max_gain_adj
      // then multiply that by the inverted cost
      click_gain[i] += click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;

      // make sure that the click_gain is not more than the max_click_gain
      click_gain[i] = min(click_gain[i], MAX_CLICK_GAIN);

      // update the click gain in the three gain stages
      updateClickGain();
    }
    // then check to see if there are too many clicks per-minute
    else if (clicks_per_minute > MAX_CLICKS_PER_MINUTE) {
      // determine a cost function... the higher the value the more the cost
      cost = 1.0 - (MAX_CLICKS_PER_MINUTE / clicks_per_minute);

      // adjust the click_gain
      click_gain[i] -= click_gain[i] * MAX_GAIN_ADJUSTMENT * cost;

      // make sure that the gain is not set too low
      click_gain[i] = max(click_gain[i], MIN_CLICK_GAIN);

      // update the click gain in the three gain stages
      updateClickGain();
    }
    num_past_clicks[i] = 0;
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
  autoGainPrintLn("-------------------- Auto Gain Start ---------------------------");
  checkClickAutoGain();
  checkSongAutoGain();

  last_auto_gain_adjustment = 0;
  autoGainPrintLn(" ------------------------------------------------- ");
}

#endif
