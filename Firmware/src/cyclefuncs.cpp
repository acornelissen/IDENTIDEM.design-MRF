#include <Arduino.h>
#include <Preferences.h>

#include <globals.h>
#include <lenses.h>
#include <formats.h>
#include <mrfconstants.h>
#include <helpers.h>
#include <cyclefuncs.h>

// Functions to cycle values
// ---------------------
void cycleApertures(String direction)
{

  if (direction == "up")
  {
    aperture_index++;
    // Use the defined size constant for the apertures array from lenses.h
    if (aperture_index >= NUM_APERTURES)
    {
      aperture_index = 0;
    }
    // Skip placeholder zero values if any
    if (lenses[selected_lens].apertures[aperture_index] == 0)
    {
      cycleApertures("up"); // Recursive call to skip
      return; // Prevent setting aperture and saving prefs multiple times
    }
  }
  else if (direction == "down")
  {
    aperture_index--;
    if (aperture_index < 0)
    {
      // Use the defined size constant for the apertures array from lenses.h
      aperture_index = NUM_APERTURES - 1;
    }
    // Skip placeholder zero values if any
    if (lenses[selected_lens].apertures[aperture_index] == 0)
    {
      cycleApertures("down"); // Recursive call to skip
      return; // Prevent setting aperture and saving prefs multiple times
    }
  }

  aperture = lenses[selected_lens].apertures[aperture_index];
  savePrefs();
}

void cycleISOs()
{
  iso_index = (iso_index + 1) % (sizeof(ISOS) / sizeof(ISOS[0]));
  iso = ISOS[iso_index];
  savePrefs();
}

void cycleLenses()
{
  do {
    selected_lens = (selected_lens + 1) % (NUM_LENSES);
  } while (!lenses[selected_lens].calibrated); // Keep cycling until a calibrated lens is found

  savePrefs();
}

void cycleCalibLenses()
{
  calib_lens = (calib_lens + 1) % (NUM_LENSES);
  savePrefs();
}

void cycleFormats()
{
  selected_format = (selected_format + 1) % (NUM_FILM_FORMATS);
  savePrefs();
}
// ---------------------