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
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TFMPlus.h>
#include <BH1750.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <Adafruit_seesaw.h>
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
  ads1115.begin();
  mpu.begin();

  ads1115.setDataRate(RATE_ADS1115_16SPS);
  ads1115.setGain(GAIN_TWOTHIRDS); 
  lbutton.attach(10, INPUT_PULLUP);
  lbutton.interval(5);
  lbutton.setPressedState(LOW);
  rbutton.attach(9, INPUT_PULLUP);
  rbutton.interval(5);
  rbutton.setPressedState(LOW);

  delay(1000); // Slight delay or the displays won't work
  display.begin(0x3D, true); // Address 0x3D default
  display.oled_command(0xC8);
  display.setRotation(3);
  u8g2.begin(display);
  display.clearDisplay();
  display.display();

  delay(500); // Slight delay or the displays won't work

  display_ext.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS_EXT);

  // Boot up sceen
  display_ext.clearDisplay();
  display_ext.setTextSize(2); // Draw 2X-scale text
  display_ext.setTextColor(SSD1306_WHITE);
  display_ext.setCursor(20, 10);
  display_ext.print(F("MRF "));
  display_ext.println(FWVERSION);
  display_ext.display();

  delay(1000);

  u8g2_ext.begin(display_ext);
  display_ext.clearDisplay();
  display_ext.display();

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
  lightMeter.begin();

  // Seesaw NEOpixel
  if (sspixel.begin(SEESAW_ADDR)) {
    delay(10);
    sspixel.setBrightness(80);
    sspixel.show();
  }


  // Start the encoder
  if (encoder.begin(SEESAW_ADDR)) {
    delay(10);
    encoder.setEncoderPosition(encoder_value);
    encoder.enableEncoderInterrupt();
  }

}

void loop()
{
  
  if (millis() - lastActivityTime > SLEEPTIMEOUT) {
    sleepMode = true;
  }

  checkButtons();
  setFilmCounter();

  if (sleepMode == true)
  {
    toggleLidar();
    drawSleepUI();
  }
  else { 
    toggleLidar();
    
    if (ui_mode == "main")
      { 
        setLensDistance();
        setDistance();
        setVoltage();
        setLightMeter();
        drawMainUI();
      }
      else if (ui_mode == "config")
      {
        drawConfigUI();
      }
      else if (ui_mode == "calib")
      {
        lens_sensor_reading = getLensSensorReading() - LENS_CALIB_OFFSET;
        drawCalibUI();
      }
      drawExternalUI();
  }
}
// ---------------------