void updateLBS(uint8_t feature) {
    if (lbs_timer > LBS_TIME_FRAME) {
        lbs_timer = 0;
        lbs_min = (uint8_t)((double)lbs_min * 1.1);
        lbs_max = (uint8_t)((double)lbs_max * 0.9);
        Serial.print("Reset the lbs timers");
    }
    if (feature > lbs_max) {
        lbs_max = feature;
    } else if (feature < lbs_min) {
        lbs_min = feature;
    } else {
        return;
    }
    // if we do not return then it means we updated the min or max and now
    // need to update the lbs_scaler_min_thresh and max thresb
    // double range  = lbs_max - lbs_min;
    // double lbs_scaler_min_thresh = lbs_min
    // double lbs_scaler_max_thresh =
    dprint(PRINT_LBS, "old lbs min/max : ");
    dprint(PRINT_LBS, lbs_scaler_min_thresh);
    dprint(PRINT_LBS, " / ");
    dprintln(PRINT_LBS, lbs_scaler_max_thresh);
    lbs_scaler_min_thresh = lbs_min + ((lbs_max - lbs_min) * LBS_LOW_TRUNCATE_THRESH);
    lbs_scaler_max_thresh = lbs_max + ((lbs_max - lbs_min) * LBS_HIGH_TRUNCATE_THRESH);
    dprint(PRINT_LBS, "\tnew min/max : ");
    dprint(PRINT_LBS, lbs_scaler_min_thresh);
    dprint(PRINT_LBS, " / ");
    dprintln(PRINT_LBS, lbs_scaler_max_thresh);
}

uint8_t applyLBS(uint8_t brightness) {
    dprint(PRINT_LBS, "brightness (Before/After) lbs: ");
    dprint(PRINT_LBS, brightness);
    updateLBS(brightness);
    // constrain the brightness to the low and high thresholds
    dprint(PRINT_LBS, " / ");
    brightness = constrain(brightness,  lbs_scaler_min_thresh, lbs_scaler_max_thresh);
    brightness = map(brightness, lbs_scaler_min_thresh, lbs_scaler_max_thresh, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    dprint(PRINT_LBS, " = ");
    dprint(PRINT_LBS, brightness);
    dprint(PRINT_LBS, "\tmin/max thresh: ");
    dprint(PRINT_LBS, lbs_scaler_min_thresh);    
    dprint(PRINT_LBS, " / ");    
    dprintln(PRINT_LBS, lbs_scaler_max_thresh);    
    return brightness;
}