///////////////////////////////////////////////////////////////////////
//                    Click Audio Functions
///////////////////////////////////////////////////////////////////////
void printClickStats() {
  if (PRINT_CLICK_FEATURES) {
    // TODO update this to have a stereo option
    Serial.print("1000* Click | rms: ");
    Serial.print(click_rms_val[0] * 1000);
    Serial.print(" delta: ");
    Serial.print(click_rms_delta[0] * 1000);
    Serial.print(" peak: ");
    Serial.print(click_peak_val[0] * 1000);
    if (stereo_audio) {
      Serial.print(" R: ");
      Serial.print(click_rms_val[1] * 1000);
      Serial.print(" delta: ");
      Serial.print(click_rms_delta[1] * 1000);
      Serial.print(" peak: ");
      Serial.print(click_peak_val[1] * 1000);
    }
    Serial.println();
  }
}

void clickPrint(String data) {
  if (PRINT_CLICK_FEATURES) {
    Serial.print(data);
  }
}

void clickPrintLn() {
  if (PRINT_CLICK_FEATURES) {
    Serial.println();
  }
}

void clickPrintLn(String data) {
  if (PRINT_CLICK_FEATURES) {
    Serial.println(data);
  }
}

void clickDebugPrint(String data) {
  if (PRINT_CLICK_DEBUG) {
    Serial.print(data);
  }
}

void clickDebugPrintLn() {
  if (PRINT_CLICK_DEBUG) {
    Serial.println();
  }
}

void clickDebugPrintLn(String data) {
  if (PRINT_CLICK_DEBUG) {
    Serial.println(data);
  }
}


void updateClicks() {
  // this should be in the main audio loop
  calculateClickAudioFeatures();
  clickFlashUpdate();
}

void calculateClickAudioFeatures() {
  // todo double check the logic of this function...
  // for (int i = 0; i < num_channels; i++) {
  if (CLICK_FEATURE == "rms_delta" || CLICK_FEATURE == "all") {
    if (click_rms1.available()) {
      last_click_rms_val[0] = click_rms_val[0];
      click_rms_val[0] = click_rms1.read();
      click_rms_delta[0] = last_click_rms_val[0] - click_rms_val[0];
      if (click_rms_delta[0] > CLICK_RMS_DELTA_THRESH) {
        // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
        // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
        if (remaining_flash_delay[0] <= 0) {
          remaining_flash_delay[0] = MIN_CLICK_LENGTH;
#if (AUTO_GAIN == 1)
          num_past_clicks[0]++;
#endif
          total_clicks_detected[0]++;
          num_cpm_clicks[0]++;
          // Serial.print("num_cpm_clicks ");Serial.print(0);Serial.print("\t:\t");Serial.println(num_cpm_clicks[0]);
          // TODO have a timeout for the clicks, perhaps 100ms or something like that, prevent the click from being detected if under that time...
          clickPrint("\n-------------- FRONT CLICK DETECTED --------------------- ");
          clickPrint("rms_delta | ");
          clickPrint(num_past_clicks[0]); clickPrint("\t");
          clickPrint(total_clicks_detected[0]);
        } else if (remaining_flash_delay[0] < MAX_CLICK_LENGTH) {
          remaining_flash_delay[0]++;
          clickPrint(".");
        } else if (remaining_flash_delay[0] > MAX_CLICK_LENGTH) {
          remaining_flash_delay[0] = MAX_CLICK_LENGTH;
          clickPrint("-");
        }
      }
    }
    if (click_rms2.available()) {
      last_click_rms_val[1] = click_rms_val[1];
      click_rms_val[1] = click_rms2.read();
      click_rms_delta[1] = last_click_rms_val[1] - click_rms_val[1];
      if (click_rms_delta[1] > CLICK_RMS_DELTA_THRESH) {
        // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
        // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
        if (remaining_flash_delay[1] <= 0) {
          total_clicks_detected[1]++;
          num_cpm_clicks[1]++;
          // Serial.print("num_cpm_clicks ");Serial.print(1);Serial.print("\t:\t");Serial.println(num_cpm_clicks[1]);
          remaining_flash_delay[1] = MIN_CLICK_LENGTH;
#if (AUTO_GAIN == 1)
          num_past_clicks[1]++;
#endif
          // TODO have a timeout for the clicks, perhaps 100ms or something like that, prevent the click from being detected if under that time...
          clickPrint("\n-------------- REAR  CLICK DETECTED --------------------- ");
          clickPrint("rms_delta | ");
          clickPrint(num_past_clicks[1]); clickPrint("\t");
          clickPrint(total_clicks_detected[1]);
        } else if (remaining_flash_delay[1] < MAX_CLICK_LENGTH) {
          remaining_flash_delay[1]++;
          clickPrint(".");
        } else if (remaining_flash_delay[1] > MAX_CLICK_LENGTH) {
          clickPrint("-");
          remaining_flash_delay[1] = MAX_CLICK_LENGTH;
        }
      }
    }
  }/*
    if (CLICK_FEATURE == "peak_delta" || CLICK_FEATURE == "all") {
      if ( click_peak[i].available() ) {
        last_click_peak_val[i] = click_peak_val[i];
        click_peak_val[i] = click_peak[i].read();
        click_peak_delta[i] = last_click_peak_val[i]  - click_peak_val[i];
        if (click_peak_delta[i] > CLICK_PEAK_DELTA_THRESH) {
          // incrment num_past_clicks which keeps tract of the total number of clicks detected throughout the boot lifetime
          // If a click is detected set the flash timer to 20ms, if flash timer already set increase count by 1
          if (remaining_flash_delay[i] <= 0) {
            remaining_flash_delay[i] = MIN_CLICK_LENGTH;
            num_cpm_clicks[i]++;
            num_past_clicks[i]++;
            // clickPrint("-------------- CLICK "); clickPrint(i); clickPrint(" DETECTED --------------------- ");
            // clickPrint("peak_delta | ");
            // clickPrintLn(num_past_clicks[i]);
          } else if (remaining_flash_delay[i] < MAX_CLICK_LENGTH) {
            remaining_flash_delay[i]++;
          } else if (remaining_flash_delay[i] > MAX_CLICK_LENGTH) {
            remaining_flash_delay[i] = MAX_CLICK_LENGTH;
          }
        }
      }
    }
    if (CLICK_FEATURE != "peak_delta" && CLICK_FEATURE != "rms_delta") {
      clickPrint("sorry the CLICK_FEATURE ");
      clickPrint(CLICK_FEATURE);
      clickPrintLn(" is not implemented/available");
    }
*/
}

