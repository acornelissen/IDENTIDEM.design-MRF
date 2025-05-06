#include "setfuncs.h"

#include "globals.h"      // For global variables like distance, lens_sensor_reading, film_counter, etc.
#include "lenses.h"       // For lenses array structure
#include "formats.h"      // For film_formats array structure
#include "mrfconstants.h" // For LIDAR_OFFSET, DISTANCE_MAX, K_VALUE, etc.
#include "hardware.h"     // For ads1015, encoder, maxlipo, lightMeter, tfminiplus objects
#include "helpers.h"      // For calcMovingAvg, cmToReadable, savePrefs
#include "cyclefuncs.h"   // For cycleApertures()
#include <Arduino.h>      // For abs, round, dtostrf, strcat, millis, String

// Functions to read values from sensors and set variables
// ---------------------
void setDistance()
{
  int16_t lidar_reading_cm = 0; 
  
  if (tfminiplus.getData(lidar_reading_cm)) 
  { 
    lidar_reading_cm = lidar_reading_cm + LIDAR_OFFSET; // Apply offset to local
    if (lidar_reading_cm != prev_distance) // Compare with global prev_distance
    {
      // Update global distance related variables
      if (lidar_reading_cm <= LIDAR_OFFSET) // Check local_distance
      {
         distance_cm = "> " + String(DISTANCE_MAX) + "m";
      }
      else if (lidar_reading_cm < DISTANCE_MIN)
      {
        distance_cm = "< " + String(DISTANCE_MIN) + "cm";
      }
      else {
        distance_cm = cmToReadable(lidar_reading_cm);
      }
      distance = lidar_reading_cm; // Update the global distance with the new processed reading
      prev_distance = distance;    // prev_distance now matches the new current distance
    }
  }
  else { // tfminiplus.getData failed
    distance_cm = "..."; 
  }
}

int getLensSensorReading() {
  int sensorVal = ads1015.readADC_SingleEnded(LENS_ADC_PIN);
  return calcMovingAvg(0, sensorVal) / 10;
}

void setLensDistance()
{
  lens_sensor_reading = getLensSensorReading();

  if (lens_sensor_reading != prev_lens_sensor_reading)
  {
    if (abs(lens_sensor_reading - prev_lens_sensor_reading) > 3)
    {
      lastActivityTime = millis();
    }
    prev_lens_sensor_reading = lens_sensor_reading;

    bool found = false; // Flag to stop loop once interpolated
    for (int i = 0; i < (sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0])) -1 && !found; i++)
    {
      // Check boundary conditions first 
      if (lens_sensor_reading > lenses[selected_lens].sensor_reading[0]) { // Closer than 1m
        lens_distance_raw = lenses[selected_lens].distance[0] * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
        found = true;
      } else if (lens_sensor_reading < lenses[selected_lens].sensor_reading[sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]) - 1] - 5) { // Further than last point (infinity) (-5 for buffer)
        lens_distance_raw = 9999999;
        lens_distance_cm = "Inf.";
        found = true;
      }
      // Exact match
      if (lens_sensor_reading == lenses[selected_lens].sensor_reading[i] && !found) {
        lens_distance_raw = lenses[selected_lens].distance[i] * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
        found = true;
      } 
      // Interpolation
      else if (lens_sensor_reading < lenses[selected_lens].sensor_reading[i] && lens_sensor_reading > lenses[selected_lens].sensor_reading[i + 1] && !found) {
        float dist_interpolated = lenses[selected_lens].distance[i] + 
                            (float)(lens_sensor_reading - lenses[selected_lens].sensor_reading[i]) * (lenses[selected_lens].distance[i + 1] - lenses[selected_lens].distance[i]) / 
                            (float)(lenses[selected_lens].sensor_reading[i + 1] - lenses[selected_lens].sensor_reading[i]);
        lens_distance_raw = dist_interpolated * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
        found = true;
      }
    }
    // If not found after loop (e.g. exactly matches last sensor reading)
    if (!found && lens_sensor_reading == lenses[selected_lens].sensor_reading[sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]) - 1]) {
         lens_distance_raw = lenses[selected_lens].distance[sizeof(lenses[selected_lens].distance) / sizeof(lenses[selected_lens].distance[0]) - 1] * 100;
         lens_distance_cm = cmToReadable(lens_distance_raw);
    }
  }
}


