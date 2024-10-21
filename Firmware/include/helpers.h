

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

  if (prefs.isKey("iso")) {
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
  }

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
    return String(float(cm) / 100, 1) + "m";
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
  int minRadius = 3;
  int maxRadius = 30;

  int radius = min(maxRadius, max(minRadius, abs(distance - lens_distance_raw)));

  return radius;
}

// ---------------------