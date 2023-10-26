#define DEBUG_MODE true

#include <EEPROM.h>

// WS2812B vars
#include "FastLED.h"

#define LED_TYPE WS2811      // led type
#define LED_MATRIX_1_PIN 2  // Pin to cotrol first ws812b leds
#define LED_MATRIX_2_PIN 3  // Pin to cotrol second ws812b leds

#define NUM_LEDS 256     // Count of leds
#define NUM_LED_COLS 8 // Count of leds in one row

const uint8_t NUM_LED_ROWS = NUM_LEDS / NUM_LED_COLS; // Count of led rows 

CRGB leds[NUM_LEDS];

#define START_TOP_RIGHT 1
#define START_TOP_LEFT 0

#define LED_START_DIRECTION START_TOP_RIGHT // first point (led)

/// @param scale a 0-255 value for how much to scale all leds before writing them out
uint8_t current_brightness = 50;

// potenciometr vars
#define POT_BRIGHTNESS_PIN A0 // Pit to control brightness

#define BRIGHTNESS_VALS_COUNT 50
byte bryghtness_vals[BRIGHTNESS_VALS_COUNT];
byte bryghtness_val_index = 0; // calculate brightness count

// color vars
uint32_t colors[] = // uint32_t 0 .. 4,294,967,295
{
  CRGB::Red,
  // CRGB::Orange,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Cyan,
  CRGB::Blue,
  CRGB::Purple,
};

const byte colors_count = sizeof(colors) / sizeof(colors[0]);

byte current_color; // byte 	0..256

#define LETTER_HEIGHT 6

const byte LET_Z[][LETTER_HEIGHT] = {{1, 0, 0, 0, 0, 1},{1, 1, 0, 0, 0, 1},{1, 0, 1, 0, 0, 1},{1, 0, 0, 1, 0, 1},{1, 0, 0, 0, 1, 1}};
const byte LET_H[][LETTER_HEIGHT] = {{1, 1, 1, 1, 1, 1},{0, 0, 1, 0, 0, 0},{0, 0, 1, 0, 0, 0},{0, 0, 1, 0, 0, 0},{1, 1, 1, 1, 1, 1}};
const byte LET_E[][LETTER_HEIGHT] = {{1, 0, 0, 0, 0, 1},{1, 0, 1, 0, 0, 1},{1, 0, 1, 0, 0, 1},{1, 1, 1, 1, 1, 1}};
const byte LET_N[][LETTER_HEIGHT] = {{1, 1, 1, 1, 1, 1},{0, 0, 0, 0, 1, 0},{0, 0, 0, 1, 0, 0},{0, 0, 1, 0, 0, 0},{1, 1, 1, 1, 1, 1}};
const byte LET_I[][LETTER_HEIGHT] = {{1, 1, 1, 1, 1, 1}};
const byte LET_A[][LETTER_HEIGHT] = {{0, 0, 1, 1, 1, 1},{0, 1, 0, 1, 0, 0}, {1, 0, 0, 1, 0, 0},{0, 1, 0, 1, 0, 0},{0, 0, 1, 1, 1, 1}};
const byte LET_P[][LETTER_HEIGHT] = {{0, 1, 1, 0, 0, 0},{1, 0, 0, 1, 0, 0}, {1, 0, 0, 1, 0, 0},{1, 1, 1, 1, 1, 1}};
const byte LET_V[][LETTER_HEIGHT] = {{1, 1, 1, 1, 0, 0},{0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 1},{0, 0, 0, 0, 1, 0},{1, 1, 1, 1, 0, 0}};
const byte LET_L[][LETTER_HEIGHT] = {{0, 0, 0, 0, 0, 1},{0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 1},{1, 1, 1, 1, 1, 1}};

#define ROWS_WRAP 1
#define COLLS_WRAP 1
#define LETTER_SPACE 1

uint16_t current_position_x = ROWS_WRAP;

// buttons vars
#define BTN_CHANGE_COLOR 12 // pin to control matrix change color
#define BTN_SWITCH_NAMES 11 // pin to control matrix change color

bool btn_change_color_last_state = LOW;
bool btn_change_color_current_state = LOW;

bool btn_switch_names_last_state = LOW;
bool btn_switch_names_current_state = LOW;

