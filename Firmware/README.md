
This is a Arduino C++ rewrite of the LRF45 CircuitPython code that was adapted for use in my medium format LiDAR rangefinder. 
With the amount of sensors I am using on this camera, as well as needing to drive two displays, the CircuitPython performance was not acceptable, even running on a better MCU.

This code was built for the Adafruit Feather ESP32-S3 (I am using the 4MB Flash 2MB PSRAM version, but the one without PSRAM should work just as well). It's a nice board with a built in LiPO charger and battery monitor, 
so no need to faff about with voltage dividers and guesstimate how much battery is left. 
