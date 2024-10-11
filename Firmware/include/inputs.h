// Functions to check and act on button presses
// ---------------------
void checkButtons()
{
  lbutton.update();
  if (deepSleep == true) {
    if (rbutton.isPressed()) { 
      longPress += rbutton.currentDuration();

      if (longPress >= 5000) {
        lastActivityTime = millis();
        enableInternalPower();
        rtc_gpio_deinit(GPIO_NUM_10);
        deepSleep = false;
      }
    }
    else if (rbutton.rose() && rbutton.previousDuration() < 5000)
    {
      longPress = 0;
    }
  }
  else {
    if (lbutton.isPressed() && lbutton.currentDuration() >= 5000 && DEEPSLEEP_ENABLED == true) {
      drawSleepUI(0);
      delay(1000);
      deepSleep = true;
    }
    else if (lbutton.rose() && lbutton.previousDuration() < 1000)
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
          if (calib_step == 0)
          {
            cycleCalibLenses();
          }
          if (calib_step == 1)
          {
            calib_distance_set[current_calib_distance] = lens_sensor_reading;
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
            aperture = lenses[selected_lens].apertures[non_zero_aperture_index];
            aperture_index = non_zero_aperture_index;
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
            encoder.setEncoderPosition(0);
            encoder_value = 0;
            prev_encoder_value = 0;
            film_counter = 0;
            frame_progress = 0;
            prev_frame_progress = 0;
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