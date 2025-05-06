#ifndef HELPERS_H_
#define HELPERS_H_

#include <Arduino.h> // For String, int_fast16_t (from stdint.h, often included by Arduino.h)
#include <stdint.h> // For int_fast16_t

// Helper functions
// ---------------------
float getFirstNonZeroAperture();
void loadPrefs();
void savePrefs();
String cmToReadable(int cm);
int calcMovingAvg(int index, int sensorVal);
int getLensSensorReading(); // Declaration for function in helpers.cpp
int_fast16_t getFocusRadius(); // Declaration for function in helpers.cpp
void enableInternalPower();
void disableInternalPower();
// ---------------------

#endif // HELPERS_H_