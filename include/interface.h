// Functions to draw UI
// ---------------------
void drawMainUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(BLACK);
  u8g2.setBackgroundColor(WHITE);
  u8g2.setFont(u8g2_font_4x6_mf);
  display.fillRect(0, 7, 64, 31, WHITE);
  display.drawLine(0, 17, 64, 17, BLACK);
  display.drawLine(0, 27, 64, 27, BLACK);
  display.drawLine(31, 0, 31, 40, BLACK);
  u8g2.setCursor(3, 15);
  u8g2.print(bat_per);
  u8g2.print(F("%"));
  u8g2.setCursor(35, 15);
  u8g2.print(F("ISO"));
  u8g2.print(iso);
  u8g2.setCursor(3, 25);
  u8g2.print(F("f"));
  if (aperture == static_cast<int>(aperture))
  {
    u8g2.print(static_cast<int>(aperture));
  }
  else
  {
    u8g2.print(aperture, 1);
  }
  u8g2.setCursor(34, 25);
  u8g2.print(shutter_speed);
  u8g2.setCursor(3, 35);
  u8g2.print(F("D:"));
  u8g2.print(distance_cm);
  u8g2.setCursor(34, 35);
  u8g2.print(F("L:"));
  u8g2.print(lens_distance_cm);

  ReticlePosition reticlePosition = calculateReticlePosition(distance);

  display.drawRect(-18, 42, 99, 85, WHITE);
  display.fillCircle(reticlePosition.x, reticlePosition.y, 2, WHITE);
  display.drawCircle(reticlePosition.x, reticlePosition.y, getFocusRadius(), WHITE);

  display.display();
}

void drawConfigUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  u8g2.setFont(u8g2_font_9x15_mf);
  u8g2.setCursor(3, 15);
  u8g2.print(F("Setup"));

  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.setCursor(3, 26);

  if (config_step == 0)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.print(F(" ISO:"));
  u8g2.print(iso);
  u8g2.print(F(" "));

  if (config_step == 1)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 37);
  u8g2.print(F(" Format:"));
  u8g2.print(film_formats[selected_format].name);
  u8g2.print(F(" "));

  if (config_step == 2)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 48);
  u8g2.print(F(" Lens:"));
  u8g2.print(lenses[selected_lens].name);
  u8g2.print(F(" "));

  if (config_step == 3)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 59);
  u8g2.print(F(" Lens Calib. > "));

  if (config_step == 4)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 70);
  u8g2.print(F(" Reset count >> "));

  if (config_step == 5)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 81);
  u8g2.print(F(" Exit >> "));

  display.display();
}

void drawCalibUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.setCursor(3, 15);
  u8g2.print(F("Calibrate"));

  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.setCursor(3, 35);

  if (calib_step == 0)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.print(F(" Lens:"));
  u8g2.print(lenses[calib_lens].name);
  u8g2.print(F(" "));

  if (calib_step == 1)
  {
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  }
  else
  {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.setCursor(3, 47);

  u8g2.print(F(" "));
  u8g2.print(CALIB_DISTANCES[current_calib_distance], 1);
  u8g2.print(F("m: "));
  u8g2.print(getLensSensorReading());
  u8g2.print(F(" "));

  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);

  if (calib_step == 0)
  {
    u8g2.setCursor(3, 70);
    u8g2.print(F(" (L) to Cycle"));
    u8g2.setCursor(3, 81);
    u8g2.print(F(" (R) to Select"));
  }
  else
  {
    u8g2.setCursor(3, 70);
    u8g2.print(F(" (L) to Select"));
    u8g2.setCursor(3, 81);
    u8g2.print(F("(R) to Cancel"));
  }

  display.display();
}

void drawExternalUI()
{

  int progessBarWidth = 90;
  int progressBarHeight = 17;
  int progressBarX = 34;
  int progressBarY = 15;

  display_ext.clearDisplay();

  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(BLACK);
  u8g2_ext.setBackgroundColor(WHITE);
  u8g2_ext.setFont(u8g2_font_6x10_mf);

  u8g2_ext.setCursor(2, 8);
  display_ext.fillRect(0, 0, 128, 10, WHITE);
  u8g2_ext.print(film_formats[selected_format].name);

  display_ext.drawLine(33, 0, 33, 10, BLACK);

  u8g2_ext.setCursor(37, 8);
  u8g2_ext.print(lenses[selected_lens].name);

  if (bat_per == 100)
  {
    display_ext.drawLine(100, 0, 100, 10, BLACK);
    u8g2_ext.setCursor(104, 8);
  }
  else if (bat_per < 10)
  {
    display_ext.drawLine(111, 0, 111, 10, BLACK);
    u8g2_ext.setCursor(115, 8);
  }
  else
  {
    display_ext.drawLine(103, 0, 103, 10, BLACK);
    u8g2_ext.setCursor(107, 8);
  }

  u8g2_ext.print(bat_per);
  u8g2_ext.print(F("%"));

  u8g2_ext.setCursor(8, 30);

  if (frame_progress > 0)
  {
    float progressPercentage = frame_progress * 100;
    int progressWidth = progessBarWidth * (progressPercentage / 100);

    display_ext.drawRect(progressBarX, progressBarY, progessBarWidth, progressBarHeight, WHITE);
    display_ext.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, WHITE);
  }
  else
  {
    u8g2_ext.setCursor(60, 30);
  }

  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf);
  if (film_counter == 0 && frame_progress == 0)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Load film."));
  }
  else if (film_counter == 99)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Roll end."));
  }
  else
  {
    u8g2_ext.print(film_counter);
  }

  display_ext.display();
}
// ---------------------