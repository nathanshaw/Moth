#ifndef __LUX_H__
#define __LUX_H__

#include "Adafruit_VEML7700.h"
#include <Wire.h>
#include "Configuration.h"
#include "Neos.h"

// TODO add code so if the TCA is not available things are cool... also add firmware #define to control this
#define TCAADDR 0x70

class Lux {
    // initalises its own lux sensors and then handles the readings
  public:
    Lux(long minrt, long maxrt, int tca, String _name, NeoGroup *n);
    double getLux() {
      return lux;
    };
    void startSensor(byte g, byte r);
    double forceLuxReading();
    void calibrate(long len, bool first_time);
    void update();

    void resetMinMax();

    double getMinLux() {
      return min_reading;
    };
    double getMaxLux() {
      return max_reading;
    };

  private:
    Adafruit_VEML7700 sensor = Adafruit_VEML7700();

    int tca_addr;

    NeoGroup *neo;
    String id = "";

    double min_reading = 9999.9;
    double max_reading = 0.0;
    void updateMinMax();

    double past_readings[10];
    double lux;
    void readLux();

    unsigned long min_reading_time;
    unsigned long max_reading_time;

    elapsedMillis last_reading;
    long polling_rate;

    // for datalogging and such
    double lux_total;
    double lux_readings;

    // for brightness
    double brightness_scaler = 0.0;
    double calculateBrightnessScaler();

    double checkForLuxOverValue();

    double read();
};

//////////////////////////// lux and stuff /////////////////////////

Lux::Lux (long minrt, long maxrt, int tca, String _name, NeoGroup *n) {
  tca_addr = tca;
  id = _name;
  neo = n;
  min_reading_time = minrt;
  max_reading_time = maxrt;
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void Lux::startSensor(byte g, byte r) {
  Wire.begin();
  if (tca_addr > -1) {
    tcaselect(tca_addr);
  }
  if (!sensor.begin()) {
    Serial.print("ERROR ---- VEML "); Serial.print(id); Serial.println(" not found");
    neo->colorWipe(255, 100, 0);
    unsigned long then = millis();
    while (millis() < then + 5000) {
      Serial.print(".");
      delay(100);
    }
  }
  else {
    Serial.print("VEML "); Serial.print(id); Serial.println(" found");
    sensor.setGain(g);
    sensor.setIntegrationTime(r);// 800ms was default
  }
}

double Lux::checkForLuxOverValue() {
  // sometimes the sensor will give an incorrect extremely high reading, this compensates for this...
  if (lux > 100000) {
    dprintln(PRINT_LUX_DEBUG, "lux "); dprintln(PRINT_LUX_DEBUG, id); dprintln(PRINT_LUX_DEBUG, " reading error: ");
    dprintln(PRINT_LUX_DEBUG, (String)lux);
    // take the reading again
    if (SMOOTH_LUX_READINGS && lux != 0) {
      lux = (lux + sensor.readLux()) * 0.5;
      lux_total += lux;
      lux_readings++;
    } else {
      lux = sensor.readLux();
      lux_total += lux;
      lux_readings++;
    }
  }
  return lux;
}

void Lux::readLux() {
  dprintln(PRINT_LUX_DEBUG, millis()); dprint(PRINT_LUX_DEBUG, "readLux("); dprint(PRINT_LUX_DEBUG, id); dprintln(PRINT_LUX_DEBUG, ")");
  if (tca_addr > -1) {
    tcaselect(tca_addr);
  }
  double temp  = sensor.readLux();
  if (temp > 10000) {
    return;
  }
  if (SMOOTH_LUX_READINGS && lux != 0) {
      lux = (lux + temp) * 0.5;
      checkForLuxOverValue();
      lux_total += lux;
      lux_readings++;
  } else {
    lux = temp;
    checkForLuxOverValue();
    lux_total += lux;
    lux_readings++;
  }
  dprint(PRINT_LUX_READINGS, "lux: "); dprint(PRINT_LUX_READINGS, id); dprintTab(PRINT_LUX_READINGS);
  dprint(PRINT_LUX_READINGS, lux); dprintTab(PRINT_LUX_READINGS);
  // dprint(PRINT_LUX_READINGS, "lux_readings "); dprint(PRINT_LUX_READINGS, id);
  // dprint(PRINT_LUX_READINGS, " increased to : "); dprintln(PRINT_LUX_READINGS, lux_readings);
  // update the brightness scales TODO , this logic does not work if the number of lux sensors is less than the number of groups
  // todo have the brightness scaler mapping
  brightness_scaler = calculateBrightnessScaler();
  neo->setBrightnessScaler(brightness_scaler);
  dprint(PRINT_LUX_READINGS, "\tbs: "); dprintln(PRINT_LUX_READINGS, brightness_scaler);
  updateMinMax();
  last_reading = 0;
}


double Lux::calculateBrightnessScaler() {
  // todo need to make this function better... linear mapping does not really work, need to map li
  double t = constrain(lux, LOW_LUX_THRESHOLD, HIGH_LUX_THRESHOLD);
  double bs = map(t, LOW_LUX_THRESHOLD, HIGH_LUX_THRESHOLD, BRIGHTNESS_SCALER_MIN * 1000000, 1000000) / (500000);
  return bs;
}

void Lux::updateMinMax() {
  if (lux < min_reading && lux > 0.0) {
    min_reading = lux;
  } else if (lux > max_reading && lux < 10000) {
    max_reading = lux;
  }
}

void Lux::resetMinMax() {
  min_reading = 10000;
  max_reading = 0;
}
// todo move me to the correct place
void Lux::calibrate(long len, bool first_time = true) {
  // todo change this function so it takes the average of these readings
  Serial.println("------------------------");
  Serial.println("Starting Lux Calibration");
  double lux_tot = 0.0;
  for (int i = 0; i < 10; i++) {
    Serial.print(i);
    printTab();
    delay(len / 10);
    forceLuxReading(); // todo change this to not be hard coded
    if (first_time) {
      Serial.print(lux);
    }
    lux_tot += lux;
    // when we have the first 10 readings
  }
  Serial.print("\nAverage lux readings : ");
  lux = lux_tot / 10.0;
  Serial.print(lux);
  Serial.println();
  if (first_time) {
    lux_total = 0;
    lux_readings = 0;
  }
  Serial.println("\nLux calibration finished");
  Serial.println("------------------------\n");
}

double Lux::forceLuxReading() {
  readLux();
  return lux;
}

void Lux::update() {
  if (neo->getLedsOn() == false && neo->getOnOffLen() >= neo->getShdnLen() && last_reading > min_reading_time) {
    readLux();
    // dprintln(PRINT_LUX_DEBUG, lux);
  } else if (last_reading > max_reading_time && neo->getLedsOn() == true) {
    neo->shutdown(LUX_SHDN_LEN);
    dprintln(PRINT_LUX_DEBUG, "Sending lux shutdown message to Neos");
  }
}

#endif // __LUX_H__
