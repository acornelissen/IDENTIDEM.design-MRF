// Functions to draw UI
// ---------------------
void drawMainUI()
{
  display.clearDisplay();

  display.fillRect(
    lenses[selected_lens].framelines[0], 
    lenses[selected_lens].framelines[1], 
    lenses[selected_lens].framelines[2],
    lenses[selected_lens].framelines[3], 
    WHITE
  );

  int new_width = int(lenses[selected_lens].framelines[2] * film_formats[selected_format].frame_fill[1] / 100);
  int new_height =  int(lenses[selected_lens].framelines[3] * film_formats[selected_format].frame_fill[0] / 100);
  int width_offset = lenses[selected_lens].framelines[0] + (lenses[selected_lens].framelines[2] - new_width) / 2;
  int heigh_offset = lenses[selected_lens].framelines[1] + (lenses[selected_lens].framelines[3] - new_height) / 2;
  
  display.fillRect(
    width_offset,
    heigh_offset,
    new_width,
    new_height, 
    BLACK
  );
  display.drawRect(
    width_offset,
    heigh_offset,
    new_width,
    new_height,
    WHITE
  );

  // Calculate the center of the rectangle
  int rectCenterX = (lenses[selected_lens].framelines[0] + lenses[selected_lens].framelines[2] / 2) + RETICLE_OFFSET_X;
  int rectCenterY = (lenses[selected_lens].framelines[1] + lenses[selected_lens].framelines[3] / 2 - 5) + RETICLE_OFFSET_Y;
 
  // Draw a circle at the center of the rectangle
  display.fillCircle(rectCenterX, rectCenterY, 3, WHITE);
  display.drawCircle(rectCenterX, rectCenterY, getFocusRadius(), WHITE);


  // State
  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(BLACK);
  u8g2.setBackgroundColor(WHITE);
  u8g2.setFont(u8g2_font_4x6_mf);
  display.fillRect(0, 0, 128, 27, WHITE);
  display.drawLine(64, 0, 64, 27, BLACK);
  display.drawLine(0, 16, 128, 16, BLACK);
  u8g2.setCursor(2, 7);
  u8g2.print(F("ISO"));
  u8g2.print(iso);
  u8g2.setCursor(46, 7);
  u8g2.print(F("f"));
  if (aperture == static_cast<int>(aperture))
  {
    u8g2.print(static_cast<int>(aperture));
  }
  else
  {
    u8g2.print(aperture, 1);
  }
  u8g2.setCursor(2, 14);
  u8g2.print(shutter_speed);
  u8g2.setCursor(68, 7);
  u8g2.print(F("Dist:"));
  u8g2.print(distance_cm);
  u8g2.setCursor(68, 14);
  u8g2.print(F("Focus:"));
  u8g2.print(lens_distance_cm);

  u8g2.setCursor(2, 25);
  u8g2.print(F("Lens:"));
  u8g2.print(lenses[selected_lens].name);
  u8g2.setCursor(68, 25);
  u8g2.print(F("Bat:"));
  u8g2.print(bat_per);
  u8g2.print(F("%"));


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
  u8g2.setCursor(3, 48);
  u8g2.print(F(" Lens:"));
  u8g2.print(lenses[selected_lens].name);
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
  u8g2.setCursor(3, 59);
  u8g2.print(F(" Lens Calib. > "));


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
  u8g2.setCursor(3, 81);
  u8g2.print(F(" Exit >> "));

  u8g2.setCursor(3, 100);
  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);
  u8g2.print(F(" IDENTIDEM.design WIDESTAX-RF "));
  u8g2.print(FWVERSION);

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
    u8g2.print(F(" (UP) to Select"));
    u8g2.setCursor(3, 81);
    u8g2.print(F(" (DOWN) to Cycle"));
  }
  else
  {
    u8g2.setCursor(3, 70);
    u8g2.print(F(" (UP) to Cancel"));
    u8g2.setCursor(3, 81);
    u8g2.print(F(" (DOWN) to Select"));
   
  }

  display.display();
}

void drawSleepUI() {
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setFont(u8g2_font_4x6_mf);
  
  // Calculate the width of the text to be centered
  const char* text = "ZZZZZZZZZzzzzzZZzzZZzzzz....";
  int16_t textWidth = u8g2.getUTF8Width(text);

  // Calculate the x position to center the text
  int16_t x = (SCREEN_WIDTH - textWidth) / 2;

  // Set the cursor to the calculated position
  u8g2.setCursor(x, SCREEN_HEIGHT / 2);
  u8g2.print(F(text));

  display.display();
}

// ---------------------