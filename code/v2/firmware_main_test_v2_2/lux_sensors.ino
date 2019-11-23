void luxPrint(String data) {
  if (PRINT_LUX_DEBUG) {
    Serial.print(data);
  }
}

void luxPrintLn() {
  if (PRINT_LUX_DEBUG) {
    Serial.println();
  }
}

void luxPrintLn(String data) {
  if (PRINT_LUX_DEBUG) {
    Serial.println(data);
  }
}

// todo move me to the correct place
void luxSetupCal(bool first_time) {
  // todo change this function so it takes the average of these readings
  Serial.println("------------------------");
  Serial.println("Starting Lux Calibration");
  double lux_tot[3];
  for (int i = 0; i < 10; i++) {
    Serial.print(i);
    Serial.print("\t");
    lux_reading_timer[0] = lux_min_reading_delay;
    lux_reading_timer[1] = lux_min_reading_delay;
    delay(100);
    readLuxSensors(2); // todo change this to not be hard coded
    if (!first_time) {
      printLuxReadings();
    };

    if (i > 4) {
      for (int iii = 0; iii < 3; iii++) {
        lux_tot[iii] += lux[iii];
      }
    }
  }
  // when we have the first 10 readings
  Serial.print("Average lux readings : ");
  for (int ii = 0; ii < 3; ii++) {
    lux[ii] = lux_tot[ii] / 5;
    Serial.print(lux[ii]);
    Serial.print("\t");
  }
  if (first_time) {
    for (int i = 0; i < 2; i++) {
      lux_total[i] = 0;
      lux_readings[i] = 0;
    }
  }
  Serial.println("\nLux calibration finished");
  Serial.println("------------------------\n");
}

uint8_t checkLuxSensors() {
  // Return a 1 if the lux sensors are read and a 0 if they are not
  // if the LEDs have been off, on their own regard, for 40ms or longer...
  // and it has been long-enough to warrent a new reading
  // luxPrint("\nchecking lux sensors: ");
  for (int i = 0; i < num_lux_sensors; i++) {
    // for each lux sensor, if the LEDs are off, have been off for longer than the LED_SHDN_LEN
    // and it has been longer than the
    // min reading delay then read the sensors
    if (leds_on[i] == false && led_off_len[i] >= LED_SHDN_LEN && lux_reading_timer[i] > lux_min_reading_delay) {
      if (I2C_MULTI) {
        readLuxSensors(i);
        printLuxReadings();
        return 1;
      } else {
        luxPrint("readLuxSensorsWithout()");
        readLuxSensorsWithout();
        return 1;
      }
    } else {
      // luxPrint("x");
    }
  }

  // if it has been longer than the "lux_max_reading_delay" force a new reading...
  // TODO update this code to handle reading on each side if both_lux_sensors active
  if (both_lux_sensors == true && combine_lux_readings == false) {
    for (int i = 0; i < num_lux_sensors; i++) {
      if (lux_reading_timer[i] > lux_max_reading_delay && led_shdn_timer[i] > LED_SHDN_LEN) {
        colorWipe(0, 0, 0, i); // TODO make sure this should not be called with a mode
        leds_on[i] = false;
        led_shdn_timer[i] = 0;
        luxPrint("lux_reading_timer for ");
        luxPrint(i); luxPrintLn(" is > lux_max_reading_delay and led_shdn_timer is greater than led_shdn_len");
      }
      else {
        luxPrint("y");
      }
    }
  } else if (both_lux_sensors == true && combine_lux_readings == true) {
    // todo for (int sensor = 0; sensor <
    for (int i = 0; i < 2; i++) {
      if (lux_reading_timer[i] > lux_max_reading_delay && led_shdn_timer[i] > LED_SHDN_LEN) {
        colorWipe(0, 0, 0, 0);
        colorWipe(0, 0, 0, 1);
        leds_on[0] = false;
        leds_on[1] = false;
        if (led_shdn_timer[i] > LED_SHDN_LEN) {
          led_shdn_timer[0] = 0;
          led_shdn_timer[1] = 0;
        }
        luxPrint(" \nSUCCESS: \nlux_reading_timer for ");luxPrint(i);
        luxPrintLn(" is > lux_max_reading_delay and led_shdn_timer is greater than led_shdn_len");
        luxPrintLn("setting lux_shdn_timer to 0 and turning off leds");
        return 0;
      }
      else {
        /*
        luxPrint("failed: lux_reading_timer for ");
        luxPrint(i); luxPrint(" is :\t");
        luxPrint((long)lux_reading_timer[i]);
        luxPrint("\t");luxPrintLn(lux_max_reading_delay);
        // luxPrint("z");
        */
      }
    }
  }
  else if (both_lux_sensors == false) {
    if (lux_reading_timer[0] > lux_max_reading_delay && led_shdn_timer[0] > LED_SHDN_LEN) {
      // if it is over the max amount of time for a reading, and a shutdown is not active,
      // then force a reading
      // turn the LEDs off
      // luxPrint("lux_reading_timer for 0 ");
      // luxPrintLn(" is > lux_max_reading_delay and led_shdn_timer is greater than led_shdn_len");
      colorWipe(0, 0, 0, 0);
      //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
      led_shdn_timer[0] = 0;
      leds_on[0] = false;
      return 0;
    }
    else {
      luxPrint("y");
    }
  } else {
    // luxPrint("lux_reading_timer/lux_max_reading_delay :\t");
    // luxPrint((long)lux_reading_timer[0]); luxPrint(" / "); luxPrintLn(lux_max_reading_delay);
  }
}