void flashOn(uint8_t side) {
  clickDebugPrint("\nFlashOn ");
  clickDebugPrintLn(side);
  if (side < 2) {
    setFlashOn(side, true); // turn the light on along with the flag
    leds_on[side] = true;
    colorWipe(0, 0, 255, side);
    resetLastClickFlash(side); // reset the elapsed millis variable as the light was just turned on
  } else {
    for (int i = 0; i < 2; i++) {
      setFlashOn(i, true);
      leds_on[i] = true;
      colorWipe(0, 0, 255, i);
      resetLastClickFlash(i); // reset the elapsed millis variable as the light was just turned on
    }
  }
}

void setFlashOn(int idx, bool val) {
  flash_on[idx] = val;
}

void flashOff(uint8_t side) {
  clickDebugPrint("FlashOff ");
  clickDebugPrintLn(side);
  if (side < 2) {
    setFlashOn(side, false);
    leds_on[side] = false;
    colorWipe(0, 0, 0, side);
    // remaining_flash_delay[side] = 0;
    // resetLastClickFlash(side);
  } else {
    for (int i = 0; i < 2; i++) {
      setFlashOn(i, false);
      leds_on[i] = false;
      // remaining_flash_delay[i] = 0;
      colorWipe(0, 0, 0, i);
      // resetLastClickFlash(side);
    }
  }
}

void resetLastClickFlash(uint8_t i) {
  if (last_click_flash != 0) {
    last_click_flash[i] = 0;
  }
}

void dependentClickFlashUpdate() {
  // go through each channel
  for (int i = 0; i < num_channels; i++) {
    // if there is time remaining in the flash it either needs to be turned on or the timer needs to increment
    if (remaining_flash_delay[i] > 0) {
      // clickPrint("flash delay "); clickPrint(i); clickPrint(" : ");
      // clickPrintLn(remaining_flash_delay[i]);
      if (flash_on[i] < 1) { //and the light is not currently on
        flashOn(2);// flash both sides
        resetLastClickFlash(1);
        resetLastClickFlash(0);
      }
      else {
        // if the light is already on
        // subtract the number of ms which have gone by since the last check
        // TODO optimize below lines
        // Serial.print("last_click_flash :"); Serial.println(last_click_flash[i]);
        // Serial.print("remaining_flash_delay["); Serial.print(i); Serial.print("] :\t");
        // Serial.print(remaining_flash_delay[i]); Serial.print("\t");
        remaining_flash_delay[i] = max(remaining_flash_delay[i] - last_click_flash[i], 0);
        // Serial.println(remaining_flash_delay[i]);

        if (remaining_flash_delay[i] == 0) {
          clickDebugPrint("Click time over, turning off flash "); clickDebugPrintLn(i);
          flashOff(2); // turn off the NeoPixels
        }
      }
    } else { };
    /*else { // there is no time in the flash delay timer
      if (flash_on[i]  > 0) {
        clickDebugPrintLn("WARNING : flash_on stuck to true for some reason");
        flashOff(2);
        resetLastClickFlash(i);
        }
      }
      */
  }
}