// animation
bool animation_state = HIGH; // state that show animation in progress
bool animation_mode_state = HIGH; // state HIGH show that animation start displaying. LOW - finished displaying

uint16_t current_animation_position_x = 0;
uint16_t current_animation_position_y = 0;
int16_t current_animation_direction_x = 1;
int16_t current_animation_direction_y = 0;

unsigned long global_timer; // global timer
unsigned long mode_timer; // timer for animations

#define GLOBAL_LOOP 2 // global timer loop
#define COLOR_MODE_SPEED 3 // animation speed

// convert i, j coordinates to led num for ws2812b 
int get_led_num(uint8_t i = 0, uint8_t j = 0){
  switch (LED_START_DIRECTION) {
    case START_TOP_RIGHT:
      return NUM_LED_COLS * i + ((i % 2) ? j : (NUM_LED_COLS - j - 1));
      break;
    case START_TOP_LEFT:
    default:
      return 0;
      break;
  }
}

void drawChar(byte _symbol[][6], byte letter_width, uint16_t &current_position_x){
  //byte letter_width = sizeof(_symbol) / sizeof(_symbol[0]);
  
  uint16_t _x = 0;
  uint16_t _y = 0;
  
  // if (DEBUG_MODE == true) {
  //   Serial.print("letter width: ");
  //   Serial.println(sizeof(_symbol));
  //   Serial.println(sizeof(LET_Z));

  //   Serial.print("possition: ");
  //   Serial.println(current_position_x);
  // }

  for (int j = 0; j < LETTER_HEIGHT; j++) {
    for (int i = 0; i < letter_width; i++) {
      if (_symbol[i][j] == 1) {
        // rotate letter
        _x = current_position_x + (letter_width - 1) - i;
        _y = (LETTER_HEIGHT - 1) + COLLS_WRAP - j;

        // leds[get_led_num(i, j)] = colors[current_color];
        leds[get_led_num(_x, _y)] = colors[current_color];
      }
    }
  }

  current_position_x += letter_width;
}

void drawName(){
  current_position_x = ROWS_WRAP;

  FastLED.clear();

  // ZHENIA
  drawChar(LET_Z, sizeof(LET_Z) / sizeof(LET_Z[0]), current_position_x);
  current_position_x += LETTER_SPACE;
  drawChar(LET_H, sizeof(LET_H) / sizeof(LET_H[0]), current_position_x);
  current_position_x += LETTER_SPACE;
  drawChar(LET_E, sizeof(LET_E) / sizeof(LET_E[0]), current_position_x);
  current_position_x += LETTER_SPACE;
  drawChar(LET_N, sizeof(LET_N) / sizeof(LET_N[0]), current_position_x);
  current_position_x += LETTER_SPACE;
  drawChar(LET_I, sizeof(LET_I) / sizeof(LET_I[0]), current_position_x);
  current_position_x += LETTER_SPACE;
  drawChar(LET_A, sizeof(LET_A) / sizeof(LET_A[0]), current_position_x);

  // PAVEL
  // current_position_x = ROWS_WRAP + 2;

  // drawChar(LET_P, sizeof(LET_P) / sizeof(LET_P[0]), current_position_x);
  // current_position_x += LETTER_SPACE;
  // drawChar(LET_A, sizeof(LET_A) / sizeof(LET_A[0]), current_position_x);
  // current_position_x += LETTER_SPACE;
  // drawChar(LET_V, sizeof(LET_V) / sizeof(LET_V[0]), current_position_x);
  // current_position_x += LETTER_SPACE;
  // drawChar(LET_E, sizeof(LET_E) / sizeof(LET_E[0]), current_position_x);
  // current_position_x += LETTER_SPACE;
  // drawChar(LET_L, sizeof(LET_L) / sizeof(LET_L[0]), current_position_x);

  FastLED.show();
}

