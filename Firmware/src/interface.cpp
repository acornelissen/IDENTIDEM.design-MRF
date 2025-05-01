#include <interface.h>
#include <Arduino.h>
#include <globals.h>     // Access to global variables (iso, aperture, ui_mode, etc.)
#include <hardware.h>    // Access to hardware objects (display, u8g2, display_ext, u8g2_ext, mpu, sspixel)
#include <helpers.h>     // Access to helper functions (getFocusRadius)
#include <lenses.h>      // Access to lens data structure (framelines, name)
#include <formats.h>     // Access to film format data structure (name, frame_fill)
#include <mrfconstants.h> // Access to constants (FWVERSION, RETICLE_OFFSET_X/Y, CALIB_DISTANCES, etc.)
#include <Adafruit_Sensor.h> // For sensors_event_t
#include <math.h>        // For atan2, sqrt, cos, sin, abs


// Functions to draw UI
// ---------------------

/**
 * @brief Draws the main user interface on the primary display.
 */
void drawMainUI()
{
  display.clearDisplay();

  // --- Top Info Bar ---
  u8g2.setFontMode(1);             // Opaque font mode
  u8g2.setFontDirection(0);        // Horizontal text
  u8g2.setForegroundColor(BLACK);  // Black text
  u8g2.setBackgroundColor(WHITE); // White background
  u8g2.setFont(u8g2_font_4x6_mf); // Small font
  display.fillRect(0, 0, SCREEN_WIDTH, 15, WHITE); // White bar background
  display.drawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, 15, BLACK); // Center divider line

  // Left side: ISO, Aperture, Shutter Speed
  u8g2.setCursor(2, 7);
  u8g2.print(F("ISO"));
  u8g2.print(iso);

  u8g2.setCursor(46, 7);
  u8g2.print(F("f"));
  // Print aperture as int if whole number, else with 1 decimal place
  if (aperture == static_cast<int>(aperture))
  {
    u8g2.print(static_cast<int>(aperture));
  }
  else
  {
    u8g2.print(aperture, 1);
  }

  u8g2.setCursor(2, 14);
  u8g2.print(shutter_speed); // Assumes shutter_speed is already formatted String

  // Right side: LiDAR Distance, Lens Distance
  u8g2.setCursor(SCREEN_WIDTH / 2 + 4, 7); // Position relative to center line
  u8g2.print(F("Dist:"));
  u8g2.print(distance_cm); // Assumes distance_cm is already formatted String

  u8g2.setCursor(SCREEN_WIDTH / 2 + 4, 14);
  u8g2.print(F("Lens:"));
  u8g2.print(lens_distance_cm); // Assumes lens_distance_cm is already formatted String

  // Reset colors for main area
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  // --- Framelines and Aspect Ratio Mask ---
  // Ensure selected_lens and selected_format are valid indices
  if (selected_lens >= 0 && selected_lens < (NUM_LENSES) &&
      selected_format >= 0 && selected_format < NUM_FILM_FORMATS)
  {
    // Draw outer frameline rectangle (as a filled rect first, then draw inner mask)
    int frameX = lenses[selected_lens].framelines[0];
    int frameY = lenses[selected_lens].framelines[1];
    int frameW = lenses[selected_lens].framelines[2];
    int frameH = lenses[selected_lens].framelines[3];
    display.fillRect(frameX, frameY, frameW, frameH, WHITE); // White background for frameline area

    // Calculate inner mask dimensions based on film format aspect ratio fill percentage
    // Ensure frame_fill values are within reasonable bounds (e.g., 0-100)
    int fillPercentW = constrain(film_formats[selected_format].frame_fill[1], 0, 100);
    int fillPercentH = constrain(film_formats[selected_format].frame_fill[0], 0, 100);

    int maskW = static_cast<int>(frameW * (fillPercentW / 100.0f));
    int maskH = static_cast<int>(frameH * (fillPercentH / 100.0f));
    int maskX = frameX + (frameW - maskW) / 2;
    int maskY = frameY + (frameH - maskH) / 2;

    // Draw the black inner mask representing the film format area
    display.fillRect(maskX, maskY, maskW, maskH, BLACK);
    // Draw a white border around the mask (optional, for clarity)
    display.drawRect(maskX, maskY, maskW, maskH, WHITE);

    // --- Center Reticle and Focus Indicator ---
    // Calculate the center based on the *outer* framelines for consistent positioning
    int rectCenterX = frameX + frameW / 2 + RETICLE_OFFSET_X;
    int rectCenterY = frameY + frameH / 2 + RETICLE_OFFSET_Y; // Adjusted Y offset? Original had -5

    // Draw center dot
    display.fillCircle(rectCenterX, rectCenterY, 2, WHITE); // Smaller center dot?

    // Draw focus distance indicator circle
    int focusRadius = getFocusRadius(); // Get radius based on distance difference
    display.drawCircle(rectCenterX, rectCenterY, focusRadius, WHITE);

    // --- Horizon Level Indicator (using MPU6050) ---
    sensors_event_t a, g, temp;
    if (mpu.getEvent(&a, &g, &temp)) // Check if getting event was successful
    {
      // Use accelerometer data (more stable for tilt than gyroscope)
      float accX = a.acceleration.x;
      float accY = a.acceleration.y;
      float accZ = a.acceleration.z;

      // Calculate Pitch and Roll angles in radians
      float pitch_rad = atan2(-accX, sqrt(accY * accY + accZ * accZ));
      float roll_rad = atan2(accY, accZ); // Simpler roll calculation if Z is mostly gravity

      // --- Deadzone ---
      const float deadzone_rad = 0.03; // Radians (~1.7 degrees)
      if (abs(pitch_rad) < deadzone_rad) pitch_rad = 0;
      if (abs(roll_rad) < deadzone_rad) roll_rad = 0;

      // --- Scaling for Display ---
      // Adjust scaling factors based on desired sensitivity on screen
      const float pitch_scale = 40; // Pixels per radian for pitch (vertical movement)
      const float roll_scale_factor = 1.0; // Factor for roll (rotation) - adjust if needed

      float pitch_display_offset = pitch_rad * pitch_scale;
      float roll_display_rad = roll_rad * roll_scale_factor;

      // --- Draw Horizon Line ---
      const float line_length = SCREEN_WIDTH * 0.8f; // Make line length relative to screen width

      // Calculate line endpoints based on roll angle and pitch offset
      float startX = rectCenterX - line_length / 2.0f * cos(roll_display_rad);
      float startY = rectCenterY + line_length / 2.0f * sin(roll_display_rad) + pitch_display_offset;
      float endX = rectCenterX + line_length / 2.0f * cos(roll_display_rad);
      float endY = rectCenterY - line_length / 2.0f * sin(roll_display_rad) + pitch_display_offset;

      display.drawLine(static_cast<int16_t>(startX), static_cast<int16_t>(startY),
                       static_cast<int16_t>(endX), static_cast<int16_t>(endY), WHITE);

      // --- Draw Fixed Vertical Center Line ---
      const int vertLineLength = 20; // Shorter vertical line?
      display.drawLine(rectCenterX, rectCenterY - vertLineLength / 2,
                       rectCenterX, rectCenterY + vertLineLength / 2, WHITE);
    }
  } else {
      // Handle invalid lens/format selection - display error message?
      u8g2.setFont(u8g2_font_6x10_mf);
      u8g2.setCursor(10, SCREEN_HEIGHT / 2);
      u8g2.print("Lens/Format Error");
  }

  display.display();
} 

