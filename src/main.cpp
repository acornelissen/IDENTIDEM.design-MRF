#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX1704X.h>
#include <TFMPlus.h>
#include <BH1750.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <ESP32Encoder.h>
#include <Preferences.h>

// Preferences
Preferences prefs;

// Constants
#define RXD2 RX
#define TXD2 TX

#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define SCREEN_HEIGHT_EXT 32    // OLED display height, in pixels
#define OLED_RESET -1           // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D     ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS_EXT 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

const int SMOOTHING_WINDOW_SIZE = 25;

#define CIRCLE_X 33
#define CIRCLE_Y 82
#define CIRCLE_X_MAX 48
#define CIRCLE_Y_MAX 92
#define CIRCLE_X_CAP 43
#define CIRCLE_Y_CAP 84
#define CIRCLE_MAX_DIST 800
#define CLOSE_FOCUS 100

const int ISOS[] = {50, 80, 100, 125, 200, 400, 500, 640, 800, 1600, 3200, 6400};
const float CALIB_DISTANCES[] = {1, 1.2, 1.5, 2, 3, 5, 10};

// Structs
struct Lens
{
  int id;
  String name;
  int sensor_reading[7];
  float distance[7];
  float apertures[9];
  bool calibrated;
};

struct FilmFormat
{
  int id;
  String name;
  int sensor[22];
  int frame[22];
};

struct ReticlePosition
{
  int x;
  int y;
};

// Lens and film format definitions
Lens lenses[] = {
    {5063, "50/6.3", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {0, 6.3, 8, 11, 16, 22, 32, 0, 0}, false},
    {6563, "65/6.3", {270, 256, 243, 231, 219, 210, 203}, {1, 1.2, 1.5, 2, 3, 5, 10}, {0, 6.3, 8, 11, 16, 22, 32, 0, 0}, true},
    {7556, "75/5.6", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {0, 5.6, 8, 11, 16, 22, 32, 45, 0}, false},
    {9035, "90/3.5", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {3.5, 4, 5.6, 8, 11, 16, 22, 32, 0}, false},
    {10035, "100/3.5", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {3.5, 4, 5.6, 8, 11, 16, 22, 32, 0}, false},
    {10028, "100/2.8", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {2.8, 4, 5.6, 8, 11, 16, 22, 32, 0}, false},
    {12747, "127/4.7", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {4.7, 5.6, 8, 11, 16, 22, 32, 45, 64}, false},
    {15056, "150/5.6", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {5.6, 8, 11, 16, 22, 32, 45, 0, 0}, false},
    {25005, "250/5.0", {0, 0, 0, 0, 0, 0, 0}, {1, 1.2, 1.5, 2, 3, 5, 10}, {5, 8, 11, 16, 22, 32, 45, 0, 0}, false}};
FilmFormat film_formats[] = {
    {35, "PANO", {0, 35, 65, 95, 125, 155, 185, 215, 245, 275, 305, 335, 365, 395, 425, 455, 485, 515, 545, 575, 605, 700}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 99}},
    {645, "6x4.5", {0, 130, 157, 183, 208, 232, 255, 277, 298, 318, 337, 355, 372, 388, 403, 550}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 99}},
    {66, "6x6", {0, 135, 165, 194, 222, 249, 275, 300, 324, 347, 367, 386, 404, 550}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 99}},
    {67, "6x7", {0, 137, 171, 204, 236, 267, 297, 326, 354, 381, 407, 550}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 99}},
};

// Variables
// ---------------------

// Lightmeter
int prev_iso = 400;
int iso = 400;
float prev_aperture;
float aperture;
float prev_lux = 0;
float lux = 0;
String shutter_speed = "1/500";
int iso_index = 5;
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
int selected_format = 3;
int calib_lens = 0;

int calib_distance_set[7] = {};
int current_calib_distance = 0;

int film_counter = 0;
int prev_encoder_value = 0;
int encoder_value = 0;
float frame_progress = 0;

unsigned long lastActivityTime = millis();
bool lowPowerMode = false;
// ---------------------

// Hardware init
// ---------------------
// Inputs
ESP32Encoder encoder;
Bounce2::Button lbutton = Bounce2::Button();
Bounce2::Button rbutton = Bounce2::Button();

// Battery gauge
Adafruit_MAX17048 maxlipo;

