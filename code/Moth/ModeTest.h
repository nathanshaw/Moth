/*
#ifndef TEST_MODE_H
#define TEST_MODE_H

// if you are not using any of these libraries then remove from the include statements
#include <WS2812Serial.h>
#include "DLManager/DLManager.h"
#include "Configuration.h"
#include "Configuration_pitch.h"
#include "NeopixelManager/NeopixelManager.h"
#include "LuxManager/LuxManager.h"
#include "AudioEngine/AudioEngine.h"
#include <Audio.h>
#include <SerialFlash.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

///////////////// Globals /////////////////////////////////////
uint8_t red, green, blue;
uint32_t last_color_written = 0;

WS2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);
NeoGroup neos[NUM_NEO_GROUPS] = {
  NeoGroup(&leds, 0, 4, "Front", MIN_FLASH_TIME, MAX_FLASH_TIME),
  NeoGroup(&leds, 5, 9, "Rear", MIN_FLASH_TIME, MAX_FLASH_TIME)
};

// lux managers to keep track of the VEML readings
LuxManager lux_managers[NUM_LUX_SENSORS] = {
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 0, (String)"Front", &neos[0]),
  LuxManager(lux_min_reading_delay, lux_max_reading_delay, 1, (String)"Rear ", &neos[1])
};

// FeatureCollector fc[2] = {FeatureCollector("front"), FeatureCollector("rear")};

DLManager datalog_manager = DLManager((String)"Datalog Manager");

// AutoGain auto_gain[2] = {AutoGain("Song", &fc[0], &fc[1], MIN_SONG_GAIN, MAX_SONG_GAIN, MAX_GAIN_ADJUSTMENT),
//                          AutoGain("Onset", &fc[2], &fc[3], MIN_ONSET_GAIN, MAX_ONSET_GAIN, MAX_GAIN_ADJUSTMENT)
//                         };

///////////////////////////////////////////////////////////////////////////////////////////

///////////////// Audio code should appear here (created from the Teensy Audio Design Tool)

///////////////////////////////////////////////////////////////////////////////////////////

void setupAudio() {
  Serial.println("WARNING - setupAudio() is not implemented yet");
}

void linkFeatureCollector() {
  Serial.println("WARNING - linkFeatureCollector() is not implemented yet");
}

void setupDLManager() {
  Serial.println("WARNING - setupDLManager() is not implemented yet");
}

void wipeAll(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LED; i++) {
    leds.setPixel(i, r, g, b);
  }
  leds.show();
}

void handleReceivedMessage(char *b) {
  // listen for serial messages coming from the computer
  char inc_byte = *b;
  String sval = "";
  Serial.println(*b);
  Serial.println(*(b + 1));
  Serial.println(*(b + 2));
  Serial.println(*(b + 3));
  for (int i = 0; i < 3; i++) {
    if (*(b + 1 + i) != '\n') {
      sval += *(b + 1 + i);
    }
  }
  uint8_t val = sval.toInt();
  // Serial.print("val is "); Serial.println(val);
  // if the message is R 0-255 turn the neopixels red, same for green and blue
  if (inc_byte == 'r' || inc_byte == 'R') {
    red = val;
    green = 0;
    blue = 0;
    Serial.print("Setting Neos Red brightness to : "); Serial.println(red);
    last_color_written = RED;
  } else if (inc_byte == 'g' || inc_byte == 'G') {
    green = val;
    red = 0;
    blue = 0;
    last_color_written = GREEN;
    Serial.print("Setting Neos Green brightness to : "); Serial.println(green);
  } else if (inc_byte == 'b' || inc_byte == 'B') {
    blue = val;
    green = 0;
    red = 0;
    last_color_written = BLUE;
    Serial.print("Setting Neos Blue brightness to : "); Serial.println(blue);
  } else if (inc_byte == 'y' || inc_byte == 'Y') {
    blue = 0;
    green = val;
    red = val;
    last_color_written = YELLOW;
    Serial.print("Setting Neos Blue brightness to : "); Serial.println(blue);
  } else if (inc_byte == 'p' || inc_byte == 'P') {
    blue = val;
    green = 0;
    red = val;
    last_color_written = PURPLE;
    Serial.print("Setting Neos Blue brightness to : "); Serial.println(blue);
  } else if (inc_byte == 'w' || inc_byte == 'w') {
    red = green = blue = val;
    Serial.print("Setting Neos White brightness to : "); Serial.println(red);
    last_color_written = WHITE;
  } else if (inc_byte == 'l' || inc_byte == 'L') {
    wipeAll(0, 0, 0);
    delay(500);
    // if message is lux, Lux or LUX then run the lux calibration and print the results
    Serial.println("running calibration on both lux sensors");
    lux_managers[0].calibrate(3000);
    lux_managers[1].calibrate(3000);
  }  else if (inc_byte == '-') {
    switch (last_color_written) {
      case RED:
        red -= 1;
        Serial.print("red now :"); Serial.println(red);
        break;
      case GREEN:
        green -= 1;
        Serial.print("green now :"); Serial.println(green);
        break;
      case BLUE:
        blue -= 1;
        Serial.print("blue now :"); Serial.println(blue);
        break;
      case YELLOW:
        green -= 1;
        red -= 1;
        break;
      case PURPLE:
        red -= 1;
        blue -= 1;
        break;
      case WHITE:
        red -= 1;
        green -= 1;
        blue -= 1;
        break;
    }
  } else if (inc_byte == '+') {
    Serial.println("entered in a increase gain msg");
    switch (last_color_written) {
      case RED:
        red += 1;
        Serial.print("red now :"); Serial.println(red);
        break;
      case GREEN:
        green += 1;
        Serial.print("green now :"); Serial.println(green);
        break;
      case BLUE:
        blue += 1;
        Serial.print("blue now :"); Serial.println(blue);
        break;
      case YELLOW:
        green += 1;
        red += 1;
        break;
      case PURPLE:
        red += 1;
        blue += 1;
        break;
      case WHITE:
        red += 1;
        green += 1;
        blue += 1;
        break;
    }
  }
  wipeAll(red, green, blue);
}

void handleSerial()
{
  const int BUFF_SIZE = 32; // make it big enough to hold your longest command
  static char buffer[BUFF_SIZE + 1]; // +1 allows space for the null terminator
  static int length = 0; // number of characters currently in the buffer

  if (Serial.available())
  {
    char c = Serial.read();
    if ((c == '\r') || (c == '\n'))
    {
      // end-of-line received
      if (length > 0)
      {
        handleReceivedMessage(buffer);
      }
      length = 0;
    }
    else
    {
      if (length < BUFF_SIZE)
      {
        buffer[length++] = c; // append the received character to the array
        buffer[length] = 0; // append the null terminator
      }
      else
      {
        // buffer full - discard the received character
      }
    }
  }
}

void updateAutogain() {
  // Serial.println("WARNING - updateAutogain() is not implemented yet for TEST_MODE");
}

void updateMode() {
  handleSerial();
}

#endif
*/
