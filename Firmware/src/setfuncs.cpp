#include <setfuncs.h>
#include <Arduino.h>
#include <globals.h>     // Access to global variables (distance, distance_cm, etc.)
#include <hardware.h>    // Access to hardware objects (tfminiplus, ads1115, encoder, maxlipo, lightMeter)
#include <helpers.h>     // Access to helper functions (cmToReadable, calcMovingAvg, savePrefs)
#include <lenses.h>      // Access to lens data structure and constants (LENS_INF_THRESHOLD)
#include <formats.h>     // Access to film format data structure
#include <mrfconstants.h> // Access to constants (LIDAR_OFFSET, DISTANCE_MAX, etc.)
#include <cyclefuncs.h>  // Access to cycleApertures
#include <math.h>        // For round()
#include <string.h>      // For strcpy, strcat
#include <stdlib.h>      // For dtostrf (though often included by Arduino.h)

// Functions to read values from sensors and set variables
// ---------------------

/**
 * @brief Reads distance from LiDAR sensor, applies offset, updates global variables.
 */
void setDistance()
{
  // Attempt to get data from LiDAR
  if (tfminiplus.getData(distance)) // distance is int16_t
  {
    // Apply offset only if reading is valid (getData returns true)
    distance += LIDAR_OFFSET; // Add offset in cm

    // Update display string only if distance changed
    if (distance != prev_distance)
    {
      if (distance <= LIDAR_OFFSET) // Check if reading is effectively zero or negative after offset
      {
        // Indicate out of range (too far or error)
        distance_cm = "> " + String(DISTANCE_MAX) + "m"; // Assuming DISTANCE_MAX is in meters
      }
      else if (distance < DISTANCE_MIN) // Check if below minimum reliable range
      {
        // Indicate too close
        distance_cm = "< " + String(DISTANCE_MIN) + "cm";
      }
      else
      {
        // Format valid distance reading
        distance_cm = cmToReadable(distance, 2); // Show 2 decimal places for meters
      }
      prev_distance = distance; // Store current value for next comparison
    }
  }
  else
  {
    // Handle case where getData failed (e.g., timeout, sensor error)
    // Check if the string needs updating to avoid constant redraws
    if (distance_cm != "...")
    {
      distance_cm = "...";
      distance = 0;
      prev_distance = 0;
    }
  }
}

/**
 * @brief Reads the lens ADC pin, applies scaling, and returns a moving average.
 * @return Smoothed lens sensor reading.
 */
int getLensSensorReading()
{
  // Read ADC value
  int rawAdcValue = ads1115.readADC_SingleEnded(LENS_ADC_PIN);
  
  // Apply scaling (division by 10) and round
  float scaledValue = static_cast<float>(rawAdcValue) / 10.0f;
  int sensorVal = static_cast<int>(round(scaledValue));

  // Calculate and return the moving average using helper function
  return calcMovingAvg(0, sensorVal);
}

/**
 * @brief Reads lens sensor, interpolates distance based on calibration, updates globals.
 */
void setLensDistance()
{
  lens_sensor_reading = getLensSensorReading();

  if (lens_sensor_reading != prev_lens_sensor_reading)
  {
    // Reset sleep timer only if change is significant enough
    if (abs(lens_sensor_reading - prev_lens_sensor_reading) > LENS_ACTIVITY_THRESHOLD) // Use a constant
    {
      lastActivityTime = millis();
      if (sleepMode) { sleepMode = false; } // Wake up if sleeping
    }

    prev_lens_sensor_reading = lens_sensor_reading;

    // Ensure selected_lens is valid before accessing lens data
    if (selected_lens < 0 || selected_lens >= (NUM_LENSES)) {
        lens_distance_cm = "Lens Error";
        lens_distance_raw = -1; // Indicate error
        return; // Exit function
    }

    // Get number of calibration points for the selected lens
    const size_t num_calib_points = sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]);
    if (num_calib_points < 2) { // Need at least 2 points for interpolation
        lens_distance_cm = "Not Calib.";
        lens_distance_raw = -1;
        return;
    }

    // Handle edge cases: below min or above max calibration reading
    if (lens_sensor_reading < lenses[selected_lens].sensor_reading[0])
    {
      lens_distance_raw = static_cast<int>(lenses[selected_lens].distance[0] * 100.0f); // Distance is float, convert to cm
      lens_distance_cm = cmToReadable(lens_distance_raw, 1);
    }
    else if (lens_sensor_reading > lenses[selected_lens].sensor_reading[num_calib_points - 1] + LENS_INF_THRESHOLD)
    {
      lens_distance_raw = 9999999; // Use a large number to signify infinity
      lens_distance_cm = "Inf.";
    }
    else
    {
      // Interpolate between calibration points
      bool found = false;
      for (size_t i = 0; i < num_calib_points - 1; ++i) // Iterate up to second-to-last point
      {
        // Check for exact match first
        if (lens_sensor_reading == lenses[selected_lens].sensor_reading[i])
        {
          lens_distance_raw = static_cast<int>(lenses[selected_lens].distance[i] * 100.0f);
          lens_distance_cm = cmToReadable(lens_distance_raw, 1);
          found = true;
          break;
        }
        // Check if reading falls between this point and the next
        else if (lens_sensor_reading > lenses[selected_lens].sensor_reading[i] && lens_sensor_reading < lenses[selected_lens].sensor_reading[i + 1])
        {
          // Perform linear interpolation
          float sensor_range = lenses[selected_lens].sensor_reading[i + 1] - lenses[selected_lens].sensor_reading[i];
          // Avoid division by zero if calibration points are identical
          if (sensor_range == 0) {
              lens_distance_raw = static_cast<int>(lenses[selected_lens].distance[i] * 100.0f); // Use the distance of the current point
          } else {
              float fraction = static_cast<float>(lens_sensor_reading - lenses[selected_lens].sensor_reading[i]) / sensor_range;
              float distance_range = lenses[selected_lens].distance[i + 1] - lenses[selected_lens].distance[i];
              float interpolated_distance_m = lenses[selected_lens].distance[i] + (fraction * distance_range);
              lens_distance_raw = static_cast<int>(interpolated_distance_m * 100.0f); // Convert to cm
          }
          lens_distance_cm = cmToReadable(lens_distance_raw, 1);
          found = true;
          break;
        }
      }
      // Handle exact match with the last point if not found in the loop
      if (!found && lens_sensor_reading == lenses[selected_lens].sensor_reading[num_calib_points - 1]) {
          lens_distance_raw = static_cast<int>(lenses[selected_lens].distance[num_calib_points - 1] * 100.0f);
          lens_distance_cm = cmToReadable(lens_distance_raw, 1);
      }
    }
  }
}