// Lightmeter
BH1750 lightMeter;
const int K = 20;

// LiDAR setup
TFMPlus tfluna;
HardwareSerial lidarSerial(2); // Using serial port 2

// Display setup
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display_ext(SCREEN_WIDTH, SCREEN_HEIGHT_EXT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2;
U8G2_FOR_ADAFRUIT_GFX u8g2_ext;
// ---------------------

// Helper functions
// ---------------------
float getFirstNonZeroAperture()
{
  for (int i = 0; i < sizeof(lenses[selected_lens].apertures) / sizeof(lenses[selected_lens].apertures[0]); i++)
  {
    if (lenses[selected_lens].apertures[i] != 0)
    {
      return i;
    }
  }
  return -1;
}

void loadPrefs()
{
  prefs.begin("mrf", false);
  iso = prefs.getInt("iso", 400);
  iso_index = prefs.getInt("iso_index", 5);

  byte tempLenses[sizeof(lenses)];
  prefs.getBytes("lenses", tempLenses, sizeof(lenses));
  memcpy(lenses, tempLenses, sizeof(lenses));

  selected_format = prefs.getInt("selected_format", 3);
  selected_lens = prefs.getInt("selected_lens", 1);

  int non_zero_aperture_index = getFirstNonZeroAperture();

  aperture = prefs.getFloat("aperture", lenses[selected_lens].apertures[non_zero_aperture_index]);
  aperture_index = prefs.getInt("aperture_index", non_zero_aperture_index);

  film_counter = prefs.getInt("film_counter", 0);
  encoder_value = prefs.getInt("encoder_value", 0);

  prefs.end();
}

void savePrefs()
{
  prefs.begin("mrf", false);
  prefs.putInt("iso", iso);
  prefs.putInt("iso_index", iso_index);
  prefs.putFloat("aperture", aperture);
  prefs.putInt("aperture_index", aperture_index);
  prefs.putInt("selected_format", selected_format);
  prefs.putInt("selected_lens", selected_lens);
  prefs.putInt("film_counter", film_counter);
  prefs.putInt("encoder_value", encoder_value);
  prefs.putBytes("lenses", (byte *)lenses, sizeof(lenses));
  prefs.end();
}

String cmToReadable(int cm)
{
  if (cm < 100)
  {
    return String(cm) + "cm";
  }
  else
  {
    return String(float(cm) / 100, 2) + "m";
  }
}

int calcMovingAvg(int index, int sensorVal)
{
  int readIndex = curReadIndex[index];
  sampleTotal[index] = sampleTotal[index] - (samples[index][readIndex]);

  samples[index][readIndex] = sensorVal;
  sampleTotal[index] = sampleTotal[index] + samples[index][readIndex];
  curReadIndex[index] = curReadIndex[index] + 1;

  if (curReadIndex[index] >= SMOOTHING_WINDOW_SIZE)
  {
    curReadIndex[index] = 0;
  }

  sampleAvg[index] = sampleTotal[index] / SMOOTHING_WINDOW_SIZE;
  return sampleAvg[index];
}

int_fast16_t getFocusRadius()
{
  int minRadius = 2;
  int maxRadius = 30;

  int radius = min(maxRadius, max(minRadius, abs(distance - lens_distance_raw)));

  return radius;
}

ReticlePosition calculateReticlePosition(float distance)
{

  if (distance < CLOSE_FOCUS)
  {
    distance = CLOSE_FOCUS;
  }

  float ratio = min((distance - CLOSE_FOCUS) / (CIRCLE_MAX_DIST - CLOSE_FOCUS), 1.0f);

  // Interpolate the new positions
  int new_x = int(CIRCLE_X + (CIRCLE_X_MAX - CIRCLE_X) * (ratio * 3));
  int new_y = CIRCLE_Y;

  if (new_x > CIRCLE_X_CAP)
  {
    new_x = CIRCLE_X_CAP;
  }
  if (new_y > CIRCLE_Y_CAP)
  {
    new_y = CIRCLE_Y_CAP;
  }

  ReticlePosition reticlePosition;
  reticlePosition.x = new_x;
  reticlePosition.y = new_y;

  return reticlePosition;
}
// ---------------------

// Functions to cycle values
// ---------------------
void cycleApertures(String direction)
{

  if (direction == "up")
  {
    aperture_index++;
    if (aperture_index >= sizeof(lenses[selected_lens].apertures) / sizeof(lenses[selected_lens].apertures[0]))
    {
      aperture_index = 0;
    }
    if (lenses[selected_lens].apertures[aperture_index] == 0)
    {
      cycleApertures("up");
    }
  }
  else if (direction == "down")
  {
    aperture_index--;
    if (aperture_index < 0)
    {
      aperture_index = sizeof(lenses[selected_lens].apertures) / sizeof(lenses[selected_lens].apertures[0]) - 1;
    }
    if (lenses[selected_lens].apertures[aperture_index] == 0)
    {
      cycleApertures("down");
    }
  }

  aperture = lenses[selected_lens].apertures[aperture_index];

  savePrefs();
}

void cycleISOs()
{
  iso_index++;
  if (iso_index >= sizeof(ISOS) / sizeof(ISOS[0]))
  {
    iso_index = 0;
  }

  iso = ISOS[iso_index];
  savePrefs();
}

void cycleLenses()
{
  selected_lens++;
  if (selected_lens >= sizeof(lenses) / sizeof(lenses[0]))
  {
    selected_lens = 0;
  }
  while (!lenses[selected_lens].calibrated)
  {
    selected_lens++;
    if (selected_lens >= sizeof(lenses) / sizeof(lenses[0]))
    {
      selected_lens = 0;
    }
  }

  savePrefs();
}

void cycleCalibLenses()
{
  calib_lens++;
  if (calib_lens >= sizeof(lenses) / sizeof(lenses[0]))
  {
    calib_lens = 0;
  }
  savePrefs();
}

void cycleFormats()
{
  selected_format++;
  if (selected_format >= sizeof(film_formats) / sizeof(film_formats[0]))
  {
    selected_format = 0;
  }
  savePrefs();
}
// ---------------------

// Functions to read values from sensors and set variables
// ---------------------
void setDistance()
{
  if (tfluna.getData(distance, strength, temperature))
  { // Get data from Lidar
    if (distance != prev_distance)
    {
      distance = calcMovingAvg(1, distance);
      prev_distance = distance;
      distance_cm = cmToReadable(distance);
    }
  }
}

// Borrows moving average code from
// https://github.com/makeabilitylab/arduino/blob/master/Filters/MovingAverageFilter/MovingAverageFilter.ino
int getLensSensorReading()
{
  int sensorVal = analogRead(A1);
  return calcMovingAvg(0, sensorVal) / 10;
}

void setLensDistance()
{

  lens_sensor_reading = getLensSensorReading();

  if (lens_sensor_reading != prev_lens_sensor_reading)
  {
    lastActivityTime = millis();
    prev_lens_sensor_reading = lens_sensor_reading;

    for (int i = 0; i < sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]); i++)
    {
      if (lens_sensor_reading > lenses[selected_lens].sensor_reading[0])
      {
        lens_distance_raw = lenses[selected_lens].distance[0] * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
      }
      else if (lens_sensor_reading < lenses[selected_lens].sensor_reading[sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]) - 1])
      {
        lens_distance_raw = 9999999;
        lens_distance_cm = "Inf.";
      }
      if (lens_sensor_reading == lenses[selected_lens].sensor_reading[i])
      {
        lens_distance_raw = lenses[selected_lens].distance[i] * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
      }
      else if (lens_sensor_reading < lenses[selected_lens].sensor_reading[i] && lens_sensor_reading > lenses[selected_lens].sensor_reading[i + 1])
      {
        float distance = lenses[selected_lens].distance[i] + (lens_sensor_reading - lenses[selected_lens].sensor_reading[i]) * (lenses[selected_lens].distance[i + 1] - lenses[selected_lens].distance[i]) / (lenses[selected_lens].sensor_reading[i + 1] - lenses[selected_lens].sensor_reading[i]);
        lens_distance_raw = distance * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
      }
    }
    Serial.println(lens_distance_raw);
  }
}

