#ifndef FORMATS_H
#define FORMATS_H

#include <Arduino.h> // For String type

struct FilmFormat
{
  int id;
  String name;
  int sensor[22]; 
  int frame[22];  
  int frame_fill[2];
};

// Declare the array as extern, the actual data will be in formats.cpp
extern FilmFormat film_formats[];
// Declare a constant for the number of formats
extern const size_t NUM_FILM_FORMATS;

#endif // FORMATS_H