void setFilmCounter()
{
  int encoder_position = encoder.getEncoderPosition();

  if (encoder_position != prev_encoder_value && encoder_position > prev_encoder_value)
  {

    bool frame_updated = false;
    for (int i = 0; i < (sizeof(film_formats[selected_format].sensor) / sizeof(film_formats[selected_format].sensor[0]))-1; i++) // Iterate to second last for ranges
    {
      if (film_formats[selected_format].sensor[i] == encoder_position) // Exact match to a frame start
      {
        film_counter = film_formats[selected_format].frame[i];
        frame_progress = 0;
        encoder_value = encoder_position; // Update the 'start of frame' value
        frame_updated = true;
        break;
      }
      else if (film_formats[selected_format].sensor[i] < encoder_position && encoder_position < film_formats[selected_format].sensor[i + 1])
      {
        // Check if the encoder value is within +/- 2 of the next frame (original snap logic)
        if (abs(encoder_position - film_formats[selected_format].sensor[i + 1]) <= 1)
        {
          film_counter = film_formats[selected_format].frame[i + 1];
          frame_progress = 0;
          encoder_value = film_formats[selected_format].sensor[i+1]; // Snap global value
        }
        else
        {
          film_counter = film_formats[selected_format].frame[i]; // Current frame
          frame_progress = static_cast<float>(encoder_position - film_formats[selected_format].sensor[i]) / 
                           (film_formats[selected_format].sensor[i + 1] - film_formats[selected_format].sensor[i]);
        }
        frame_updated = true;
        break;
      }
    }
    // Handle last frame exactly matching sensor value if loop didn't catch it.
    if (!frame_updated) {
        int last_idx = (sizeof(film_formats[selected_format].sensor) / sizeof(film_formats[selected_format].sensor[0])) -1;
        if (encoder_position >= film_formats[selected_format].sensor[last_idx]) { // At or beyond last sensor point
             film_counter = film_formats[selected_format].frame[last_idx]; // Usually 99 (end of roll)
             frame_progress = 0;
             encoder_value = film_formats[selected_format].sensor[last_idx]; // Snap to this if beyond
        }
    }


    prev_encoder_value = encoder_position; // Update prev_encoder_value to current raw position
    lastActivityTime = millis();

    if (sleepMode == true) {
        sleepMode = false;
    }

    savePrefs(); // Save updated encoder_value (start of frame) and film_counter
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
    prev_lux = lux; // Update prev_lux
    if (lux <= 0) // Original check
    {
      shutter_speed = "Dark!";
    }
    else
    {
      if (aperture == 0) // Original check
      {
        cycleApertures("up");
      }


      float speed = round(((aperture * aperture) * K_VALUE) / (lux * iso) * 1000.0f) / 1000.0f;

      struct SpeedRange 
      {
        float lower;
        float upper;
        const char *print_speed_range;
      };

      SpeedRange speed_ranges[] = {
          {0.001f, 0.002f, "1/1000"}, {0.002f, 0.004f, "1/500"},
          {0.004f, 0.008f, "1/250"}, {0.008f, 0.016f, "1/125"},
          {0.016f, 0.033f, "1/60"},  {0.033f, 0.066f, "1/30"},
          {0.066f, 0.125f, "1/15"},  {0.125f, 0.250f, "1/8"},
          {0.250f, 0.500f, "1/4"},   {0.500f, 1.0f, "1/2"} // Changed 1 to 1.0f
      };

      char print_speed[10];

      bool speed_set_from_table = false;
      for (int i = 0; i < sizeof(speed_ranges) / sizeof(speed_ranges[0]); i++)
      {
        if (speed_ranges[i].lower <= speed && speed < speed_ranges[i].upper)
        {
          strcpy(print_speed, speed_ranges[i].print_speed_range);
          speed_set_from_table = true;
          break;
        }
      }

      if (speed >= 1.0f && !speed_set_from_table) // If speed >= 1s and not caught by table
      {
        char print_speed_raw[10];
        dtostrf(speed, 4, (speed == static_cast<int>(speed)) ? 0:2 , print_speed_raw); // 0 decimal if whole, else 2
        shutter_speed = String(print_speed_raw) + " sec."; // String concatenation
      }
      else if (!speed_set_from_table) { // If not in table and < 1s (e.g. very fast)
         // Default to raw speed if very fast and not in table, or very slow and not handled
         dtostrf(speed, 1, 4, print_speed); // More precision for very small numbers
         shutter_speed = String(print_speed) + " sec.";
      } else { // Speed was set from table
         shutter_speed = String(print_speed) + " sec.";
      }
    }
  }
}

void toggleLidar(bool lidarStatus)
{
  if ((sleepMode == true) && lidarStatus == false) {
    tfminiplus.sendCommand(DISABLE_OUTPUT, 0);
  }
  else {
    tfminiplus.sendCommand(ENABLE_OUTPUT, 0);
  }
}
// ---------------------