void setFilmCounter()
{
  if (encoder.getCount() > prev_encoder_value)
  {
    lastActivityTime = millis();
    encoder_value = encoder.getCount();
    if (encoder_value != prev_encoder_value)
    {
      prev_encoder_value = encoder_value;

      for (int i = 0; i < sizeof(film_formats[selected_format].sensor) / sizeof(film_formats[selected_format].sensor[0]); i++)
      {
        if (film_formats[selected_format].sensor[i] == encoder_value)
        {
          film_counter = film_formats[selected_format].frame[i];
          frame_progress = 0;
        }
        else if (film_formats[selected_format].sensor[i] < encoder_value && encoder_value < film_formats[selected_format].sensor[i + 1])
        {
          film_counter = film_formats[selected_format].frame[i];
          frame_progress = static_cast<float>(encoder_value - film_formats[selected_format].sensor[i]) / (film_formats[selected_format].sensor[i + 1] - film_formats[selected_format].sensor[i]);
        }
      }
      savePrefs();
    }
  }
}

void setVoltage()
{

  bat_per = maxlipo.cellPercent();
  if (bat_per > 100)
  {
    bat_per = 100;
  }

  if (bat_per != prev_bat_per)
  {
    prev_bat_per = bat_per;
  }
}

void setLightMeter()
{
  lux = lightMeter.readLightLevel();

  if (lux != prev_lux)
  {
    prev_lux = lux;
    if (lux <= 0)
    {
      shutter_speed = "Dark!";
    }
    else
    {

      if (aperture == 0)
      {
        cycleApertures("up");
      }

      float speed = round(((aperture * aperture) * K) / (lux * iso) * 1000.0) / 1000.0;

      struct SpeedRange
      {
        float lower;
        float upper;
        const char *print_speed_range;
      };

      SpeedRange speed_ranges[] = {
          {0.001, 0.002, "1/1000"},
          {0.002, 0.004, "1/500"},
          {0.004, 0.008, "1/250"},
          {0.008, 0.016, "1/125"},
          {0.016, 0.033, "1/60"},
          {0.033, 0.066, "1/30"},
          {0.066, 0.125, "1/15"},
          {0.125, 0.250, "1/8"},
          {0.250, 0.500, "1/4"},
          {0.500, 1, "1/2"}};

      char print_speed[10];
      dtostrf(speed, 4, 1, print_speed);

      for (int i = 0; i < sizeof(speed_ranges) / sizeof(speed_ranges[0]); i++)
      {
        if (speed_ranges[i].lower <= speed && speed < speed_ranges[i].upper)
        {
          strcpy(print_speed, speed_ranges[i].print_speed_range);
          break;
        }
      }

      shutter_speed = print_speed;
      if (speed >= 0.500)
      {
        shutter_speed = String(print_speed) + "s";
      }
    }
  }
}
// ---------------------

