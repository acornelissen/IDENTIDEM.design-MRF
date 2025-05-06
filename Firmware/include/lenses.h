#ifndef LENSES_H_
#define LENSES_H_

#include <Arduino.h> // For String type

struct Lens
{
  int id;
  String name;
  int sensor_reading[7];
  float distance[7];
  float apertures[9];
  int framelines[4];
  bool calibrated;
};

extern Lens lenses[];
extern const int LENSES_SIZE;

#endif // LENSES_H_