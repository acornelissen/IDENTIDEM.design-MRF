#include "globals.h"
#include <Arduino.h>     // For millis() for lastActivityTime initialization
#include "mrfconstants.h" // For SMOOTHING_WINDOW_SIZE if not pulled via globals.h (it is)

// Preferences
Preferences prefs;

// Variables
// ---------------------
// Lightmeter
int prev_iso = 400;
int iso = 400;
float prev_aperture;
float aperture;
float prev_lux = 0;
float lux = 0;
String shutter_speed = "...";
int iso_index = 5;
int aperture_index;

// Filter algorithm
int samples[2][SMOOTHING_WINDOW_SIZE];
int curReadIndex[2] = {0, 0};
int sampleTotal[2] = {0, 0};
int sampleAvg[2] = {0, 0};

// Lens distance
int prev_lens_sensor_reading = 0;
int lens_sensor_reading = 0;
int lens_distance_raw = 0;
String lens_distance_cm = "...";

// LiDAR distance
int prev_distance = 0;
int16_t distance = 0;
int16_t strength = 0;
int16_t temperature = 0;
String distance_cm = "...";

// Battery gauge
int prev_bat_per = 0;
int bat_per = 0;

// Camera state
String ui_mode = "main";
int config_step = 0;
int calib_step = 0;
int selected_lens = 1;
int selected_format = 3;
int calib_lens = 0;

int calib_distance_set[7] = {};
int current_calib_distance = 0;

int film_counter = 0;
int prev_encoder_value = 0;
int encoder_value = 0;
float frame_progress = 0;
float prev_frame_progress = 0;

unsigned long lastActivityTime = millis();
bool sleepMode = false;
// ---------------------