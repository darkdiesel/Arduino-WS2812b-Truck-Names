// WS2812B vars
#include "FastLED.h"

#include <EEPROMex.h>

#define LED_TYPE WS2811 // led type
#define NUM_LEDS 256     // 290. Count of leds

#define LED_1_PIN 2       // Pin to control first ws812b leds
#define LED_2_PIN 4       // Pin to control second ws812b leds

CRGB leds[NUM_LEDS];
CLEDController* controllers[2];

byte this_name = 1;

byte name_1[] = {
                 0,0,0,0,0,0,0,0, // top
                 1,1,1,1,1,1,1,0, // bottom
                 0,1,0,0,1,0,0,1, // top
                 1,0,0,1,0,0,1,0, // bottom
                 0,1,0,0,0,0,0,1, // top
                 0,0,0,0,0,0,0,0, // bottom
                 0,1,1,1,1,1,0,0, // top
                 0,1,0,0,0,0,0,0, // bottom
                 0,0,0,0,0,0,0,1, // top
                 0,1,0,0,0,0,0,0, // bottom
                 0,1,1,1,1,1,0,0, // top
                 0,0,0,0,0,0,0,0, // bottom
                 0,0,1,1,1,1,1,0, // top
                 1,0,0,0,0,0,1,0, // bottom
                 0,1,0,0,0,1,0,1, // top
                 1,1,1,0,0,0,1,0, // bottom
                 0,0,0,0,0,0,0,0, // top
                 1,1,1,1,1,1,1,0, // bottom
                 0,1,0,0,1,0,0,1, // top
                 1,0,0,1,0,0,1,0, // bottom
                 0,1,0,0,0,0,0,1, // top
                 0,0,0,0,0,0,0,0, // bottom
                 0,1,1,1,1,1,1,1, // top
                 0,0,0,0,1,0,0,0, // bottom
                 0,0,0,0,1,0,0,0, // top
                 1,1,1,1,1,1,1,0, // bottom
                 0,0,0,0,0,0,0,0, // top
                 0,0,0,0,1,1,1,0, // bottom
                 0,0,0,0,1,1,1,1, // top
                 0,0,0,0,1,1,1,0, // bottom
                 };

byte name_2[] = {
                 0,0,0,0,0,0,0,0, // top
                 0,0,0,0,0,0,0,0, // bottom
                 0,0,0,0,0,0,0,0, // top
                 1,1,1,1,1,1,1,0, // bottom
                 0,1,0,0,1,0,0,0, // top
                 0,0,0,1,0,0,1,0, // bottom
                 0,0,1,1,0,0,0,0, // top
                 0,0,0,0,0,0,0,0, // bottom
                 
                 0,0,1,1,1,1,1,1, // top
                 0,0,0,1,0,0,1,0, // bottom
                 0,1,0,0,1,0,0,0, // top
                 1,1,1,1,1,1,0,0, // bottom
                 
                 0,0,0,0,0,0,0,0, // top
                 0,0,1,1,1,1,1,0,  // bottom
                 0,0,0,0,0,0,1,0, // top
                 1,0,0,0,0,0,0,0, // bottom
                 0,0,0,0,0,0,1,0, // top
                 0,0,1,1,1,1,1,0, // bottom
                 0,0,0,0,0,0,0,0, // top

                 1,1,1,1,1,1,1,0, // bottom
                 0,1,0,0,1,0,0,1, // top
                 1,0,0,1,0,0,1,0, // bottom
                 0,1,0,0,0,0,0,1, // top
                 0,0,0,0,0,0,0,0, // bottom

                0,1,1,1,1,1,1,1, // top
                1,0,0,0,0,0,0,0, // bottom
                0,0,0,0,0,0,0,1, // top
                1,0,0,0,0,0,0,0, // bottom
                 };

int cur_brightness = 30;
int start_from = 0;

void updateEEPROM() {
  if (this_name == 1) {
    EEPROM.updateByte(1, 2);
  } else {
    EEPROM.updateByte(1, 1);
  }  
}

void readEEPROM() {
  this_name = EEPROM.readByte(1);
}

void set_name(CLEDController* const strip, byte controller_num) {
  // clean leads
  fill_solid(strip->leds(), strip->size(), CRGB::Black);

  if (controller_num == 1) {
    if (this_name == 1) {
      for (int i = 0; i < sizeof(name_1); i++ ) {
        if (name_1[i] == 1) {
          strip->leds()[i].setRGB( 255, 0, 0);
        } else {
          strip->leds()[i].setRGB( 0, 0, 0);      
        }
      }
    } else {
      for (int i = 0; i < sizeof(name_2); i++ ) {
        if (name_2[i] == 1) {
          strip->leds()[i].setRGB( 255, 0, 0);
        } else {
          strip->leds()[i].setRGB( 0, 0, 0);      
        }
      }
    }
  } else {
    if (this_name == 1) {
      for (int i = 0; i < sizeof(name_2); i++ ) {
        if (name_2[i] == 1) {
          strip->leds()[i].setRGB( 255, 0, 0);
        } else {
          strip->leds()[i].setRGB( 0, 0, 0);      
        }
      }
    } else {
      for (int i = 0; i < sizeof(name_1); i++ ) {
        if (name_1[i] == 1) {
          strip->leds()[i].setRGB( 255, 0, 0);
        } else {
          strip->leds()[i].setRGB( 0, 0, 0);      
        }
      }   
    }
  }

  delay(1000);

  strip->showLeds();
}

void set_names(){
  for(int i = 0; i < 2; i++) {
    set_name(controllers[i], i);
  }

  updateEEPROM();
}

void setup() {
  Serial.begin(9600);

  // setup led
  FastLED.setBrightness(cur_brightness);  // set leds brightness
  
  controllers[0] = &FastLED.addLeds<LED_TYPE, LED_1_PIN, GRB>(leds, NUM_LEDS);
  controllers[1] = &FastLED.addLeds<LED_TYPE, LED_2_PIN, GRB>(leds, NUM_LEDS);

  readEEPROM();

  set_names();
}

void loop() {
  // No Loop
}