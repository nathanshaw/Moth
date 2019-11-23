///////////////////////////////////////////////////////////////////////
//                       TCA9532A I2C bus expander
///////////////////////////////////////////////////////////////////////
#ifndef __LUX_H__
#define __LUX_H__

// #include "MothConfig.h"
#include "Adafruit_VEML7700.h"
// #include "logging_lux.h"
// #include "Neos.h"

Adafruit_VEML7700 veml[2] = Adafruit_VEML7700();

#include <Wire.h>
// TODO add code so if the TCA is not available things are cool... also add firmware #define to control this
#define TCAADDR 0x70
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setupVEMLthroughTCA() {
  Wire.begin();
  if (!I2C_MULTI) {
    if (!veml[0].begin()) {
      Serial.print("VEML "); Serial.print(0); Serial.println(" not found");
    }
    else {
      Serial.print("VEML "); Serial.print(0); Serial.println(" found");
      veml[0].setGain(VEML7700_GAIN_1);
      veml[0].setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
    }
  } else {
    for (int i = 0; i < both_lux_sensors + 1; i++) {
      tcaselect(i);
      if (!veml[i].begin()) {
        Serial.print("VEML "); Serial.print(i); Serial.println(" not found");
      } else {
        Serial.print("VEML "); Serial.print(i); Serial.println(" found");
        veml[i].setGain(VEML7700_GAIN_1);
        veml[i].setIntegrationTime(VEML7700_IT_25MS);// 800ms was default
      }
    }
  }
}