/**
 * @brief Helper function to draw a configuration menu item.
 * @param y The Y cursor position for the item.
 * @param label The text label for the item.
 * @param value The current value string for the item.
 * @param is_selected True if this item is currently selected.
 */
void drawConfigItem(int y, const __FlashStringHelper* label, const String& value, bool is_selected) {
    u8g2.setCursor(3, y);
    if (is_selected) {
        u8g2.setBackgroundColor(WHITE);
        u8g2.setForegroundColor(BLACK);
    } else {
        u8g2.setBackgroundColor(BLACK);
        u8g2.setForegroundColor(WHITE);
    }
    // Print label and value, ensuring consistent spacing/padding if needed
    String itemText = String(label) + value + " "; // Add space for highlight padding
    u8g2.print(itemText);

    // Reset colors after drawing the item
    u8g2.setBackgroundColor(BLACK);
    u8g2.setForegroundColor(WHITE);
}

/**
 * @brief Draws the configuration menu interface on the primary display.
 */
void drawConfigUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  // Title
  u8g2.setFont(u8g2_font_9x15_mf);
  u8g2.drawStr(3, 15, "Setup"); // Use drawStr for constant strings

  // Menu Items (using helper function)
  u8g2.setFont(u8g2_font_6x10_mf); // Slightly larger font for menu items? Original was 4x6
  int currentY = 30; // Starting Y position for items
  int itemHeight = 11; // Height between items

  // Ensure selected_format and selected_lens are valid before accessing data
  String formatName = (selected_format >= 0 && selected_format < NUM_FILM_FORMATS) ? film_formats[selected_format].name : "Error";
  String lensName = (selected_lens >= 0 && selected_lens < (NUM_LENSES)) ? lenses[selected_lens].name : "Error";

  drawConfigItem(currentY, F(" ISO: "), String(iso), config_step == 0);
  currentY += itemHeight;
  drawConfigItem(currentY, F(" Format: "), formatName, config_step == 1);
  currentY += itemHeight;
  drawConfigItem(currentY, F(" Lens: "), lensName, config_step == 2);
  currentY += itemHeight;
  drawConfigItem(currentY, F(" Lens Calib. "), ">", config_step == 3); // Indicate action
  currentY += itemHeight;
  drawConfigItem(currentY, F(" Reset Count "), ">>", config_step == 4); // Indicate action
  currentY += itemHeight;
  drawConfigItem(currentY, F(" Exit "), ">>", config_step == 5); // Indicate action

  // Footer - Firmware Version
  u8g2.setFont(u8g2_font_4x6_mf); // Small font for footer
  u8g2.setCursor(3, SCREEN_HEIGHT - 5); // Position near bottom
  u8g2.print(F("IDENTIDEM.design MRF v"));
  u8g2.print(FWVERSION);

  display.display();
}

