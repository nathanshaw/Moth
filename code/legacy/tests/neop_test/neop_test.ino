/*
 * This program acts as a sanity sketch for testing the NeoPixels using Paul's NeoP Library
 * 
 * This sketch is assuming that you will be using the Teensy with the NeoP signal connected to pin #5
 * Is also assumes you are using the "Moth" hardware configuration wherein the strip contains 
 * 12x pixels, with the first six pixels on the front of the board and the remaining six pixels on the rear of the board.
*/

#include <WS2812Serial.h>

const int numled = 40;
const int pin = 5;

// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33

byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF

// Less intense...
/*
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
*/

uint32_t ittr = 0;

void setup() {
  leds.begin();
  leds.setBrightness(20);
  Serial.begin(57600);
}

void loop() {
  // change all the LEDs in 1.5 seconds
  Serial.println(ittr++);
  int microsec = 1500000 / leds.numPixels();
  colorWipe(RED, microsec);
  colorWipe(GREEN, microsec);
  colorWipe(BLUE, microsec);
  colorWipe(YELLOW, microsec);
  colorWipe(PINK, microsec);
  colorWipe(ORANGE, microsec);
  colorWipe(WHITE, microsec);
}

void colorWipe(int color, int wait) {
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  } 
}
