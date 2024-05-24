// Functions to read values from sensors and set variables
// ---------------------
void setDistance()
{
  if (tfminiplus.getData(distance))
  { // Get data from Lidar
    Serial.println(distance);
    distance = distance + LIDAR_OFFSET;
    if (distance != prev_distance)
    {
      if (distance <= LIDAR_OFFSET)
      {
         distance_cm = "> " + String(DISTANCE_MAX) + "m";
      }
      else if (distance < DISTANCE_MIN)
      {
        distance_cm = "< " + String(DISTANCE_MIN) + "cm";
      }
      else {
        distance_cm = cmToReadable(distance);
      }
      prev_distance = distance;
    }
  }
  else {
    distance_cm = "...";
  }
}

// Borrows moving average code from
// https://github.com/makeabilitylab/arduino/blob/master/Filters/MovingAverageFilter/MovingAverageFilter.ino
int getLensSensorReading()
{
  int sensorVal = ads1015.readADC_SingleEnded(1);
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

    for (int i = 0; i < sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]); i++)
    {
      if (lens_sensor_reading > lenses[selected_lens].sensor_reading[0])
      {
        lens_distance_raw = lenses[selected_lens].distance[0] * 100;
        lens_distance_cm = cmToReadable(lens_distance_raw);
      }
      else if (lens_sensor_reading < lenses[selected_lens].sensor_reading[sizeof(lenses[selected_lens].sensor_reading) / sizeof(lenses[selected_lens].sensor_reading[0]) - 1] - 5)
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

  }
}

void setFilmCounter()
{
  int encoder_position = -encoder.getEncoderPosition();
  if (encoder_position != prev_encoder_value && encoder_position > prev_encoder_value)
  {
    encoder_value = encoder_position;
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
        // Check if the encoder value is within +/- 2 of the next frame
        if (abs(encoder_value - film_formats[selected_format].sensor[i + 1]) <= 1)
        {
          // Snap to the next frame
          film_counter = film_formats[selected_format].frame[i + 1];
          frame_progress = 0;
        }
        else
        {
          film_counter = film_formats[selected_format].frame[i];
          frame_progress = static_cast<float>(encoder_value - film_formats[selected_format].sensor[i]) / (film_formats[selected_format].sensor[i + 1] - film_formats[selected_format].sensor[i]);
        }
        
      }
    }

    lastActivityTime = millis();
    savePrefs();
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

     
      if (speed >= 1)
      {
        char print_speed_raw[10];
        dtostrf(speed, 4, 2, print_speed_raw);
        shutter_speed = strcat(print_speed_raw, " sec.");
      }
      else {
         shutter_speed = strcat(print_speed, " sec.");
      }
       
    }
  }
}

void toggleLidar()
{
  if (sleepMode == true)
  {
    tfminiplus.sendCommand(DISABLE_OUTPUT, 0);
  }
  else
  {
    tfminiplus.sendCommand(ENABLE_OUTPUT, 0);
  }
}
// ---------------------