/**
 * @brief Draws the lens calibration interface on the primary display.
 */
void drawCalibUI()
{
  display.clearDisplay();

  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  u8g2.setForegroundColor(WHITE);
  u8g2.setBackgroundColor(BLACK);

  // Title
  u8g2.setFont(u8g2_font_6x10_mf);
  u8g2.drawStr(3, 15, "Calibrate Lens");

  // Ensure calib_lens is valid
  if (calib_lens < 0 || calib_lens >= (NUM_LENSES)) {
      u8g2.setFont(u8g2_font_6x10_mf);
      u8g2.drawStr(3, 40, "Lens Error!");
      display.display();
      return;
  }

  u8g2.setFont(u8g2_font_6x10_mf); // Consistent font size

  // Step 0: Select Lens
  u8g2.setCursor(3, 35);
  if (calib_step == 0) {
      u8g2.setBackgroundColor(WHITE);
      u8g2.setForegroundColor(BLACK);
  } else {
      u8g2.setBackgroundColor(BLACK);
      u8g2.setForegroundColor(WHITE);
  }
  u8g2.print(F(" Lens: "));
  u8g2.print(lenses[calib_lens].name);
  u8g2.print(F(" ")); // Padding for highlight

  // Step 1: Set Calibration Point
  u8g2.setCursor(3, 47);
  if (calib_step == 1) {
      u8g2.setBackgroundColor(WHITE);
      u8g2.setForegroundColor(BLACK);
  } else {
      u8g2.setBackgroundColor(BLACK);
      u8g2.setForegroundColor(WHITE);
  }
  // Ensure current_calib_distance is valid
  const size_t num_calib_distances = sizeof(CALIB_DISTANCES) / sizeof(CALIB_DISTANCES[0]);
  if (current_calib_distance >= 0 && current_calib_distance < num_calib_distances) {
      u8g2.print(F(" Set "));
      u8g2.print(CALIB_DISTANCES[current_calib_distance], 1); // Show target distance
      u8g2.print(F("m: "));
      u8g2.print(lens_sensor_reading); // Show current reading
      u8g2.print(F(" ")); // Padding
  } else {
      u8g2.print(F(" Done? ")); // Or error state
  }

  // Instructions
  u8g2.setBackgroundColor(BLACK);
  u8g2.setForegroundColor(WHITE);
  u8g2.setFont(u8g2_font_4x6_mf); // Smaller font for instructions
  int instructionY = SCREEN_HEIGHT - 20;
  if (calib_step == 0)
  {
    u8g2.drawStr(3, instructionY, "(L) Cycle Lens");
    u8g2.drawStr(3, instructionY + 7, "(R) Select Lens");
  }
  else // calib_step == 1
  {
    u8g2.drawStr(3, instructionY, "(L) Set Point");
    u8g2.drawStr(3, instructionY + 7, "(R) Cancel/Exit");
  }

  display.display();
}

