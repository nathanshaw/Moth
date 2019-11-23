void readJumpers() {
  Serial.println("reading jumpers");
  pinMode(JMP1_PIN, INPUT);
  pinMode(JMP2_PIN, INPUT);
  pinMode(JMP3_PIN, INPUT);
  pinMode(JMP4_PIN, INPUT);
  pinMode(JMP5_PIN, INPUT);
  pinMode(JMP6_PIN, INPUT);

  delay(100);

  cicada_mode = digitalRead(JMP1_PIN);
  Serial.print("cicada_mode set to : "); Serial.println(cicada_mode);
  stereo_audio = digitalRead(JMP2_PIN);
  num_channels = stereo_audio + 1; // the number of channels we will be using
  Serial.print("stereo_audio set to : "); Serial.println(stereo_audio);
  INDEPENDENT_CLICKS = digitalRead(JMP3_PIN);
  // both_lux_sensors = digitalRead(JMP3_PIN);
  // num_lux_sensors = both_lux_sensors + 1;
  Serial.print("independent clicks set to : "); Serial.println(INDEPENDENT_CLICKS);
  combine_lux_readings = digitalRead(JMP4_PIN);
  Serial.print("combine_lux_readings set to : "); Serial.println(combine_lux_readings);
  gain_adjust_active = digitalRead(JMP5_PIN);
  Serial.print("gain_adjust_active set to : "); Serial.println(gain_adjust_active);

  data_logging_active = digitalRead(JMP6_PIN);
  Serial.print("data_logging_active set to : "); Serial.println(data_logging_active);
  Serial.println("\n------------------------------------");
}
