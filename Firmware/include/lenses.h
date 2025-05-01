#ifndef LENSES_H
#define LENSES_H

#include <Arduino.h> // For String type

// Define constants for array sizes within the struct for clarity and safety
constexpr size_t NUM_CALIBRATION_POINTS = 7;
constexpr size_t NUM_APERTURES = 9;
constexpr size_t NUM_FRAMELINE_PARAMS = 4;

struct Lens
{
  int id;
  String name;
  int sensor_reading[NUM_CALIBRATION_POINTS];
  float distance[NUM_CALIBRATION_POINTS]; // Corresponds to sensor_reading points
  float apertures[NUM_APERTURES];         // Available aperture values (0 used as placeholder)
  int framelines[NUM_FRAMELINE_PARAMS];   // {x, y, width, height} for display
  bool calibrated;
};

// Declare the array of lenses as extern; the definition will be in lenses.cpp
extern Lens lenses[];
// Declare a constant for the number of lenses
extern const size_t NUM_LENSES;

#endif // LENSES_H