/**
 * @brief Draws the user interface on the external (secondary) display.
 */
void drawExternalUI()
{
  // Define constants for layout
  const int TOP_BAR_HEIGHT = 10;
  const int PROGRESS_BAR_WIDTH = 90;
  const int PROGRESS_BAR_HEIGHT = 17;
  const int PROGRESS_BAR_X = 34;
  const int PROGRESS_BAR_Y = 15;

  display_ext.clearDisplay();

  // --- Top Info Bar ---
  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(BLACK);
  u8g2_ext.setBackgroundColor(WHITE);
  u8g2_ext.setFont(u8g2_font_6x10_mf);

  display_ext.fillRect(0, 0, SCREEN_WIDTH, TOP_BAR_HEIGHT, WHITE); // White background

  // Format Name
  u8g2_ext.setCursor(2, 8);
  String formatName = (selected_format >= 0 && selected_format < NUM_FILM_FORMATS) ? film_formats[selected_format].name : "ERR";
  u8g2_ext.print(formatName);

  // Divider 1
  display_ext.drawLine(33, 0, 33, TOP_BAR_HEIGHT, BLACK);

  // Lens Name
  u8g2_ext.setCursor(37, 8);
  String lensName = (selected_lens >= 0 && selected_lens < (NUM_LENSES)) ? lenses[selected_lens].name : "ERR";
  u8g2_ext.print(lensName);

  // Divider 2 (dynamic position based on battery %)
  int batDividerX = 103; // Default position
  int batTextX = 107;    // Default position
  if (bat_per == 100) { batDividerX = 100; batTextX = 104; }
  else if (bat_per < 10) { batDividerX = 111; batTextX = 115; }
  display_ext.drawLine(batDividerX, 0, batDividerX, TOP_BAR_HEIGHT, BLACK);

  // Battery Percentage
  u8g2_ext.setCursor(batTextX, 8);
  u8g2_ext.print(bat_per);
  u8g2_ext.print(F("%"));

  // --- Main Area: Film Counter / Progress ---
  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);

  // Frame Progress Bar (only if progress > 0)
  if (frame_progress > 0.0f && frame_progress < 1.0f) // Show only when actively progressing
  {
    float progressPercentage = constrain(frame_progress * 100.0f, 0.0f, 100.0f);
    int progressWidth = static_cast<int>(PROGRESS_BAR_WIDTH * (progressPercentage / 100.0f));

    display_ext.drawRect(PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT, WHITE);
    display_ext.fillRect(PROGRESS_BAR_X, PROGRESS_BAR_Y, progressWidth, PROGRESS_BAR_HEIGHT, WHITE);

    // Update NeoPixel based on progress (Red to Green transition)
    // Ensure frame_progress is clamped 0.0 to 1.0
    float clamped_progress = constrain(frame_progress, 0.0f, 1.0f);
    int greenValue = static_cast<int>(clamped_progress * 255);
    int redValue = static_cast<int>((1.0f - clamped_progress) * 255);
    sspixel.setPixelColor(0, sspixel.Color(redValue, greenValue, 0));

  } else {
      // Set NeoPixel to default color when not progressing (or exactly on frame)
      // Blue for 'on frame', Violet for 'load/end'
      if (film_counter == 0 || film_counter == 99) {
          sspixel.setPixelColor(0, sspixel.Color(238, 130, 238)); // Violet
      } else {
          sspixel.setPixelColor(0, sspixel.Color(0, 0, 255)); // Blue
      }
  }

  // Film Counter Text
  u8g2_ext.setFont(u8g2_font_10x20_mf); // Large font for counter
  int textY = PROGRESS_BAR_Y + PROGRESS_BAR_HEIGHT / 2 + 6; // Center text vertically relative to progress bar area

  if (film_counter == 0 && frame_progress == 0.0f)
  {
    u8g2_ext.drawStr(8, textY, "Load film.");
  }
  else if (film_counter == 99)
  {
    u8g2_ext.drawStr(8, textY, "Roll end.");
  }
  else if (frame_progress > 0.0f && frame_progress < 1.0f)
  {
      // Show the *previous* frame number small while progressing
      u8g2_ext.setFont(u8g2_font_6x10_mf); // Use a smaller font
      String prevCounterStr = String(film_counter);
      int textWidth = u8g2_ext.getUTF8Width(prevCounterStr.c_str());
      // Position it, for example, bottom-right below the progress bar
      int textX = PROGRESS_BAR_X + PROGRESS_BAR_WIDTH - textWidth - 1; // Align right
      int textY = PROGRESS_BAR_Y + PROGRESS_BAR_HEIGHT + 8; // Below progress bar
      u8g2_ext.setCursor(textX, textY);
      u8g2_ext.print(prevCounterStr);
  }
  else // On a frame mark (progress is 0 or 1)
  {
      // Center the frame number text horizontally
      String counterStr = String(film_counter);
      int textWidth = u8g2_ext.getUTF8Width(counterStr.c_str());
      int textX = (SCREEN_WIDTH - textWidth) / 2;
      u8g2_ext.setCursor(textX, textY);
      u8g2_ext.print(counterStr);
  }

  sspixel.show(); // Update NeoPixel
  display_ext.display();
}

/**
 * @brief Draws the sleep mode indicator on both displays.
 */
void drawSleepUI()
{
  // Clear both displays
  display.clearDisplay();
  display_ext.clearDisplay();

  // Draw "Zzz" on external display (usually the smaller one)
  u8g2_ext.setFontMode(1);
  u8g2_ext.setFontDirection(0);
  u8g2_ext.setForegroundColor(WHITE);
  u8g2_ext.setBackgroundColor(BLACK);
  u8g2_ext.setFont(u8g2_font_10x20_mf); // Use a reasonably large font

  const char* sleepText = "ZzzZZzZz...";
  int textWidth = u8g2_ext.getUTF8Width(sleepText);
  int textX = (SCREEN_WIDTH - textWidth) / 2; // Center horizontally
  int textY = SCREEN_HEIGHT_EXT / 2 + 7; // Center vertically (adjust baseline)
  u8g2_ext.drawStr(textX, textY, sleepText);

  // Turn off NeoPixel
  sspixel.setPixelColor(0, sspixel.Color(0, 0, 0));
  sspixel.show();

  // Update both displays
  display.display();
  display_ext.display();
}

// ---------------------