void animateSwitchOn() {
  if (animation_mode_state) {
    leds[get_led_num(current_animation_position_x, current_animation_position_y)] = colors[current_color];
  } else {
    leds[get_led_num(current_animation_position_x, current_animation_position_y)] = CRGB::Black;
  }

  current_animation_position_x += current_animation_direction_x;
  current_animation_position_y += current_animation_direction_y;

  if (current_animation_position_x >= NUM_LED_ROWS - 1) {
    if (current_animation_direction_y == 0) {
      current_animation_direction_y = 1;
      current_animation_direction_x = 0;
    }

    if (current_animation_position_y >= NUM_LED_COLS - 1) {
      current_animation_direction_y = 0;
      current_animation_direction_x = -1;
    }
  } else if (current_animation_position_x == 0) {
    if (current_animation_position_y >= NUM_LED_COLS - 1) {
      current_animation_direction_y = -1;
      current_animation_direction_x = 0;
    }

    if (current_animation_position_y == 0) {
      current_animation_direction_y = 0;
      current_animation_direction_x = 1;

      // change mode 
      if (!animation_mode_state) {
        animation_state = LOW;
        animation_mode_state = HIGH;

        drawName();
      } else {
        animation_mode_state = LOW;
      }
    }
  }
}

void changeCurrentColor(){
  if (++current_color >= colors_count) {
    current_color = 0;
  }
}

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

bool analogReadSave(uint8_t analog_pin, byte &bryghtness_val_index, byte* bryghtness_vals_arr) {
  int sensorValue = analogRead(analog_pin);
  int new_brightness = map(sensorValue, 0, 1023, 0, 255);

  bryghtness_vals_arr[bryghtness_val_index] = new_brightness;

  if (++bryghtness_val_index >= BRIGHTNESS_VALS_COUNT) {
    bryghtness_val_index = 0;
  }
}

byte analogReadMedium(byte* bryghtness_vals_arr) {
  int sum = 0;

  for (int i = 0; i < BRIGHTNESS_VALS_COUNT; i++) {
    sum += bryghtness_vals_arr[i];
  }

  // sum = sum>>4;
  
  return round((float)sum / (float)BRIGHTNESS_VALS_COUNT);
  //return (int)((float)sum / (float)BRIGHTNESS_VALS_COUNT) + 1;  
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
  if (current_color >= colors_count) {
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
  //fill_solid(leds, 256, colors[current_color]);

  //drawName();
  
  // FastLED.show();

  // setup potentiometer
  pinMode(POT_BRIGHTNESS_PIN, INPUT);

  // setup buttons
  pinMode(BTN_CHANGE_COLOR, INPUT_PULLUP);
  pinMode(BTN_SWITCH_NAMES, INPUT_PULLUP);
}

void loop() {
  analogReadSave(POT_BRIGHTNESS_PIN, bryghtness_val_index, bryghtness_vals);

  if (bryghtness_val_index  == (BRIGHTNESS_VALS_COUNT - 1)) {
    byte new_brightness = analogReadMedium(bryghtness_vals);

    //if ( (new_brightness > ( current_brightness + 1)) || (new_brightness < ( current_brightness - 1)) ) {
    
    if ( new_brightness != current_brightness ) {
      if (DEBUG_MODE) {
        Serial.print("New brightness: ");
        Serial.println(new_brightness);
      }

      current_brightness = new_brightness;

      FastLED.setBrightness(current_brightness);  // set leds brightness
      FastLED.show();

      updateEEPROM();
    }
  }

  // control color for matrix
  btn_change_color_current_state = debounce(BTN_CHANGE_COLOR, btn_change_color_last_state);

  if (btn_change_color_last_state == HIGH && btn_change_color_current_state == LOW) {
    //change color

    if (DEBUG_MODE) {
      Serial.println("Change color btn pressed");
    }

    // check if animation switch off and then change color
    if (!animation_state) {
      changeCurrentColor();

      // start animation
      animation_state = HIGH;

      updateEEPROM();
    }    
  }

  btn_change_color_last_state = btn_change_color_current_state;


  // control name for matrix
  btn_switch_names_current_state = debounce(BTN_SWITCH_NAMES, btn_switch_names_last_state);

  if (btn_switch_names_last_state == HIGH && btn_switch_names_current_state == LOW) {
    //change color

    if (DEBUG_MODE) {
      Serial.println("Switch names btn pressed");
    }

    updateEEPROM();
  }

  btn_switch_names_last_state = btn_switch_names_current_state;

  //check animation
  if (animation_state) {
    //if (millis() - global_timer > GLOBAL_LOOP) {
      global_timer = millis();  // reset main timer

      if (millis() - mode_timer > COLOR_MODE_SPEED) {
        mode_timer = millis();  // reset mode timer

        animateSwitchOn();
      }
    //}

    FastLED.show();
  }
}
