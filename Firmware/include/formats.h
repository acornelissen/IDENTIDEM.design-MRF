#ifndef FORMATS_H_
#define FORMATS_H_

#include <Arduino.h> // For String type used in struct

struct FilmFormat
{
  int id;
  String name;
  int sensor[22];
  int frame[22];
  int frame_fill[2];
};

extern FilmFormat film_formats[];
extern const int FILM_FORMATS_SIZE;

#endif // FORMATS_H_