double checkForLuxOverValue(int i) {
  // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
  if (lux[i] > 100000) {
    dprintln(PRINT_LUX_DEBUG, "lux "); dprintln(PRINT_LUX_DEBUG, i); dprintln(PRINT_LUX_DEBUG, " reading error: ");
    dprintln(PRINT_LUX_DEBUG, (String)lux[i]);
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
  dprintln(PRINT_LUX_DEBUG, millis()); dprintln(PRINT_LUX_DEBUG, "readLuxSensors("); dprintln(PRINT_LUX_DEBUG, mode); dprintln(PRINT_LUX_DEBUG, ")");
  int _start;
  int _end;
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
    dprintln(PRINT_LUX_DEBUG, "WARNING _start and _end are the same value");
    _start = 0;
    _end = 0;
  }
  dprint(PRINT_LUX_DEBUG, "start/end\t"); dprint(PRINT_LUX_DEBUG, _start);
  dprint(PRINT_LUX_DEBUG, "\t"); dprintln(PRINT_LUX_DEBUG, _end);
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
    dprint(PRINT_LUX_DEBUG, "lux_readings "); dprint(PRINT_LUX_DEBUG, i);
    dprint(PRINT_LUX_DEBUG, " increased to : ");
    dprintln(PRINT_LUX_DEBUG, lux_readings[i]);
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
  // update the brightness scales TODO , this logic does not work if the number of lux sensors is less than the number of groups
  if (millis() > LOG_START_DELAY) {
    for (int i = 0; i < num_lux_sensors; i++) {
      neos[i].setBrightnessScaler(map(constrain(lux[i], MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000);
      dprint(PRINT_LUX_DEBUG, "changed brightness scaler to : ");
      dprintln(PRINT_LUX_DEBUG, neos[i].getBrightnessScaler());
    }
    updateLuxMinMax(lux, min_lux_reading, max_lux_reading);
    // todo - make sure these two can be taken out..
  }
}

void printBrightnessScalers() {
  dprintln(PRINT_LUX_DEBUG, "Brightness Scalers :\t");
  for (int i =  0; i < 6; i++) {
    Serial.println("printBrightnessScalers broken");
    //dprintln(PRINT_LUX_DEBUG,brightness_scaler[i]);dprintln(PRINT_LUX_DEBUG,"\t");
  }
  dprintln(PRINT_LUX_DEBUG, "\n");
}

void printLuxReadings() {
  if (PRINT_LUX_READINGS) {
    Serial.print("Lux: "); Serial.print(lux[2]); Serial.print(" = ");
    Serial.print(lux[0]); Serial.print(" ("); Serial.print(neos[0].getBrightnessScaler()); Serial.print(")");
    Serial.print(" + "); Serial.print(lux[1]); Serial.print(" ("); Serial.print(neos[1].getBrightnessScaler()); Serial.println(")");
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
    if (first_time) {
      printLuxReadings();
    }
    if (i > 4) {
      for (int iii = 0; iii < 3; iii++) {
        lux_tot[iii] += lux[iii];
      }
    }
    Serial.print(".");
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

void readLuxSensorsWithout() {
  lux[0] = (lux[0] + veml[0].readLux()) * 0.5;
  // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
  if (lux[0] > 1000000) {
    // dprintln(PRINT_LUX_DEBUG,"lux "); dprintln(PRINT_LUX_DEBUG,0); dprintln(PRINT_LUX_DEBUG," reading error: ");
    // dprintln(PRINT_LUX_DEBUG,lux[0]);
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
    neos[i].setBrightnessScaler(map(constrain(lux[0], MIN_LUX_EXPECTED, MAX_LUX_EXPECTED), MIN_LUX_EXPECTED, MAX_LUX_EXPECTED, BRIGHTNESS_SCALER_MIN, 1000) / 1000);
  }
  updateLuxMinMax(lux, min_lux_reading, max_lux_reading);
}

uint8_t checkLuxSensors() {
  // Return a 1 if the lux sensors are read and a 0 if they are not
  // if the LEDs have been off, on their own regard, for 40ms or longer...
  // and it has been long-enough to warrent a new reading
  // dprintln(PRINT_LUX_DEBUG,"\nchecking lux sensors: ");
  for (int i = 0; i < num_lux_sensors; i++) {
    // for each lux sensor, if the LEDs are off, have been off for longer than the LED_SHDN_LEN
    // and it has been longer than the
    // min reading delay then read the sensors
    if (neos[i].getLedsOn() == false && neos[i].getOnOffLen() >= LED_SHDN_LEN && lux_reading_timer[i] > lux_min_reading_delay) {
      if (I2C_MULTI) {
        readLuxSensors(i);
        printLuxReadings();
        return 1;
      } else {
        dprintln(PRINT_LUX_DEBUG, "readLuxSensorsWithout()");
        readLuxSensorsWithout();
        return 1;
      }
    } else {
      // dprintln(PRINT_LUX_DEBUG,"x");
    }
  }

  // if it has been longer than the "lux_max_reading_delay" force a new reading...
  // TODO update this code to handle reading on each side if both_lux_sensors active
  if (both_lux_sensors == true && combine_lux_readings == false) {
    for (int i = 0; i < num_lux_sensors; i++) {
      if (lux_reading_timer[i] > lux_max_reading_delay && neos[i].getShdnTimer() > LED_SHDN_LEN) {
        neos[i].luxShutdown();
        dprintln(PRINT_LUX_DEBUG, "lux_reading_timer for ");
        dprintln(PRINT_LUX_DEBUG, i); dprintln(PRINT_LUX_DEBUG, " is > lux_max_reading_delay and led_shdn_timer is greater than led_shdn_len");
      }
      else {
        dprintln(PRINT_LUX_DEBUG, "y");
      }
    }
  } else if (both_lux_sensors == true && combine_lux_readings == true) {
    // todo for (int sensor = 0; sensor <
    for (int i = 0; i < 2; i++) {
      if (lux_reading_timer[i] > lux_max_reading_delay && neos[i].getShdnTimer() > LED_SHDN_LEN) {
        for (int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++){
            neos[i].luxShutdown();
        }
        dprintln(PRINT_LUX_DEBUG, " \nSUCCESS: \nlux_reading_timer for "); dprintln(PRINT_LUX_DEBUG, i);
        dprintln(PRINT_LUX_DEBUG, " is > lux_max_reading_delay and led_shdn_timer is greater than led_shdn_len");
        dprintln(PRINT_LUX_DEBUG, "setting lux_shdn_timer to 0 and turning off leds");
        return 0;
      }
      else {
        /*
          dprintln(PRINT_LUX_DEBUG,"failed: lux_reading_timer for ");
          dprintln(PRINT_LUX_DEBUG,i); dprintln(PRINT_LUX_DEBUG," is :\t");
          dprintln(PRINT_LUX_DEBUG,(long)lux_reading_timer[i]);
          dprintln(PRINT_LUX_DEBUG,"\t");dprintln(PRINT_LUX_DEBUG,lux_max_reading_delay);
          // dprintln(PRINT_LUX_DEBUG,"z");
        */
      }
    }
  }
  else if (both_lux_sensors == false) {
    if (lux_reading_timer[0] > lux_max_reading_delay && neos[0].getShdnTimer() > LED_SHDN_LEN) {
      // if it is over the max amount of time for a reading, and a shutdown is not active,
      // then force a reading
      // turn the LEDs off
      // dprintln(PRINT_LUX_DEBUG,"lux_reading_timer for 0 ");
      // dprintln(PRINT_LUX_DEBUG," is > lux_max_reading_delay and led_shdn_timer is greater than led_shdn_len");
      
        for (int i = 0; i < sizeof(neos) / sizeof(neos[0]); i++){
            neos[i].luxShutdown();
        }
      //reset the led_shdn timer to ensure that the LEDs stay off for 40 ms
      return 0;
    }
    else {
      dprintln(PRINT_LUX_DEBUG, "y");
    }
  } else {
    // dprintln(PRINT_LUX_DEBUG,"lux_reading_timer/lux_max_reading_delay :\t");
    // dprintln(PRINT_LUX_DEBUG,(long)lux_reading_timer[0]); dprintln(PRINT_LUX_DEBUG," / "); dprintln(PRINT_LUX_DEBUG,lux_max_reading_delay);
  }
  return 0;
}

uint8_t updateBrightnessScalerAvgLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  if (log_timer > LOG_POLLING_RATE && cpm_eeprom_idx < EEPROM_CPM_LOG_END) {
    Serial.print("Logging the average brightness scalers  :");
    for (int  i = 0; i < 2; i++) { // replace 2 with something variable
      Serial.print("\t");
      writeDoubleToEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4), neos[i].getAvgBrightnessScaler());
      Serial.print(neos[i].getAvgBrightnessScaler());
    }
    Serial.println();
    return 1;
  }
  return 0;
}

void printBrightnessAverageLog() {
  // TODO
  // write the current brightness scaler average to EEPROM
  Serial.print("Printing the average brightness scalers:");
  for (int  i = 0; i < 3; i++) {
    Serial.print("\t");
    Serial.print(readDoubleFromEEPROM(EEPROM_AVG_BRIGHTNESS_SCALER + (i * 4)));
  }
  Serial.println();
}

uint8_t updateOnRateLog() {
  // TODO
  if (log_timer > LOG_POLLING_RATE) {
    Serial.print("Logging the LED on rates                :");
    for (int  i = 0; i < 2; i++) {
      Serial.print("\t");
      writeDoubleToEEPROM(EEPROM_LED_ON_RATIO + (i * 4), neos[i].getOnRatio());
      Serial.print(neos[i].getOnRatio());
    }
    Serial.println();
    return 1;
  }
  return 0;
}

void printOnRatioLog() {
  Serial.print("Reading the LED on rates                :");
  for (int  i = 0; i < 2; i++) {
    Serial.print("\t");
    Serial.print(readDoubleFromEEPROM(EEPROM_LED_ON_RATIO + (i * 4)));
  }
  Serial.println();
}

#endif // __LUX_H__
