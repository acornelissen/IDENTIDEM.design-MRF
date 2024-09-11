// IDENTIDEM.design (M)edium Format (R)ange(F)inder firmware vPro.3.0
//
// Pro version of MRF firmware uses
// - Adafruit STEMMA I2C QT Rotary Encoder breakout 4991
// - TFMini Plus LiDAR sensor
// - Bigger optics

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MAX1704X.h>
#include <TFMPlus.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <seesaw_neopixel.h>
#include <Preferences.h>
#include <math.h>

#include "driver/rtc_io.h"
#include "esp_wifi.h"
#include "esp_bt.h"

//Constants and variables
#include <mrfconstants.h>
#include <lenses.h>
#include <formats.h>
#include <globals.h>

// Init hardware
#include <hardware.h>

// Functions
#include <helpers.h>
#include <cyclefuncs.h>
#include <setfuncs.h>
#include <interface.h>
#include <inputs.h>

// Setup and loop functions
// ---------------------
void setup()
{

  Serial.begin(115200); // Initializing serial port

  esp_wifi_stop(); // Stop WiFi to save power
  esp_bt_controller_disable(); // Stop Bluetooth to save power

  loadPrefs();

  // Initialise inputs
  lbutton.attach(9, INPUT_PULLUP);
  lbutton.interval(5);
  lbutton.setPressedState(LOW);
  rbutton.attach(10, INPUT_PULLUP);
  rbutton.interval(5);
  rbutton.setPressedState(LOW);

  delay(1000); // Slight delay or the displays won't work
  display.begin(0x3D, true); // Address 0x3D default
  display.oled_command(0xC8);
  display.setRotation(1);
  u8g2.begin(display);
  display.clearDisplay();
  display.display();


  // Start the LiDAR sensor
  lidarSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(20);
  tfminiplus.begin(&lidarSerial);

  // Clear the moving average arrays
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++)
  { 
    samples[0][i] = 0;
    samples[1][i] = 0;
  }

  // Start the battery gauge and lightmeter
  maxlipo.begin();

  // if (DEEPSLEEP_ENABLED == true) {
  //   esp_sleep_enable_ext0_wakeup(GPIO_NUM_10,0);
  //   rtc_gpio_pullup_en(GPIO_NUM_10);
  //   rtc_gpio_pulldown_dis(GPIO_NUM_10);
  // }
}

void loop()
{
  
  // if (millis() - lastActivityTime > SLEEPTIMEOUT) { // Step 3
  //   sleepMode = true;
  // }

  checkButtons();


  if (sleepMode == true)
  {
    toggleLidar();
    drawSleepUI(1);
  }
  else { 
    toggleLidar();
    
    if (ui_mode == "main")
      { 
        setDistance();
        setLensDistance();
        setVoltage();
        drawMainUI();
      }
      else if (ui_mode == "config")
      {
        drawConfigUI();
      }
      else if (ui_mode == "calib")
      {
        drawCalibUI();
      }
  }
}
// ---------------------