// Functions to draw UI
// ---------------------
void drawMainUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(BLACK);
  u8g2.setBackgroundColor(WHITE);
  u8g2.setFont(u8g2_font_4x6_mf);
  display.fillRect(0, 7, 64, 31, WHITE);
  display.drawLine(0, 17, 64, 17, BLACK);
  display.drawLine(0, 27, 64, 27, BLACK);
  display.drawLine(31, 0, 31, 40, BLACK);
  u8g2.setCursor(3, 15);
  u8g2.print(bat_per);
  u8g2.print(F("%"));
  u8g2.setCursor(35, 15);
  u8g2.print(F("ISO"));
  u8g2.print(iso);
  u8g2.setCursor(3, 25);
  u8g2.print(F("f"));
  if (aperture == static_cast<int>(aperture))
  {
    u8g2.print(static_cast<int>(aperture));
  }
  else
  {
    u8g2.print(aperture, 1);
  }
  u8g2.setCursor(34, 25);
  u8g2.print(shutter_speed);
  u8g2.setCursor(3, 35);
  u8g2.print(F("D:"));
  u8g2.print(distance_cm);
  u8g2.setCursor(34, 35);
  u8g2.print(F("L:"));
  u8g2.print(lens_distance_cm);

  ReticlePosition reticlePosition = calculateReticlePosition(distance);

  display.drawRect(-18, 42, 99, 85, WHITE);
  display.fillCircle(reticlePosition.x, reticlePosition.y, 2, WHITE);
  display.drawCircle(reticlePosition.x, reticlePosition.y, getFocusRadius(), WHITE);

  display.display();
}

void drawConfigUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  u8g2.setFont(u8g2_font_9x15_mf);
  u8g2.setCursor(3, 15);
  u8g2.print(F("Setup"));

  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.setCursor(3, 26);

  if (config_step == 0)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.print(F(" ISO:"));
  u8g2.print(iso);
  u8g2.print(F(" "));

  if (config_step == 1)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 37);
  u8g2.print(F(" Format:"));
  u8g2.print(film_formats[selected_format].name);
  u8g2.print(F(" "));

  if (config_step == 2)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 48);
  u8g2.print(F(" Lens:"));
  u8g2.print(lenses[selected_lens].name);
  u8g2.print(F(" "));

  if (config_step == 3)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 59);
  u8g2.print(F(" Lens Calib. > "));

  if (config_step == 4)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 70);
  u8g2.print(F(" Reset count >> "));

  if (config_step == 5)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 81);
  u8g2.print(F(" Exit >> "));

  display.display();
}

void drawCalibUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.setCursor(3, 15);
  u8g2.print(F("Calibrate"));

  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.setCursor(3, 35);

  if (calib_step == 0)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.print(F(" Lens:"));
  u8g2.print(lenses[calib_lens].name);
  u8g2.print(F(" "));

  if (calib_step == 1)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 47);

  u8g2.print(F(" "));
  u8g2.print(CALIB_DISTANCES[current_calib_distance], 1);
  u8g2.print(F("m: "));
  u8g2.print(getLensSensorReading());
  u8g2.print(F(" "));

  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);

  if (calib_step == 0)
  {
    u8g2.setCursor(3, 70);
    u8g2.print(F(" (L) to Cycle"));
    u8g2.setCursor(3, 81);
    u8g2.print(F(" (R) to Select"));
  }
  else
  {
    u8g2.setCursor(3, 70);
    u8g2.print(F(" (L) to Select"));
    u8g2.setCursor(3, 81);
    u8g2.print(F("(R) to Cancel"));
  }

  display.display();
}

void drawExternalUI()
{

  int progessBarWidth = 90;
  int progressBarHeight = 17;
  int progressBarX = 34;
  int progressBarY = 15;

  display_ext.clearDisplay();

  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(BLACK);
  u8g2_ext.setBackgroundColor(WHITE);
  u8g2_ext.setFont(u8g2_font_6x10_mf);

  u8g2_ext.setCursor(2, 8);
  display_ext.fillRect(0, 0, 128, 10, WHITE);
  u8g2_ext.print(film_formats[selected_format].name);

  display_ext.drawLine(33, 0, 33, 10, BLACK);

  u8g2_ext.setCursor(37, 8);
  u8g2_ext.print(lenses[selected_lens].name);

  if (bat_per == 100)
  {
    display_ext.drawLine(100, 0, 100, 10, BLACK);
    u8g2_ext.setCursor(104, 8);
  }
  else if (bat_per < 10)
  {
    display_ext.drawLine(111, 0, 111, 10, BLACK);
    u8g2_ext.setCursor(115, 8);
  }
  else
  {
    display_ext.drawLine(103, 0, 103, 10, BLACK);
    u8g2_ext.setCursor(107, 8);
  }

  u8g2_ext.print(bat_per);
  u8g2_ext.print(F("%"));

  u8g2_ext.setCursor(8, 30);

  if (frame_progress > 0)
  {
    float progressPercentage = frame_progress * 100;
    int progressWidth = progessBarWidth * (progressPercentage / 100);

    display_ext.drawRect(progressBarX, progressBarY, progessBarWidth, progressBarHeight, WHITE);
    display_ext.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, WHITE);
  }
  else
  {
    u8g2_ext.setCursor(60, 30);
  }

  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf);
  if (film_counter == 0 && frame_progress == 0)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Load film."));
  }
  else if (film_counter == 99)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Roll end."));
  }
  else
  {
    u8g2_ext.print(film_counter);
  }

  display_ext.display();
}
// ---------------------

