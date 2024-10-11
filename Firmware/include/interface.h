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
  display.fillRect(0, 0, 128, 15, WHITE);
  display.drawLine(64, 0, 64, 128, BLACK);
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
  u8g2.print(F("Lens:"));
  u8g2.print(lens_distance_cm);

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

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float x = a.acceleration.x;
  float y = a.acceleration.y;
  float z = a.acceleration.z;

  // Convert accelerometer readings into angles
  float pitch_scale = 25;
  float roll_scale = 0.5;
  float pitch = atan2(x, sqrt(x*x + z*z));
  float roll = atan2(y, sqrt(x*x + z*z));

  // Define the deadzone
  float deadzone = 0.03;

  // Apply the deadzone to the pitch and roll
  if (abs(pitch) < deadzone) {
    pitch = 0;
  }
  if (abs(roll) < deadzone) {
    roll = 0;
  }

  pitch = pitch * pitch_scale;
  roll = roll * roll_scale;

  // Define the length of the line
  float length = SCREEN_WIDTH - 10;

  // Calculate the start and end points of the line
  float startX = rectCenterX - length/2 * cos(roll);
  float startY =  rectCenterY - length/2 * sin(roll) + pitch;
  float endX = rectCenterX + length/2 * cos(roll);
  float endY =  rectCenterY + length/2 * sin(roll) + pitch;

  // Draw the line on the display
  display.drawLine(startX, startY, endX, endY, WHITE);   

  // Define the length of the vertical line
  int vertLineLength = 30;

  // Calculate the start and end points of the vertical line
  int vertLineStartY = rectCenterY - vertLineLength / 2;
  int vertLineEndY = rectCenterY + vertLineLength / 2;

  // Draw the vertical line on the display
  display.drawLine(rectCenterX, vertLineStartY, rectCenterX, vertLineEndY, WHITE);

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

  u8g2.setCursor(3, 100);
  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);
  u8g2.print(F(" IDENTIDEM.design MRF "));
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
  u8g2.print(lens_sensor_reading);
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
    u8g2.print(F(" (R) to Cancel"));
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

    if (frame_progress != prev_frame_progress) {
      if (progressPercentage > 0 && progressPercentage < 100) {
        // Transition from red to green
        int greenValue = frame_progress * 255;
        int redValue = (1 - frame_progress) * 255;
        // Set the LED color using greenValue and redValue
        sspixel.setPixelColor(0, sspixel.Color(redValue, greenValue, 0));
      }
      prev_frame_progress = frame_progress;
    }
  }
  else
  {
    sspixel.setPixelColor(0, sspixel.Color(0, 0, 255));
    u8g2_ext.setCursor(60, 30);
  }

  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf);
  if (film_counter == 0 && frame_progress == 0)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Load film."));

    sspixel.setPixelColor(0, sspixel.Color(238, 130, 238));
  }
  else if (film_counter == 99)
  {
    u8g2_ext.setCursor(8, 30);
    u8g2_ext.print(F(" Roll end."));

    sspixel.setPixelColor(0, sspixel.Color(238, 130, 238));
  }
  else
  {
    u8g2_ext.print(film_counter);
  }

  sspixel.show();
  display_ext.display();
}

void drawSleepUI(int type)
{
  display.clearDisplay();
  display_ext.clearDisplay();

  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf);

  if (type == 0) {
    u8g2_ext.setCursor(25, 22);
    u8g2_ext.print(F("Goodbye!"));
  }
  else if (type == 1) {
    u8g2_ext.setCursor(8, 22);
    u8g2_ext.print(F("ZzzZZzZz..."));
  }

  display.display();
  display_ext.display();

  sspixel.setPixelColor(0, sspixel.Color(0, 0, 0));
  sspixel.show();
}

// ---------------------