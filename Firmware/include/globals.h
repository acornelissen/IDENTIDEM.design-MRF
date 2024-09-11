// Preferences
Preferences prefs;


// Variables
// ---------------------
// Lightmeter
int prev_iso = 800;
int iso = 800;
float prev_aperture;
float aperture;
float prev_lux = 0;
float lux = 0;
String shutter_speed = "...";
int iso_index = 0;
int aperture_index;

// Filter algorithm
int samples[2][SMOOTHING_WINDOW_SIZE]; // the readings from the analog input
int curReadIndex[2] = {0, 0};          // the index of the current reading
int sampleTotal[2] = {0, 0};           // the running total
int sampleAvg[2] = {0, 0};

// Lens distance
int prev_lens_sensor_reading = 0;
int lens_sensor_reading = 0;
int lens_distance_raw = 0;
String lens_distance_cm = "...";

// LiDAR distance
int prev_distance = 0;
int16_t distance = 0;    // Distance to object in centimeters
int16_t strength = 0;    // Strength or quality of return signal
int16_t temperature = 0; // Internal temperature of Lidar sensor chip
String distance_cm = "...";

// Battery gauge
int prev_bat_per = 0;
int bat_per = 0;

// Camera state
String ui_mode = "main";
int config_step = 0;
int calib_step = 0;
int selected_lens = 1;
int selected_format = 0;
int calib_lens = 0;

int calib_distance_set[7] = {};
int current_calib_distance = 0;

unsigned long lastActivityTime = millis();
bool sleepMode = false;
RTC_DATA_ATTR  bool deepSleep = false;
RTC_DATA_ATTR  bool wokeUp = false;
RTC_DATA_ATTR int longPress = 0;
// ---------------------