// Functions to check and act on button presses
// ---------------------
void checkButtons()
{
  lbutton.update();
  if (lbutton.pressed())
  {
    lastActivityTime = millis();
    if (ui_mode == "main")
    {
      cycleApertures("down");
    }
    else if (ui_mode == "config")
    {
      config_step++;
      if (config_step > 5)
      {
        config_step = 0;
      }
    }
    else if (ui_mode == "calib")
    {
      if (calib_step == 0)
      {
        cycleCalibLenses();
      }
      if (calib_step == 1)
      {
        calib_distance_set[current_calib_distance] = getLensSensorReading();
        current_calib_distance++;
        if (current_calib_distance >= sizeof(CALIB_DISTANCES) / sizeof(CALIB_DISTANCES[0]))
        {
          lenses[calib_lens].calibrated = true;
          for (int i = 0; i < sizeof(calib_distance_set) / sizeof(calib_distance_set[0]); i++)
          {
            lenses[calib_lens].sensor_reading[i] = calib_distance_set[i];
          }
          savePrefs();
          ui_mode = "config";
        }
      }
    }
  }

  rbutton.update();
  if (rbutton.rose())
  {
    lastActivityTime = millis();
    if (rbutton.previousDuration() > 5000)
    {
      if (ui_mode == "main")
      {
        ui_mode = "config";
      }
    }
    else
    {
      if (ui_mode == "main")
      {
        cycleApertures("up");
      }
      else if (ui_mode == "config")
      {
        if (config_step == 0)
        {
          cycleISOs();
        }
        else if (config_step == 1)
        {
          cycleFormats();
        }
        else if (config_step == 2)
        {
          cycleLenses();
        }
        else if (config_step == 3)
        {
          calib_step = 0;
          calib_lens = selected_lens;
          current_calib_distance = 0;
          memset(calib_distance_set, 0, sizeof(calib_distance_set));
          ui_mode = "calib";
        }
        else if (config_step == 4)
        {
          encoder.setCount(0);
          encoder_value = 0;
          prev_encoder_value = 0;
          film_counter = 0;
          frame_progress = 0;
          savePrefs();
          ui_mode = "main";
          config_step = 0;
        }
        else if (config_step == 5)
        {
          ui_mode = "main";
          config_step = 0;
        }
      }
      else if (ui_mode == "calib")
      {
        if (calib_step == 0)
        {
          calib_step = 1;
        }
        else if (calib_step == 1)
        {
          calib_step = 0;
          ui_mode = "config";
        }
      }
    }
  }
}
// ---------------------

// Setup and loop functions
// ---------------------
void setup()
{

  Serial.begin(115200); // Initializing serial port

  loadPrefs();

  // Initialise inputs
  pinMode(A1, INPUT);
  lbutton.attach(9, INPUT_PULLUP);
  lbutton.interval(5);
  lbutton.setPressedState(LOW);
  rbutton.attach(10, INPUT_PULLUP);
  rbutton.interval(5);
  rbutton.setPressedState(LOW);

  delay(1000); // Slight delay or the displays won't work
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  Wire.beginTransmission(SCREEN_ADDRESS); // START I2C COMMUNICATION WITH OLED (0x3c=OLED ADDRESS)
  Wire.write(0x80);
  Wire.write(0xC0); // COM SCAN DIRECTION  0XC0 - Reverse scan direction, OxC8 - Normal scan direction
  Wire.endTransmission();
  display.setRotation(1);
  u8g2.begin(display);
  display.clearDisplay();
  display.display();

  delay(1000); // Slight delay or the displays won't work
  display_ext.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS_EXT);

  // Boot up screen
  display_ext.clearDisplay();
  display_ext.setTextSize(2); // Draw 2X-scale text
  display_ext.setTextColor(SSD1306_WHITE);
  display_ext.setCursor(20, 10);
  display_ext.println(F("MRF v2.5"));
  display_ext.display();

  delay(1500);
  u8g2_ext.begin(display_ext);
  display_ext.clearDisplay();
  display_ext.display();

  // Start the LiDAR sensor
  lidarSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(20);
  tfluna.begin(&lidarSerial);

  // Clear the moving average arrays
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++)
  {
    samples[0][i] = 0;
    samples[1][i] = 0;
  }

  // Start the battery gauge and lightmeter
  maxlipo.begin();
  lightMeter.begin();

  // Start the encoder
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoder.attachSingleEdge(13, 12);
  encoder.setFilter(1023);
  encoder.setCount(encoder_value);
}

void loop()
{
  checkButtons();

  if (ui_mode == "main")
  {
    setDistance();
    setLensDistance();
    setVoltage();
    setLightMeter();
    drawMainUI();
    setFilmCounter();
  }
  else if (ui_mode == "config")
  {
    drawConfigUI();
  }
  else if (ui_mode == "calib")
  {
    drawCalibUI();
  }

  drawExternalUI();
}
// ---------------------