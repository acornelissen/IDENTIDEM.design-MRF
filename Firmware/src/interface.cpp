#include "interface.h"

#include "globals.h"      // For accessing global state variables like iso, aperture, film_formats, etc.
#include "lenses.h"       // For lenses array (framelines, name)
#include "formats.h"      // For film_formats array (name, frame_fill)
#include "mrfconstants.h" // For SCREEN_WIDTH, RETICLE_OFFSET_X, FWVERSION, etc.
#include "hardware.h"     // For display, u8g2, mpu, sspixel objects
#include "helpers.h"      // For getFocusRadius(), getLensSensorReading()
#include <Arduino.h>      // For math functions (abs, cos, sin, atan2, sqrt), dtostrf
#include <Adafruit_Sensor.h> // For sensors_event_t

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

  // Reset colors for main drawing area (WHITE on BLACK)
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);


  display.fillRect( // Outer frameline area (filled white, then mask cut out in black)
    lenses[selected_lens].framelines[0],
    lenses[selected_lens].framelines[1],
    lenses[selected_lens].framelines[2],
    lenses[selected_lens].framelines[3],
    WHITE
  );

  // Mask calculation (inner black area)
  float width_scale_factor = film_formats[selected_format].frame_fill[1] / 100.0f;
  float height_scale_factor = film_formats[selected_format].frame_fill[0] / 100.0f;

  int new_width = static_cast<int>(lenses[selected_lens].framelines[2] * width_scale_factor);
  int new_height =  static_cast<int>(lenses[selected_lens].framelines[3] * height_scale_factor);

  int width_offset = lenses[selected_lens].framelines[0] + (lenses[selected_lens].framelines[2] - new_width) / 2;
  int heigh_offset = lenses[selected_lens].framelines[1] + (lenses[selected_lens].framelines[3] - new_height) / 2;

  display.fillRect( // Mask (inner area cleared to black)
    width_offset,
    heigh_offset,
    new_width,
    new_height,
    BLACK
  );
  display.drawRect( // Border for the mask
    width_offset,
    heigh_offset,
    new_width,
    new_height,
    WHITE
  );

  // Reticle
  int rectCenterX = (lenses[selected_lens].framelines[0] + lenses[selected_lens].framelines[2] / 2) + RETICLE_OFFSET_X;
  int rectCenterY = (lenses[selected_lens].framelines[1] + lenses[selected_lens].framelines[3] / 2 - 5) + RETICLE_OFFSET_Y; // -5 offset

  display.fillCircle(rectCenterX, rectCenterY, 3, WHITE); // Center dot
  display.drawCircle(rectCenterX, rectCenterY, getFocusRadius(), WHITE); // Focus radius

  // Horizon Level
  sensors_event_t a, g, temp_evt; // Renamed temp to temp_evt
  mpu.getEvent(&a, &g, &temp_evt);
  float x_acc = a.acceleration.x; // Renamed for clarity
  float y_acc = a.acceleration.y;
  float z_acc = a.acceleration.z;

  float pitch_scale = 25; // Scaling factor for pitch visualization
  float roll_scale = 0.5; // Scaling factor for roll visualization (used in cos/sin indirectly)

  // Pitch/roll calculation from accelerometer readings
  float pitch = atan2(x_acc, sqrt(x_acc*x_acc + z_acc*z_acc)); // This is actually roll if X is forward
  float roll = atan2(y_acc, sqrt(x_acc*x_acc + z_acc*z_acc));  // This is actually pitch if Y is side


  float deadzone = 0.03; // Radians
  if (abs(pitch) < deadzone) {
    pitch = 0;
  }
  if (abs(roll) < deadzone) {
    roll = 0;
  }

  // visualization logic for horizon line
  pitch = pitch * pitch_scale; 
  roll = roll * roll_scale;


  float length = SCREEN_WIDTH - 10;

  float startX = rectCenterX - length/2 * cos(roll);
  float startY = rectCenterY - length/2 * sin(roll) + pitch; // Pitch affects Y position
  float endX = rectCenterX + length/2 * cos(roll);
  float endY = rectCenterY + length/2 * sin(roll) + pitch;   // Pitch affects Y position

  display.drawLine(startX, startY, endX, endY, WHITE);

  // Fixed Vertical reference line
  int vertLineLength = 30;
  int vertLineStartY = rectCenterY - vertLineLength / 2;
  int vertLineEndY = rectCenterY + vertLineLength / 2;
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
  int y_pos = 26; // Y starting position
  int line_height = 11; // Line height

  // Menu Item Labels
  const char* itemLabels[] = {" ISO:", " Format:", " Lens:", " Lens Calib. > ", " Reset count >> ", " Exit >> "};
  String itemValues[6]; // Array for values
  itemValues[0] = String(iso);
  itemValues[1] = film_formats[selected_format].name;
  itemValues[2] = lenses[selected_lens].name;

  for (int i = 0; i < 6; ++i) {
    u8g2.setCursor(3, y_pos + i * line_height);
    if (config_step == i) {
      u8g2.setBackgroundColor(WHITE);
      u8g2.setForegroundColor(BLACK);
    } else {
      u8g2.setBackgroundColor(BLACK);
      u8g2.setForegroundColor(WHITE);
    }
    u8g2.print(itemLabels[i]);
    if (i < 3) { 
         u8g2.print(itemValues[i]);
    }
  }

  // Reset colors for footer
  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);

  u8g2.setCursor(3, 100); // Y for footer
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
  u8g2.print(F("Calibrate")); // Title

  u8g2.setFont(u8g2_font_4x6_mf); 
  int y_pos = 35; // Y
  int line_height = 12; // Line height

  // Lens Selection Line
  u8g2.setCursor(3, y_pos);
  if (calib_step == 0) { // Highlight lens selection
    u8g2.setBackgroundColor(WHITE);
    u8g2.setForegroundColor(BLACK);
  } else {
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
  }
  u8g2.print(F(" Lens:"));
  u8g2.print(lenses[calib_lens].name);
  u8g2.print(F(" ")); // Padding for highlight

  // Distance Reading Line (only if calib_step == 1)
  if (calib_step == 1) {
    u8g2.setCursor(3, y_pos + line_height);
    u8g2.setBackgroundColor(WHITE); // Highlight this line
    u8g2.setForegroundColor(BLACK);

    u8g2.print(F(" ")); // Leading space for highlight
    char dist_buf[6]; // Buffer for "x.y" or "xx.y"
    dtostrf(CALIB_DISTANCES[current_calib_distance], 3, 1, dist_buf); // Format to 1 decimal
    u8g2.print(dist_buf);

    u8g2.print(F("m: "));
    u8g2.print(getLensSensorReading());
    u8g2.print(F(" ")); // Trailing space for highlight
  }

  // Reset colors for instructions
  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);

  u8g2.setCursor(3, SCREEN_HEIGHT - 25); 
  if (calib_step == 0) {
    u8g2.print(F(" (L) to Cycle"));
    u8g2.setCursor(3, SCREEN_HEIGHT - 15);
    u8g2.print(F(" (R) to Select"));
  } else { // calib_step == 1
    u8g2.print(F(" (L) to Select"));
    u8g2.setCursor(3, SCREEN_HEIGHT - 15);
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
  u8g2_ext.setFont(u8g2_font_6x10_mf); // font

  display_ext.fillRect(0, 0, SCREEN_WIDTH, 10, WHITE); // Top bar for info

  u8g2_ext.setCursor(2, 8);
  u8g2_ext.print(film_formats[selected_format].name);

  display_ext.drawLine(33, 0, 33, 9, BLACK);

  u8g2_ext.setCursor(37, 8);
  u8g2_ext.print(lenses[selected_lens].name);

  
  int bat_percent_x_start;
  if (bat_per == 100) {
    bat_percent_x_start = 104;
    display_ext.drawLine(bat_percent_x_start - 4, 0, bat_percent_x_start - 4, 9, BLACK);

  } else if (bat_per < 10) {
    bat_percent_x_start = 115;
    display_ext.drawLine(bat_percent_x_start - 4, 0, bat_percent_x_start - 4, 9, BLACK);

  } else { // 10-99%
    bat_percent_x_start = 107;
    display_ext.drawLine(bat_percent_x_start - 4, 0, bat_percent_x_start - 4, 9, BLACK);
  }
  u8g2_ext.setCursor(bat_percent_x_start, 8);
  u8g2_ext.print(bat_per);
  u8g2_ext.print(F("%"));

  // Main area: black background, white text
  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);

  if (frame_progress > 0) // Progress bar logic
  {
    // Draw progress bar
    float progressPercentage = frame_progress * 100; 
    int progressWidth = static_cast<int>(progessBarWidth * frame_progress);

    display_ext.drawRect(progressBarX, progressBarY, progessBarWidth, progressBarHeight, WHITE);
    display_ext.fillRect(progressBarX, progressBarY, progressWidth, progressBarHeight, WHITE);

    if (frame_progress != prev_frame_progress) { // Neopixel color transition
      if (frame_progress > 0 && frame_progress < 1.0) { // Check bounds strictly
        int greenValue = static_cast<int>(frame_progress * 255);
        int redValue = static_cast<int>((1.0 - frame_progress) * 255);
        sspixel.setPixelColor(0, seesaw_NeoPixel::Color(redValue, greenValue, 0));
      }
      
    }
  }
  else // Not actively progressing (frame_progress is 0 or 1)
  {
    sspixel.setPixelColor(0, seesaw_NeoPixel::Color(0, 0, 255)); // Blue for on-frame
  }


  u8g2_ext.setFont(u8g2_font_10x20_mf); 
  int text_y_baseline = 28;

  if (film_counter == 0 && frame_progress == 0) {
    u8g2_ext.setCursor(8, text_y_baseline);
    u8g2_ext.print(F(" Load film."));
    sspixel.setPixelColor(0, seesaw_NeoPixel::Color(238, 130, 238)); // Violet
  } else if (film_counter == 99) { // End of roll
    u8g2_ext.setCursor(8, text_y_baseline);
    u8g2_ext.print(F(" Roll end."));
    sspixel.setPixelColor(0, seesaw_NeoPixel::Color(238, 130, 238)); // Violet
  } else { // Display current frame number

    // Adjust X for one or two digit numbers for centering
      if (film_counter < 10) u8g2_ext.setCursor(8, text_y_baseline); // Single digit
      else u8g2_ext.setCursor(2, text_y_baseline); // Two digits, shift left
      
      if (frame_progress > 0) { // If progress bar shown, put frame number left of it.
         if (film_counter < 10) u8g2_ext.setCursor(8, text_y_baseline);
         else u8g2_ext.setCursor(2, text_y_baseline);
      } else { // No progress bar, number can be more central if desired
         if (film_counter < 10) u8g2_ext.setCursor( (progressBarX - u8g2_ext.getUTF8Width(String(film_counter).c_str()) )/2 +2 , text_y_baseline);
         else u8g2_ext.setCursor(2, text_y_baseline); // for 2 digits
      }
      u8g2_ext.print(film_counter);
  }


  sspixel.show();
  display_ext.display();
}

void drawSleepUI()
{
  display.clearDisplay();
  display_ext.clearDisplay();

  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf); 

    // "ZzzZZzZz..."
    u8g2_ext.setCursor(8, 22); // position
    u8g2_ext.print(F("ZzzZZzZz..."));
  

  display.display();    // Show cleared internal display
  display_ext.display(); // Show message on external

  sspixel.setPixelColor(0, seesaw_NeoPixel::Color(0, 0, 0)); // Turn off Neopixel
  sspixel.show();
}
// ---------------------