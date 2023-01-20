// WS2812B vars
#include "FastLED.h"

#define LED_TYPE WS2811 // led type
#define NUM_LEDS 256     // 290. Count of leds
#define LED_1_PIN 3       // Pin to control first ws812b leds
#define LED_2_PIN 4       // Pin to control second ws812b leds

CRGB leds_1[NUM_LEDS];
CRGB leds_2[NUM_LEDS];

byte name_1[] = {0,0,1,1,1,0,0,1,
                        0,1,0,1,0,0,0,0};
byte name_2[] = {0,0,0,0,0,0,0,0,
                        0,1,0,1,0,1,0,1};

int cur_brightness = 30;
int start_from = 0;

byte r = 0;
byte g = 0;
byte b = 0;

int colors[][3] =
{
  {255, 0, 0}, // red
  {255, 128, 0}, // orange
  {255, 255, 0}, // yellow 
  {0, 255, 0}, // green
  {0, 255, 255}, // cyan
  {0, 0, 255}, // blue
  {128, 0, 128}, // purple
};

// mode 2 vars
#define MODE_2_LENGTH_COLOR_SEGMENT 5
int mode_2_length_rainbow = 0;
int mode_2_step = 0;

const int COLOR_MODE_SPEED = 10;

unsigned long mode_timer; // timer for print mode
unsigned long global_timer; // global timer

#define GLOBAL_LOOP 2 // global timer loop

void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    leds_1[i].setRGB( cred, cgrn, cblu);
    leds_2[i].setRGB( cred, cgrn, cblu);
  }
}

void mode_color_1() {
  if (colors[start_from][0] == r && colors[start_from][1] == g && colors[start_from][2] == b) {
    start_from++;

    int colors_count = (sizeof(colors) / sizeof(colors[0]));

    if (start_from >= colors_count) {
      start_from = 0;
    }
  }

  if (colors[start_from][0] > r) {
    r++;
  } else if (colors[start_from][0] < r) {
    r--;
  }

  if (colors[start_from][1] > g) {
    g++;
  } else if (colors[start_from][1] < g) {
    g--;
  }

  if (colors[start_from][2] > b) {
    b++;
  } else if (colors[start_from][2] < b) {
    b--;
  }

  one_color_all(r, g, b);
}

// big rainbow
void mode_color_2() {
  int colors_count = (sizeof(colors) / sizeof(colors[0]));
  int color_num = start_from;

  mode_2_length_rainbow = 0;
  mode_2_length_rainbow += mode_2_step;

  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    if (++mode_2_length_rainbow >= MODE_2_LENGTH_COLOR_SEGMENT) {
      if (color_num++ >= colors_count) color_num = 0; // reset color to first if it last

      mode_2_length_rainbow = 0;
    }

    leds_1[i].setRGB( colors[color_num][0], colors[color_num][1], colors[color_num][2]);
    leds_2[i].setRGB( colors[color_num][0], colors[color_num][1], colors[color_num][2]);
  }

  if (++mode_2_step >= MODE_2_LENGTH_COLOR_SEGMENT) {
    if (++start_from > colors_count) start_from = 0; // reset start
    mode_2_step = 0;
  }
}

void setup() {
  Serial.begin(9600);

// setup led
  FastLED.setBrightness(cur_brightness);  // set leds brightness
  FastLED.addLeds<LED_TYPE, LED_1_PIN, GRB>(leds_1, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, LED_2_PIN, GRB>(leds_2, NUM_LEDS);

  // make lead black before start
  FastLED.clear();
  FastLED.show();
}

void loop() {

  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    if (name_1[i] == 1) {
      leds_1[i].setRGB( 255, 0, 0);
    }

    if (name_2[i] == 1) {
      leds_2[i].setRGB( 255, 0, 0);
    }    
  }
  
  // if (millis() - global_timer > GLOBAL_LOOP) {
  //   global_timer = millis();  // reset main timer

  //   if (millis() - mode_timer > COLOR_MODE_SPEED) {
  //     mode_timer = millis();  // reset mode timer

  //     mode_color_2();

  //     FastLED.show();  
  //   }
    
  // }

}
