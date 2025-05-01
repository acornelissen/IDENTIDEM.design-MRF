#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_seesaw.h>
#include <Adafruit_NeoPixel.h> // For NEO_GRB, NEO_KHZ800
#include <seesaw_neopixel.h>
#include <Adafruit_ADS1X15.h> // Includes ADS1115
#include <Adafruit_MPU6050.h>
#include <Bounce2.h>
#include <Adafruit_MAX1704X.h> // Includes MAX17048
#include <BH1750.h>
#include <TFMPlus.h>
#include <Adafruit_SH110X.h> // Includes SH1107
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <mrfconstants.h> // For SCREEN_WIDTH, SCREEN_HEIGHT etc.

// Hardware init
// ---------------------
// Inputs
extern Adafruit_seesaw encoder;
extern Adafruit_ADS1115 ads1115;
extern Adafruit_MPU6050 mpu;
extern seesaw_NeoPixel sspixel;
extern Bounce2::Button lbutton;
extern Bounce2::Button rbutton;

// Battery gauge
extern Adafruit_MAX17048 maxlipo;

// Lightmeter
extern BH1750 lightMeter;

// LiDAR setup
extern TFMPlus tfminiplus;
extern HardwareSerial lidarSerial; // Declaration only

// Display setup
extern Adafruit_SH1107 display;
extern Adafruit_SSD1306 display_ext;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_ext;
// ---------------------

#endif // HARDWARE_H