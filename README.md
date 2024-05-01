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

- TFMini Plus "LiDAR" sensor -> for measuring distance to subject
- Adafruit Monochrome 1.12" 128x128 OLED - STEMMA I2C at 0x3D (Product ID: 5297) -> for viewfinder HUD display
- 124x32 OLED I2C display at 0x3C (I use these https://www.amazon.co.uk/gp/product/B079H2C7WH/) -> external display for displaying frame counter and some other info
- Adafruit BH1750 Light Sensor (Product ID: 4681) -> for the lightmeter
- Sakae / Caldaro S8FLP-10A-10K linear position sensor -> this couples to the Mamiya Press Lens rangefinder mechanism to measure lens position (https://store.technimeasure.co.uk/product/s8flp-10a-10k-%c2%b11/)
- Adafruit ADS1115 16-Bit ADC - STEMMA I2C (Product ID: 1085) -> Give us the precision and stability we need for reading the lens position from the linear position sensor
- Adafruit Seesaw Rotary Encoder breakout - STEMMA I2C  (Product ID: 4991)
- PEC11-4215F-S24 Rotary Encoder (6mm D-shaft)
- Two push buttons for navigation (https://www.amazon.co.uk/gp/product/B07S1MNB8C)
- A power button (I use a latched push-button type: https://www.amazon.co.uk/gp/product/B0BZJBNLCF)

