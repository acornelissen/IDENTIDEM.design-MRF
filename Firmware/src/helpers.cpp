#include "helpers.h"

#include "globals.h"      // Access to prefs, global variables like iso, lenses, etc.
#include "lenses.h"       // For lenses array structure
#include "mrfconstants.h" // For PIN_I2C_POWER (if defined there), SMOOTHING_WINDOW_SIZE etc.
#include <Arduino.h>      // For pinMode, digitalWrite, digitalRead, delay, String, memcpy, abs, min, max etc.
                          // Preferences.h is included via globals.h for `prefs` object.

// Helper functions
// ---------------------
float getFirstNonZeroAperture() // Original return type was float
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

    byte tempLenses[LENSES_SIZE * sizeof(Lens)]; // Calculate total bytes using LENSES_SIZE and sizeof(Lens)
    prefs.getBytes("lenses", tempLenses, LENSES_SIZE * sizeof(Lens));
    memcpy(lenses, tempLenses, LENSES_SIZE * sizeof(Lens));

    selected_format = prefs.getInt("selected_format", 3);
    selected_lens = prefs.getInt("selected_lens", 1);

    int non_zero_aperture_index = getFirstNonZeroAperture(); // Returns float index

    aperture = prefs.getFloat("aperture", lenses[selected_lens].apertures[(int)non_zero_aperture_index]); // Cast float index to int
    aperture_index = prefs.getInt("aperture_index", (int)non_zero_aperture_index); // Cast float index to int

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
  prefs.putBytes("lenses", (byte *)lenses, LENSES_SIZE * sizeof(Lens)); // Calculate total bytes
  prefs.end();
}

String cmToReadable(int cm_param)
{

  if (cm_param < 100)
  {
    return String(cm_param) + "cm";
  }
  else
  {
    return String(float(cm_param) / 100, 2) + "m";
  }
}

int calcMovingAvg(int index, int sensorVal)
{
  sampleTotal[index] = sampleTotal[index] - (samples[index][curReadIndex[index]]);

  samples[index][curReadIndex[index]] = sensorVal;
  sampleTotal[index] = sampleTotal[index] + samples[index][curReadIndex[index]];
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

void enableInternalPower() {
  pinMode(PIN_I2C_POWER, INPUT);
  delay(1);
  bool polarity = digitalRead(PIN_I2C_POWER);
  pinMode(PIN_I2C_POWER, OUTPUT);
  digitalWrite(PIN_I2C_POWER, !polarity);
}

void disableInternalPower() {
  pinMode(PIN_I2C_POWER, INPUT);
}
// ---------------------