#ifndef SETFUNCS_H
#define SETFUNCS_H

#include <Arduino.h> // Include if any Arduino specific types are needed in declarations

// Functions to read values from sensors and set variables
// ---------------------
/**
 * @brief Reads distance from LiDAR sensor, applies offset, updates global variables.
 */
void setDistance();

/**
 * @brief Reads the lens ADC pin, applies scaling, and returns a moving average.
 * @return Smoothed lens sensor reading.
 */
int getLensSensorReading();

/**
 * @brief Reads lens sensor, interpolates distance based on calibration, updates globals.
 */
void setLensDistance();

void setFilmCounter();

void setVoltage();

void setLightMeter();

void toggleLidar(bool lidarStatus);
// ---------------------

#endif // SETFUNCS_H