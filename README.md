# IDENTIDEM.design M(edium Format) (R)ange(F)inder Firmware

### This is a work in progress, but it's very usable in its current state.

It basically just needs a little bit of cleanup and organisation.
The design files aren't quite ready yet either, I'm cleaning them up and they should be ready soon. The build guide will follow.

### Ok, with that out of the way

This is a Arduino C++ rewrite of the LRF45 CircuitPython code that was adapted for use in my medium format LiDAR rangefinder. 
With the amount of sensors I am using on that camera, as well as needing to drive two external displays, the CircuitPython performancer was not acceptable, even running on a better MCU.

This code was built for the Adafruit Feather ESP32-S3 (I am using the 4MB Flash 2MB PSRAM version, but the one without PSRAM should work just as well). It's a nice board with a built in LiPO charger and battery monitor, 
so no need to faff about with voltage dividers and guesstimate how much battery is left. 

It requires the following hardwaren (for now), in addition to the Feather:

- TFLuna/TFMini/TFMini Plus "LiDAR" sensor -> for measuring distance to subject
- 128x128 OLED I2C display at 0x3D -> for viewfinder HUD display
- 124x32 OLED I2C display at 0x3C -> external display for displaying frame counter and some other info
- BH1750 Lux Sensor -> for the lightmeter
- Sakae / Caldaro S8FLP-10A-10K linear position sensor -> this couples to the Mamiya Press Lens rangefinder mechanism to measure lens position
- Adafruit Seesaw STEMMA I2C Rotary Encoder breakout (Product code 4991)
- PEC11 Rotary Encoder (6mm D-shaft)
- Two push buttons for navigation