void readLuxSensorsWithout() {
  lux[0] = (lux[0] + veml[0].readLux()) * 0.5;
  // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
  if (lux[0] > 1000000) {
    // luxPrint("lux "); luxPrint(0); luxPrint(" reading error: ");
    // luxPrintLn(lux[0]);
    // take the reading again
    if (SMOOTH_LUX_READINGS && lux[0] != 0) {
      lux[0] = (lux[0] + veml[0].readLux()) * 0.5;
    } else {
      lux[0] = veml[0].readLux();
    }
  }
  // TODO souble check that all these numbers should be hard coded, same for the rear below
  // calculate the combined lux
  if (num_lux_sensors == 1) {
    // if there is only one lux sensor then make the rear lux and combined lux equal to the front lux
    lux[2] = lux[0];
    lux[1] = lux[0];
  } else {
    // otherwise set the combined lux to the average of the two lux readings
    lux[2] = (lux[0] + lux[1]) * 0.5;
  }
  // update the brightness scales
  for (int i = 0; i < num_lux_sensors + 1; i++) {
    brightness_scaler[i] = map(constrain(lux[0], MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  }

  updateLuxMinMax();
}

double checkForLuxOverValue(int i) {
  // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
  if (lux[i] > 100000) {
    luxPrint("lux "); luxPrint(i); luxPrint(" reading error: ");
    luxPrintLn(lux[i]);
    // take the reading again
    if (SMOOTH_LUX_READINGS && lux[i] != 0) {
      lux[i] = (lux[i] + veml[i].readLux()) * 0.5;
      lux_total[i] += lux[i];
      lux_readings[i]++;
    } else {
      lux[i] = veml[i].readLux();
      lux_total[i] += lux[i];
      lux_readings[i]++;
    }
  }
  return lux[i];
}

void readLuxSensors(uint8_t mode) {
  // mode 0 will read just the rear sensors, mode 1 will read just the rear sensors, mode 2 will read them both
  // luxPrint(millis()); luxPrint("readLuxSensors("); luxPrint(mode); luxPrintLn(")");
  uint8_t _start;
  uint8_t _end;
  if (mode == 0 ) {
    _start = 0;
    _end = 1;
  } else if (mode == 1) {
    _start = 1;
    _end = 2;
  } else if (mode == 2) {
    _start = 0;
    _end = 2;
  } else {
    luxPrintLn("WARNING _start and _end are the same value");
    _start = 0;
    _end = 0;
  }

  for (int i = _start; i < _end; i++) {
    tcaselect(i);
    if (SMOOTH_LUX_READINGS && lux[i] != 0) {
      lux[i] = (lux[i] + veml[i].readLux()) * 0.5;
      checkForLuxOverValue(i);
      lux_total[i] += lux[i];
      lux_readings[i]++;
    } else {
      lux[i] = veml[i].readLux();
      checkForLuxOverValue(i);
      lux_total[i] += lux[i];
      lux_readings[i]++;
    }
    lux_reading_timer[i] = 0;
    // luxPrint("lux_readings "); luxPrint(i); luxPrint(" increased to : ");
    // luxPrintLn(lux_readings[i]);
  }
  // TODO souble check that all these numbers should be hard coded, same for the rear below
  // calculate the combined lux
  if (num_lux_sensors == 1) {
    // if there is only one lux sensor then make the rear lux and combined lux equal to the front lux
    lux[2] = lux[0];
    lux[1] = lux[0];
  } else {
    // otherwise set the combined lux to the average of the two lux readings
    lux[2] = (lux[0] + lux[1]) * 0.5;
  }
  // update the brightness scales TODO
  for (int i = 0; i < num_lux_sensors + 1; i++) {
    brightness_scaler[i] = map(constrain(lux[i], MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000;
  }
  updateLuxMinMax();
  updateBrightnessScalerTotals();
  updateBrightnessScalerAverages();
  // printBrightnessScalers();
}

void printBrightnessScalers() {
    luxPrint("Brightness Scalers :\t");
    for (int i =  0; i < 6; i++){
        luxPrint(brightness_scaler[i]);luxPrint("\t");
    }
    luxPrintLn("\n");
}

void printLuxReadings() {
  if (PRINT_LUX_READINGS) {
    Serial.print("Lux: "); Serial.print(lux[2]); Serial.print(" = ");
    Serial.print(lux[0]); Serial.print(" ("); Serial.print(brightness_scaler[0]); Serial.print(")");
    Serial.print(" + "); Serial.print(lux[1]); Serial.print(" ("); Serial.print(brightness_scaler[1]); Serial.println(")");
  }
}
