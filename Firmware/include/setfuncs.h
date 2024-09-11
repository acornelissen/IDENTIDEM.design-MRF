// Functions to read values from sensors and set variables
// ---------------------
void setDistance()
{
  if (tfminiplus.getData(distance))
  { // Get data from Lidar
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
  int sensorVal = analogRead(A1);
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


void toggleLidar()
{
  if (sleepMode == true || deepSleep == true)
  {
    tfminiplus.sendCommand(DISABLE_OUTPUT, 0);
  }
  else
  {
    tfminiplus.sendCommand(ENABLE_OUTPUT, 0);
  }
}
// ---------------------