void independentClickFlashUpdate() {
  // go through ecah channel

  for (int i = 0; i < num_channels; i++) {
    // if there is time remaining in the flash it either needs to be turned on or the timer needs to increment
    if (remaining_flash_delay[i] > 0) {
       // Serial.print("independentClickFlashUpdate remaining/last_click_flash:\t");
       // Serial.print(remaining_flash_delay[i]);Serial.print("\t");Serial.print(last_click_flash[i]);
       // Serial.print("\t");

      clickPrint("flash delay "); clickPrint(i); clickPrint(" : ");
      clickPrintLn(remaining_flash_delay[i]);
      if (flash_on[i] < 1) { //and the light is not currently on
        flashOn(i);
        resetLastClickFlash(i);
        // remaining_flash_delay[i] = 0;
      } else {
        // if the light is already on
        // subtract the number of ms which have gone by since the last check
        // TODO optimize below lines
        remaining_flash_delay[i] = max(remaining_flash_delay[i] - last_click_flash[i], 0);
        
        // resetLastClickFlash(i); // todo should this be here?
        // flashOff(i);
        if (remaining_flash_delay[i] <= 0) {
          remaining_flash_delay[i] = 0;
          clickDebugPrint("Click time over, turning off flash "); clickDebugPrintLn(i);
          flashOff(i); // turn off the NeoPixels
        }
        // Serial.println(remaining_flash_delay[i]);
      }
    }/* else { // there is no time in the flash delay timer
      if (flash_on[i]  > 0) {
        flashOff(i);
        resetLastClickFlash(i); // todo should this be here?
      }
    }*/
  }
}

void clickFlashUpdate() {
  if (INDEPENDENT_CLICKS && stereo_audio) {
    independentClickFlashUpdate();
  } else {
    dependentClickFlashUpdate();
  }
}

void updateClickGain() {
  if (click_gain[0] > click_gain_max[0]) {
    click_gain_max[0] = click_gain[0];
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT, click_gain_max[0]);
    clickPrint("updated click front max recorded gain : "); clickPrintLn(click_gain[0]);
  }
  if (click_gain[0] < click_gain_min[0]) {
    click_gain_min[0] = click_gain[0];
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT, click_gain_min[0]);
    clickPrint("updated click front min recorded gain : "); clickPrintLn(click_gain_min[0]);
  }
  click_input_amp1.gain(click_gain[0]);
  click_mid_amp1.gain(click_gain[0]);
  click_post_amp1.gain(click_gain[0]);
  writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT_FRONT, click_gain[0]);
  clickPrint("updated click front gain : "); clickPrintLn(click_gain[0]);

  // repeat for the rear
  if (click_gain[1] > click_gain_max[1]) {
    click_gain_max[1] = click_gain[1];
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_REAR, click_gain_max[1]);
    clickPrint("updated click rear max recorded gain : "); clickPrintLn(click_gain_max[1]);
  }
  if (click_gain[1] < click_gain_min[1]) {
    click_gain_min[1] = click_gain[1];
    writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_REAR, click_gain_min[1]);
    clickPrint("updated click rear min recorded gain : "); clickPrintLn(click_gain_min[1]);
  }
  click_input_amp2.gain(click_gain[1]);
  click_mid_amp2.gain(click_gain[1]);
  click_post_amp2.gain(click_gain[1]);
  writeDoubleToEEPROM(EEPROM_CLICK_GAIN_CURRENT_REAR, click_gain[1]);
  clickPrint("updated click rear gain : ");
  clickPrintLn(click_gain[1]);
}

/*
  void updateClickGainBoth() {
  for (int i = 0; i < num_channels; i++) {
    if (click_gain[i] > click_gain_max[i]) {
      click_gain_max[i] = click_gain[i];
      writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MAX_FRONT, click_gain_max[i]);
    }
    if (click_gain[i] < click_gain_min[i]) {
      click_gain_min[i] = click_gain[i];
      writeDoubleToEEPROM(EEPROM_CLICK_GAIN_MIN_FRONT, click_gain_min[i]);
    }
    click_input_amp[i].gain(click_gain[i]);
    click_mid_amp[i].gain(click_gain[i]);
    click_post_amp[i].gain(click_gain[i]);
    clickPrint("updated click "); clickPrint(i); clickPrint(" gain : ");
    clickPrint(click_gain[i]);
    clickPrintLn();
  }
  }
*/
