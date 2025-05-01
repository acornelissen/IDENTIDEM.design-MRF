#include <inputs.h>
#include <Arduino.h>
#include <globals.h>     // Access to global variables (ui_mode, config_step, etc.)
#include <hardware.h>    // Access to hardware objects (lbutton, rbutton, encoder)
#include <cyclefuncs.h>  // Access to cycle functions (cycleApertures, cycleISOs, etc.)
#include <helpers.h>     // Access to helper functions (savePrefs, getFirstNonZeroAperture)
#include <lenses.h>      // Access to lens data structure
#include <mrfconstants.h> // Access to constants (CALIB_DISTANCES)

// Functions to check and act on button presses
// ---------------------
void checkButtons()
{
  lbutton.update();
  // Left Button Short Press (< 1000ms)
  if (lbutton.rose() && lbutton.previousDuration() < 1000)
  {
    lastActivityTime = millis(); // Reset sleep timer
    if (sleepMode == true)
    {
      sleepMode = false; // Wake up
    }
    else
    {
      if (ui_mode == "main")
      {
        cycleApertures("down");
      }
      else if (ui_mode == "config")
      {
        // Cycle through config options
        config_step = (config_step + 1) % 6; // Cycle 0-5
      }
      else if (ui_mode == "calib")
      {
        if (calib_step == 0) // Selecting lens to calibrate
        {
          cycleCalibLenses();
        }
        else if (calib_step == 1) // Setting calibration point
        {
          calib_distance_set[current_calib_distance] = lens_sensor_reading;
          current_calib_distance++;

          // Check if all calibration points are set
          const size_t num_calib_points = sizeof(CALIB_DISTANCES) / sizeof(CALIB_DISTANCES[0]);
          if (current_calib_distance >= num_calib_points)
          {
            lenses[calib_lens].calibrated = true;
            // Copy the collected sensor readings to the lens profile
            const size_t num_sensor_readings = sizeof(lenses[calib_lens].sensor_reading) / sizeof(lenses[calib_lens].sensor_reading[0]);
            // Ensure we don't copy more data than the destination array can hold
            size_t points_to_copy = min(num_calib_points, num_sensor_readings);
            memcpy(lenses[calib_lens].sensor_reading, calib_distance_set, points_to_copy * sizeof(int));

            savePrefs();        // Save the updated lens calibration
            selected_lens = calib_lens; // Switch to the newly calibrated lens
            ui_mode = "config"; // Go back to config menu
            calib_step = 0;     // Reset calibration step
          }
        }
      }
    }
  }

  rbutton.update();
  // Right Button Long Press (>= 3000ms)
  if (rbutton.isPressed() && rbutton.currentDuration() >= 3000)
  {
    lastActivityTime = millis(); // Reset sleep timer (might trigger config mode)
    if (ui_mode == "main")
    {
      ui_mode = "config"; // Enter config mode
      config_step = 0;    // Start at the first config option
      // Potentially add haptic feedback or LED signal for entering config
    }
    // No action needed if already in config or calib mode for long press
  }
  // Right Button Short Press (< 1000ms)
  else if (rbutton.rose() && rbutton.previousDuration() < 1000)
  {
    lastActivityTime = millis(); // Reset sleep timer
    if (sleepMode == true)
    {
      sleepMode = false; // Wake up
    }
    else
    {
      if (ui_mode == "main")
      {
        cycleApertures("up");
      }
      else if (ui_mode == "config")
      {
        switch (config_step)
        {
        case 0: cycleISOs(); break;
        case 1: cycleFormats(); break;
        case 2:
          cycleLenses();
          // Set aperture to the first available one for the new lens
          aperture_index = getFirstNonZeroAperture();
          if (aperture_index != -1) {
              aperture = lenses[selected_lens].apertures[aperture_index];
          } else {
              aperture = 0.0f; // Handle case where lens has no apertures?
          }
          break;
        case 3: // Enter Calibration Mode
          calib_step = 0;
          calib_lens = selected_lens; // Start calibration with the currently selected lens
          current_calib_distance = 0;
          memset(calib_distance_set, 0, sizeof(calib_distance_set)); // Clear temporary calibration data
          ui_mode = "calib";
          break;
        case 4: // Reset Film Counter
          encoder.setEncoderPosition(0);
          encoder_value = 0;
          prev_encoder_value = 0;
          film_counter = 0;
          frame_progress = 0.0f;
          prev_frame_progress = 0.0f;
          savePrefs(); // Save the reset counter state
          // Maybe add feedback here
          break; // Stay in config menu after reset? Or exit? Current code exits.
        case 5: // Exit Config Mode
          ui_mode = "main";
          config_step = 0; // Reset config step for next time
          break;
        }
      }
      else if (ui_mode == "calib")
      {
        if (calib_step == 0) // Confirm lens selection
        {
          calib_step = 1; // Move to setting calibration points
        }
        else if (calib_step == 1) // Cancel calibration point / Exit calibration
        {
          // Maybe add a confirmation step here?
          calib_step = 0;
          ui_mode = "config"; // Go back to config menu
        }
      }
    }
  }
}
// ---------------------