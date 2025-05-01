#ifndef INTERFACE_H
#define INTERFACE_H

#include <Arduino.h> // Include if any Arduino specific types are needed in declarations

// Functions to draw UI
// ---------------------
/**
 * @brief Draws the main user interface on the primary display.
 */
void drawMainUI();

/**
 * @brief Draws the configuration menu interface on the primary display.
 */
void drawConfigUI();

/**
 * @brief Draws the lens calibration interface on the primary display.
 */
void drawCalibUI();

/**
 * @brief Draws the user interface on the external (secondary) display.
 */
void drawExternalUI();

/**
 * @brief Draws the sleep mode indicator on both displays.
 */
void drawSleepUI();
// ---------------------

#endif // INTERFACE_H