void setFilmCounter()
{
  int encoder_position = encoder.getEncoderPosition();

  // Process only if encoder moved forward
  if (encoder_position != prev_encoder_value && encoder_position > prev_encoder_value)
  {
    lastActivityTime = millis();
    if (sleepMode) { sleepMode = false; } // Wake up

    encoder_value = encoder_position;
    prev_encoder_value = encoder_value; // Update previous value

    // Ensure selected_format is valid
    if (selected_format < 0 || selected_format >= NUM_FILM_FORMATS) {
        // Handle error - maybe log or display an error?
        return;
    }

    const size_t num_format_points = sizeof(film_formats[selected_format].sensor) / sizeof(film_formats[selected_format].sensor[0]);
    if (num_format_points < 2) return; // Need at least 2 points

    bool counter_updated = false;
    for (size_t i = 0; i < num_format_points - 1; ++i) // Iterate up to second-to-last point
    {
      // Exact match with a frame mark
      if (encoder_value == film_formats[selected_format].sensor[i])
      {
        film_counter = film_formats[selected_format].frame[i];
        frame_progress = 0.0f;
        counter_updated = true;
        break;
      }
      // Between frame marks
      else if (encoder_value > film_formats[selected_format].sensor[i] && encoder_value < film_formats[selected_format].sensor[i + 1])
      {
        // Check for snapping threshold near the *next* frame mark
        if (abs(encoder_value - film_formats[selected_format].sensor[i + 1]) <= FILM_SNAP_THRESHOLD)
        {
          film_counter = film_formats[selected_format].frame[i + 1];
          frame_progress = 0.0f; // Snapped to next frame
          // Optional: Update encoder position visually? encoder.setEncoderPosition(film_formats[selected_format].sensor[i + 1]);
        }
        else
        {
          // Calculate progress between frames
          film_counter = film_formats[selected_format].frame[i];
          float sensor_range = film_formats[selected_format].sensor[i + 1] - film_formats[selected_format].sensor[i];
          if (sensor_range > 0) { // Avoid division by zero
              frame_progress = static_cast<float>(encoder_value - film_formats[selected_format].sensor[i]) / sensor_range;
          } else {
              frame_progress = 0.0f; // Or 1.0f depending on desired behavior for identical points
          }
        }
        counter_updated = true;
        break;
      }
    }

    // Handle reaching or exceeding the last defined sensor point (often end-of-roll)
    if (!counter_updated && encoder_value >= film_formats[selected_format].sensor[num_format_points - 1]) {
        // Check if the last frame entry is the special '99' end-of-roll marker
        if (film_formats[selected_format].frame[num_format_points - 1] == 99) {
            film_counter = 99;
            frame_progress = 0.0f;
            counter_updated = true;
        }
        // Optional: Handle cases where the last point isn't 99 if needed
    }

    // Save preferences only if the counter or progress was actually updated
    if (counter_updated) {
        savePrefs();
    }
  }
  // Optional: Handle encoder moving backward if needed
  // else if (encoder_position != prev_encoder_value && encoder_position < prev_encoder_value) { ... }
}

