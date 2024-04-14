#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX1704X.h>
#include <TFMPlus.h>
#include <BH1750.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Bounce2.h>
#include <ESP32Encoder.h>
#include <Preferences.h>

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

  loadPrefs();

  // Initialise inputs
  pinMode(A1, INPUT);
  lbutton.attach(9, INPUT_PULLUP);
  lbutton.interval(5);
  lbutton.setPressedState(LOW);
  rbutton.attach(10, INPUT_PULLUP);
  rbutton.interval(5);
  rbutton.setPressedState(LOW);

  delay(1000); // Slight delay or the displays won't work
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  Wire.beginTransmission(SCREEN_ADDRESS); // START I2C COMMUNICATION WITH OLED (0x3c=OLED ADDRESS)
  Wire.write(0x80);
  Wire.write(0xC0); // COM SCAN DIRECTION  0XC0 - Reverse scan direction, OxC8 - Normal scan direction
  Wire.endTransmission();
  display.setRotation(1);
  u8g2.begin(display);
  display.clearDisplay();
  display.display();

  delay(1000); // Slight delay or the displays won't work
  display_ext.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS_EXT);

  // Boot up screen
  display_ext.clearDisplay();
  display_ext.setTextSize(2); // Draw 2X-scale text
  display_ext.setTextColor(SSD1306_WHITE);
  display_ext.setCursor(20, 10);
  display_ext.println(F("MRF v2.5"));
  display_ext.display();

  delay(1500);
  u8g2_ext.begin(display_ext);
  display_ext.clearDisplay();
  display_ext.display();

  // Start the LiDAR sensor
  lidarSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(20);
  tfluna.begin(&lidarSerial);

  // Clear the moving average arrays
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++)
  {
    samples[0][i] = 0;
    samples[1][i] = 0;
  }

  // Start the battery gauge and lightmeter
  maxlipo.begin();
  lightMeter.begin();

  // Start the encoder
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoder.attachSingleEdge(13, 12);
  encoder.setFilter(1023);
  encoder.setCount(encoder_value);
}

void loop()
{
  checkButtons();

  if (ui_mode == "main")
  {
    setDistance();
    setLensDistance();
    setVoltage();
    setLightMeter();
    drawMainUI();
    setFilmCounter();
  }
  else if (ui_mode == "config")
  {
    drawConfigUI();
  }
  else if (ui_mode == "calib")
  {
    drawCalibUI();
  }

  drawExternalUI();
}
// ---------------------