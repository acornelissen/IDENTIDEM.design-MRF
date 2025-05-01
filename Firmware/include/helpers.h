#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h> // For String, int_fast16_t etc.

// Helper functions
// ---------------------

int getFirstNonZeroAperture(); 

void loadPrefs();

void savePrefs();

String cmToReadable(int cm, int places);

int calcMovingAvg(int index, int sensorVal);

int_fast16_t getFocusRadius();
// ---------------------

#endif // HELPERS_H