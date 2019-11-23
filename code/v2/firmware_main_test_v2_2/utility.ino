void printDouble( double val, unsigned int precision) {
  // prints val with number of decimal places determine by precision
  // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
  // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  Serial.print("."); // print the decimal point
  unsigned int frac;
  if (val >= 0)
    frac = (val - int(val)) * precision;
  else
    frac = (int(val) - val ) * precision;
  int frac1 = frac;
  while ( frac1 /= 10 )
    precision /= 10;
  precision /= 10;
  while (  precision /= 10)
    Serial.print("0");

  Serial.println(frac, DEC) ;
}

///////////////////////////////////////////////////////////////////////
//                    General Purpose Audio Functions
///////////////////////////////////////////////////////////////////////

void checkAudioUsage() {
  // TODO instead perhaps log the audio usage...
  if (last_usage_print > AUDIO_USAGE_POLL_RATE) {
    uint8_t use = AudioMemoryUsageMax();
    if (use > audio_usage_max) {
      audio_usage_max = use;
      writeAudioUsageToEEPROM(use);
      Serial.print("memory usage: ");
      Serial.print(use);
      Serial.print(" out of ");
      Serial.println(AUDIO_MEMORY);
    }
    last_usage_print = 0;
  }
}


///////////////////////////////////////////////////////////////////////
//                       TCA9532A I2C bus expander
///////////////////////////////////////////////////////////////////////
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
