#include "inputs.h"

#include "globals.h"      // For ui_mode, sleepMode, deepSleep, config_step, etc.
#include "helpers.h"      // For savePrefs, getLensSensorReading, enableInternalPower, etc.
#include "cyclefuncs.h"   // For cycleApertures, cycleISOs, etc.
#include "interface.h"    // For drawSleepUI
#include "mrfconstants.h" // For CALIB_DISTANCES, etc.
#include "hardware.h"     // For lbutton, rbutton, encoder objects
#include "lenses.h"       // For lenses array for aperture cycling logic
#include <Arduino.h>      // For millis(), memset(), delay()
#include "driver/rtc_io.h"// For rtc_gpio_deinit etc.

// Functions to check and act on button presses
// ---------------------
void checkButtons()
{
  lbutton.update();
  rbutton.update(); 
  
     if (lbutton.rose() && lbutton.previousDuration() < 1000)
    {
      lastActivityTime = millis();
      if (sleepMode == true) {
        sleepMode = false;
      }
      else {
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
          if (calib_step == 0) // Cycle lenses to calibrate
          {
            cycleCalibLenses();
          }
          if (calib_step == 1) // Set current sensor reading for current distance point
          {
            calib_distance_set[current_calib_distance] = getLensSensorReading();
            current_calib_distance++;

            if (current_calib_distance >= CALIB_DISTANCES_SIZE ) {          
              lenses[calib_lens].calibrated = true;
              for (int i = 0; i < CALIB_DISTANCES_SIZE; i++)
              {
                lenses[calib_lens].sensor_reading[i] = calib_distance_set[i];
              }
              savePrefs();
              selected_lens = calib_lens;
              ui_mode = "config";
            }
          }
      }
      }
    }
    

  if (rbutton.isPressed() && rbutton.currentDuration() >= 5000 )
  {
    lastActivityTime = millis();
    if (ui_mode == "main")
    {
      ui_mode = "config";
    }
  }
  else if (rbutton.rose() && rbutton.previousDuration() < 1000 ) {
    lastActivityTime = millis();
    if (sleepMode == true) {
      sleepMode = false;
    }
    else {
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
            int non_zero_aperture_index = getFirstNonZeroAperture(); 
            aperture = lenses[selected_lens].apertures[(int)non_zero_aperture_index];
            aperture_index = (int)non_zero_aperture_index;
          }
          else if (config_step == 3) // Lens Calib >
          {
            calib_step = 0;
            calib_lens = selected_lens; // Default to current lens
            current_calib_distance = 0;
            memset(calib_distance_set, 0, sizeof(calib_distance_set));
            ui_mode = "calib";
          }
          else if (config_step == 4) // Reset count >>
          {
            encoder.setEncoderPosition(0);
            encoder_value = 0;
            prev_encoder_value = 0; // Keep this consistent
            film_counter = 0;
            frame_progress = 0;
            prev_frame_progress = 0; // Keep this consistent
            savePrefs();
            ui_mode = "main";
            config_step = 0;
          }
          else if (config_step == 5) // Exit >>
          {
            ui_mode = "main";
            config_step = 0;
          }
        }
        else if (ui_mode == "calib")
        {
          if (calib_step == 0) // Lens selected, move to setting points
          {
            calib_step = 1;
          }
          else if (calib_step == 1) // Cancel calibration for current lens
          {
            calib_step = 0; 
            ui_mode = "config";
          }
        }
    }
}
}
// ---------------------