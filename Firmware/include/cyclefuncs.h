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

void cycleLenses()
{
  selected_lens++;
  if (selected_lens >= sizeof(lenses) / sizeof(lenses[0]))
  {
    selected_lens = 0;
  }
  while (!lenses[selected_lens].calibrated)
  {
    selected_lens++;
    if (selected_lens >= sizeof(lenses) / sizeof(lenses[0]))
    {
      selected_lens = 0;
    }
  }

  savePrefs();
}

void cycleCalibLenses()
{
  calib_lens++;
  if (calib_lens >= sizeof(lenses) / sizeof(lenses[0]))
  {
    calib_lens = 0;
  }
  savePrefs();
}
// ---------------------