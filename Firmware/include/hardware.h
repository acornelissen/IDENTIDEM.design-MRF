#ifndef HARDWARE_H_
#define HARDWARE_H_

// These includes provide the declarations for the types used below
#include <Adafruit_seesaw.h>
#include <seesaw_neopixel.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_MAX1704X.h>
#include <BH1750.h>
#include <TFMPlus.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <HardwareSerial.h>

// Hardware init
// ---------------------
// Inputs
extern Adafruit_seesaw encoder;
extern Adafruit_ADS1115 ads1015;
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
extern HardwareSerial lidarSerial;

// Display setup
extern Adafruit_SH1107 display;
extern Adafruit_SSD1306 display_ext;
extern U8G2_FOR_ADAFRUIT_GFX u8g2;
extern U8G2_FOR_ADAFRUIT_GFX u8g2_ext;
// ---------------------

#endif // HARDWARE_H_