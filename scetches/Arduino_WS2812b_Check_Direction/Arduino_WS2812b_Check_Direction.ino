// WS2812B vars
#include "FastLED.h"

#define LED_TYPE WS2811      // led type
#define LED_MATRIX_PIN 2  // Pin to cotrol first ws812b leds

#define NUM_LEDS 256     // Count of leds
#define NUM_LED_COLS 8 // Count of leds in one row

const uint8_t NUM_LED_ROWS = NUM_LEDS / NUM_LED_COLS; // Count of led rows 

CRGB leds[NUM_LEDS];

#define START_TOP_RIGHT 1
#define START_TOP_LEFT 0

#define LED_START_DIRECTION START_TOP_RIGHT // first point (led)

/// @param scale a 0-255 value for how much to scale all leds before writing them out
uint8_t current_brightness = 50;

// color vars
uint32_t collors[] = // uint32_t 0 .. 4,294,967,295
{
  CRGB::Red,
  CRGB::Orange,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Cyan,
  CRGB::Blue,
  CRGB::Purple,
};

const byte collors_count = sizeof(collors) / sizeof(collors[0]);

byte current_color; // byte 	0..256
int start = 0;

void setup() {
  // setup led
  FastLED.setBrightness(current_brightness);  // set leds brightness
  FastLED.addLeds<LED_TYPE, LED_MATRIX_PIN, GRB>(leds, NUM_LEDS);

  // make lead black before start
  FastLED.clear();
  //fill_solid(leds, 256, collors[current_color]);

  FastLED.show();

    // generate seed start value for random random. Get value from empty analog PIN
  randomSeed(analogRead(A3));

  current_color = random(collors_count);
}

void loop() {
  leds[start++] = collors[current_color];
  FastLED.show();
  delay(100);
  
}
