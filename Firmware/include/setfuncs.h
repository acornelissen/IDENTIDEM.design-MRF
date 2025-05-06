#ifndef SETFUNCS_H_
#define SETFUNCS_H_

#include <Arduino.h> 

// Functions to read values from sensors and set variables
// ---------------------
void setDistance();
int getLensSensorReading();
void setLensDistance();
void setFilmCounter();
void setVoltage();
void setLightMeter();
void toggleLidar(bool lidarStatus);
// ---------------------

#endif // SETFUNCS_H_