void setVoltage()
{
  int current_bat_per = maxlipo.cellPercent();

  // Clamp battery percentage to 0-100 range
  if (current_bat_per > 100) {
    current_bat_per = 100;
  } else if (current_bat_per < 0) { // Also check lower bound
    current_bat_per = 0;
  }

  // Update global only if the value changed
  if (current_bat_per != bat_per) // Use bat_per (global) for comparison
  {
    bat_per = current_bat_per;
    // prev_bat_per is not strictly needed if only updating on change, but keep if used elsewhere
    prev_bat_per = bat_per;
  }
}

void setLightMeter()
{
  // Read light level - BH1750 library handles potential errors internally, often returning 0 or -1
  float current_lux = lightMeter.readLightLevel();

  // Update only if lux value has changed significantly or crossed zero threshold
  // Using a small tolerance might prevent flicker from minor fluctuations
  if (abs(current_lux - prev_lux) > LUX_CHANGE_THRESHOLD || (current_lux <= 0 && prev_lux > 0) || (current_lux > 0 && prev_lux <= 0))
  {
    lux = current_lux; // Update global lux value

    if (lux <= 0) // Handle darkness or sensor error
    {
      shutter_speed = "Dark!";
    }
    else
    {
      // Ensure a valid aperture is selected
      if (aperture <= 0.0f) // Check against 0.0f for float
      {
        // Attempt to cycle to the first valid aperture if current one is invalid
        cycleApertures("up"); // Or find the first non-zero one directly
        if (aperture <= 0.0f) { // If still invalid after cycling (e.g., no apertures defined)
            shutter_speed = "Set Aperture";
            prev_lux = lux; // Store current lux to prevent re-entry until aperture changes
            return; // Exit early
        }
      }

      // Calculate exposure value (simplified formula, assumes K is defined)
      // Ensure iso is not zero to prevent division by zero
      if (iso == 0) {
          shutter_speed = "Set ISO";
          prev_lux = lux;
          return;
      }
      // Calculate shutter speed in seconds
      float speed_sec = ((aperture * aperture) * K) / (lux * iso);

      // --- Shutter Speed Formatting ---
      // Define standard shutter speed steps (approximations)
      const struct SpeedStep { float sec; const char* display; } speed_steps[] = {
          {1/1000.0f, "1/1000"}, {1/500.0f, "1/500"}, {1/250.0f, "1/250"},
          {1/125.0f, "1/125"}, {1/60.0f,  "1/60"},  {1/30.0f,  "1/30"},
          {1/15.0f,  "1/15"},  {1/8.0f,   "1/8"},   {1/4.0f,   "1/4"},
          {1/2.0f,   "1/2"},   {1.0f,     "1\""},    {2.0f,     "2\""},
          {4.0f,     "4\""},    {8.0f,     "8\""},    {15.0f,    "15\""},
          {30.0f,    "30\""}
          // Add more steps as needed (e.g., 60", Bulb)
      };
      const size_t num_speed_steps = sizeof(speed_steps) / sizeof(speed_steps[0]);

      // Find the closest standard shutter speed
      const char* display_speed = "---"; // Default if no match
      float min_diff = 1e6; // Initialize with a large difference

      for (size_t i = 0; i < num_speed_steps; ++i) {
          float diff = abs(log(speed_sec) - log(speed_steps[i].sec)); // Compare logarithmically for better perceptual match
          if (diff < min_diff) {
              min_diff = diff;
              display_speed = speed_steps[i].display;
          }
      }

      // Handle speeds longer than the longest defined step (e.g., > 30s)
      if (speed_sec > speed_steps[num_speed_steps - 1].sec * 1.414f) { // Check if significantly longer
          char buf[10];
          dtostrf(speed_sec, 4, (speed_sec < 10 ? 1 : 0), buf); // Adjust precision based on value
          shutter_speed = String(buf) + "\"";
      } else {
          shutter_speed = display_speed;
      }
    }
    prev_lux = lux; // Store current value for next comparison
  }
}

/**
 * @brief Enables or disables the LiDAR sensor output.
 * @param lidarStatus true to enable, false to disable.
 */
void toggleLidar(bool lidarStatus)
{
  // Send the appropriate command to the TFMini Plus
  // Note: The function parameter 'lidarStatus' shadows any global variable with the same name.
  // It's generally better to use a different name or rely on a global state if needed.
  if (lidarStatus == false)
  {
    tfminiplus.sendCommand(DISABLE_OUTPUT, 0);
    // Consider updating a global state variable here if needed, e.g., global_lidar_enabled = false;
  }
  else
  {
    tfminiplus.sendCommand(ENABLE_OUTPUT, 0);
    // Consider updating a global state variable here if needed, e.g., global_lidar_enabled = true;
  }
  
  delay(10);
}
// ---------------------