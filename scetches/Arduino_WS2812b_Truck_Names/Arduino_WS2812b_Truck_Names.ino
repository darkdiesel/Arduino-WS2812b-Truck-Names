#define DEBUG_MODE true

#include <EEPROM.h>

// WS2812B vars
#include "FastLED.h"

#define LED_TYPE WS2811      // led type
#define LED_MATRIX_1_PIN 2  // Pin to cotrol first ws812b leds
#define LED_MATRIX_2_PIN 3  // Pin to cotrol second ws812b leds

#define NUM_LEDS 256  // Count of leds

CRGB leds[NUM_LEDS];

uint8_t current_brightness = 30;

// potenciometr vars
#define POT_BRIGHTNESS_PIN A0 // Pit to control brightness

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

// buttons vars
#define BTN_CHANGE_COLOR 12 // pin to control matrix change color
#define BTN_SWITCH_NAMES 11 // pin to control matrix change color

bool btn_change_color_last_state = LOW;
bool btn_change_color_current_state = LOW;

bool btn_switch_names_last_state = LOW;
bool btn_switch_names_current_state = LOW;

// for debounce buttons
bool debounce(uint8_t btn_pin, bool last) {
  bool current = digitalRead(btn_pin);

  if (last != current)
  {
    delay(5);
    current = digitalRead(btn_pin);
  }

  return current;
}

// update device settings
void updateEEPROM() {
  EEPROM.write(1, current_brightness);
  EEPROM.write(2, current_color);
}

// read device settings
void readEEPROM() {
  current_brightness = EEPROM.read(1);
  current_color = EEPROM.read(2);
  
  if (current_brightness == 0) {
    current_brightness = 30;
    updateEEPROM();
  }

  // set random figure color
  if (current_color >= collors_count) {
      current_color = 0;
  }
}

void setup() {
  if (DEBUG_MODE) {
    Serial.begin(9600);
  }

  // Read variables from memory
  readEEPROM();

  // Update variables from memory
  updateEEPROM();

  // setup led
  FastLED.setBrightness(current_brightness);  // set leds brightness
  FastLED.addLeds<LED_TYPE, LED_MATRIX_1_PIN, GRB>(leds, NUM_LEDS);

  // make lead black before start
  FastLED.clear();
  fill_solid(leds, 256, collors[current_color]);
  FastLED.show();

  // setup potentiometer
  pinMode(POT_BRIGHTNESS_PIN, INPUT);

  // setup buttons
  pinMode(BTN_CHANGE_COLOR, INPUT_PULLUP);
  pinMode(BTN_SWITCH_NAMES, INPUT_PULLUP);
}

void loop() {
  // control brightness
  int sensorValue = analogRead(POT_BRIGHTNESS_PIN);
  int new_brightness = map(sensorValue, 0, 1023, 0, 255);

  if ( (new_brightness > ( current_brightness + 2)) || (new_brightness < ( current_brightness - 2)) ) {
    if (DEBUG_MODE) {
      Serial.print("New brightness: ");
      Serial.println(new_brightness);
    }

    current_brightness = new_brightness;

    FastLED.setBrightness(current_brightness);  // set leds brightness
    FastLED.show();

    updateEEPROM();
  }

  // control color for matrix
  btn_change_color_current_state = debounce(BTN_CHANGE_COLOR, btn_change_color_last_state);

  if (btn_change_color_last_state == HIGH && btn_change_color_current_state == LOW) {
    //change color

    if (DEBUG_MODE) {
      Serial.println("Change color btn pressed");
    }

    if (++current_color >= collors_count) {
      current_color = 0;
    }

    fill_solid(leds, 256, collors[current_color]);
    FastLED.show();

    updateEEPROM();
  }

  btn_change_color_last_state = btn_change_color_current_state;


  // control name for matrix
  btn_switch_names_current_state = debounce(BTN_SWITCH_NAMES, btn_switch_names_last_state);

  if (btn_switch_names_last_state == HIGH && btn_switch_names_current_state == LOW) {
    //change color

    if (DEBUG_MODE) {
      Serial.println("Switch names btn pressed");
    }

    if (++current_color >= collors_count) {
      current_color = 0;
    }

    fill_solid(leds, 256, collors[current_color]);
    FastLED.show();

    updateEEPROM();
  }

  btn_switch_names_last_state = btn_switch_names_current_state;
}
