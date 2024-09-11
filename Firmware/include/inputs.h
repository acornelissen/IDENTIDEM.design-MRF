// Functions to check and act on button presses
// ---------------------
void checkButtons()
{
  lbutton.update();
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
        if (config_step > 2)
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
            selected_lens = calib_lens;
            ui_mode = "config";
          }
        }
      }
    }
  }
  

  rbutton.update();
  if (rbutton.isPressed() && rbutton.currentDuration() >= 5000) 
  {
    lastActivityTime = millis();
    if (ui_mode == "main")
    {
      ui_mode = "config";
    }
  }
  else if (rbutton.rose() && rbutton.previousDuration() < 1000)
  {
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
          if (config_step == 0) {
            cycleLenses();
            int non_zero_aperture_index = getFirstNonZeroAperture();
            aperture = lenses[selected_lens].apertures[non_zero_aperture_index];
            aperture_index = non_zero_aperture_index;
          }
          else if (config_step == 1)
          {
            calib_step = 0;
            calib_lens = selected_lens;
            current_calib_distance = 0;
            memset(calib_distance_set, 0, sizeof(calib_distance_set));
            ui_mode = "calib";
          }
          else if (config_step == 2)
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