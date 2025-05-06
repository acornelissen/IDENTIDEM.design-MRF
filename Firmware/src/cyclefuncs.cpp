#include "cyclefuncs.h"

// Required for globals like selected_lens, aperture_index, iso_index, etc.
// and for lenses, film_formats, ISOS arrays.
#include "globals.h"
#include "lenses.h"
#include "formats.h"
#include "mrfconstants.h"
#include "helpers.h" // For savePrefs()

// Functions to cycle values
// ---------------------
void cycleApertures(String direction)
{

  if (direction == "up")
  {
    aperture_index++;
    if (aperture_index >= sizeof(lenses[selected_lens].apertures) / sizeof(lenses[selected_lens].apertures[0]))
    {
      aperture_index = 0;
    }
    if (lenses[selected_lens].apertures[aperture_index] == 0)
    {
      cycleApertures("up");
    }
  }
  else if (direction == "down")
  {
    aperture_index--;
    if (aperture_index < 0)
    {
      aperture_index = sizeof(lenses[selected_lens].apertures) / sizeof(lenses[selected_lens].apertures[0]) - 1;
    }
    if (lenses[selected_lens].apertures[aperture_index] == 0)
    {
      cycleApertures("down");
    }
  }

  aperture = lenses[selected_lens].apertures[aperture_index];

  savePrefs();
}

void cycleISOs()
{
  iso_index++;
  if (iso_index >= ISOS_SIZE)
  {
    iso_index = 0;
  }

  iso = ISOS[iso_index];
  savePrefs();
}

void cycleLenses()
{
  selected_lens++;
  if (selected_lens >= LENSES_SIZE)
  {
    selected_lens = 0;
  }
  while (!lenses[selected_lens].calibrated)
  {
    selected_lens++;
    if (selected_lens >= LENSES_SIZE)
    {
      selected_lens = 0;
    }
  }

  savePrefs();
}

void cycleCalibLenses()
{
  calib_lens++;
  if (calib_lens >= LENSES_SIZE)
  {
    calib_lens = 0;
  }
  // Original did not call savePrefs() here.
}

void cycleFormats()
{
  selected_format++;
  if (selected_format >= FILM_FORMATS_SIZE)
  {
    selected_format = 0;
  }
  savePrefs();
}
// ---------------------