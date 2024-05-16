# IDENTIDEM.design M(edium Format) (R)ange(F)inder Firmware

**Before anything else, a big thank you to Oscar from Panomicron for making the CAD files for his [Thulium 6x9](https://www.panomicron.com/thulium) available for download. This camera is a heavily modified version of the Thulium, and wouldn't exist if it wasn't for Oscar's excellent work.**

My goal for this camera was to build _my_ ideal medium format rangefinder using what I learned from designing and building the LRF45, with no limits on complexity, cost, and time. As I kept working on it, it evolved and evolved, and I think I've finally reached that goal.

The detailed build guide should drop soon(ish). For now though, everything you need to build one of your own modern medium format rangefinders is in this repository.

## Features
- Uses "LiDAR" to measure distance to subject (not true LiDAR, but a ToF sensor that uses a focused IR LED array) 
- Can be used with any Mamiya Universal Press lens and couples to the original lens rangefinder mechanism with a high-precision linear position sensor
- This means that the lens position and distance measured is electronically coupled, making this the first multi-lens, electronically coupled rangefinder that I know of
- In addition, there is an intuitive in-camera lens calibration routine to make sure your camera and lens work together perfectly
- Multi-format, with masks for 6x7, 6x6, 6x4.5 and 35mm Panoramics
- Uses a high-precision rotary encoder coupled to the take-up spool to count revolutions, which is then used to accurately count frames regardless of selected format
- Built-in lightmeter
- Built-in accelerometer / gyro
- In-viewfinder OLED display with:
  - Lightmeter information: Selected ISO, selected aperture, and calculated shutter speed
  - Lens focus distance
  - Distance to subject
  - Frame lines that adjust according to selected lens
  - Masking that adjusts to selected format
  - Reticle for focus that indicates focus accuracy as lens focus distance and distance to subject converges
  - Horizon indicator
- External OLED display with:
  - Frame counter
  - Battery life percentage
  - Selected format
  - Selected lens
- Open source! 


## Bill of Materials
See the Build Guide PDF for the BoM. 

---
🍺 <strong><a href="https://paypal.me/albertcor">Buy me a beer</a></strong> if you'd like to say thank you.
