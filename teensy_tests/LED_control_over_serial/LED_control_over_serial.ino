/*
 * For testing the NEOpixel lighting for the Moth Bot
 * 
 * Takes in LED control messages over USB to control a NeoPixel strip
 * 
 * LEDs 0-3 are the "front"
 * LEDs 4-7 are the "rear"
 * 
 */

#include <Adafruit_NeoPixel.h>
#define LED_PIN    6
#define LED_COUNT 8
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int resetTimer = 0;
unsigned long clickTime = 0;
int clickLength = 200;
int click_it = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  colorWipe(strip.Color(0, 250, 250), 0);
  delay(1000);
  colorWipe(strip.Color(0, 0, 0), 0);
  delay(1000);
}

void clickIt(int intensity) {
  strip.setBrightness(intensity);
  colorWipe(strip.Color(250, 250, 250), 0);
  click_it = 1;
  clickTime = millis();
}

void ampFollow(int intensity) {
  strip.setBrightness(intensity);
  colorWipe(strip.Color(250, 0, 0), 0);
}

void loop() {
  // if click light on and it has been longer than clickLength
  if (clickIt && millis() > clickTime + clickLength) {
    click_it = 0;
    colorWipe(strip.Color(250, 0, 0), 0);
  }
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    // click it command is denoted by a 1, followed by byte which corresponds to the brightness
    int cmd_type = Serial.read();
    Serial.println(cmd_type);
    if (cmd_type == '1') { // 49 is "1"
      clickIt(Serial.read());
    } // ampFollow command is denoted by a 2, followed by a byte which corresponds to the brightness 
    else if ( cmd_type == '2' && click_it == 0) { // 50 is "2"
      ampFollow(Serial.read());
    }
  }
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
