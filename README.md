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
### 3D printed parts
#### Main parts
- Mamiya Press Lens Cone
- Main Body
- Body door
- Top plate
- Bottom plate
- Grip (optional)
- Viewfinder
- Viewfinder cover
- Advance knob

#### Fine parts
- Right spool
- Left spool
- Advance post
- Advance ratchet
- Advance plate
- Pressure plate
- Power button plate
- Sensor nubbin
- Spool key (optional)

#### Film masks (print at least the 6x7 mask)
- 6x7
- 6x6
- 6x6 Circle
- 6x4.5
- 35mm Panoramic (use 35mm to 120 film spool adapters with this mask)

### Optics
- 20mm Dia. x 70mm FL Plano-Convex Lens -> https://www.edmundoptics.co.uk/p/200mm-dia-x-700mm-fl-uncoated-plano-convex-lens/2705/
- 25mm Dia. x -25 FL Plano-Concave Lens -> https://www.edmundoptics.co.uk/p/250mm-dia-x25-fl-uncoated-plano-concave-lens/5542/
- 30 x 30 x 1.2mm plate beam splitter (70R/30T is ideal, but in a pinch 50R/50T will do) -> https://www.ebay.co.uk/itm/225028201670 (or message Bohr Optics directly if that link is dead)
- At least one Mamiya Press Lens - I'd recommend starting with the 65mm/f6.3 or 100mm/f3.5

### Electronics
**Be aware that you will need to solder some components.**

- 1 x Adafruit ESP32-S3 Feather (any variant will do)
- 1 x TFMini Plus "LiDAR" sensor
- 1 x Adafruit Monochrome 1.12" 128x128 OLED - STEMMA QT I2C at 0x3D (Product ID: 5297)
- 1 x Generic 124x32 OLED I2C display at 0x3C -> https://www.amazon.co.uk/gp/product/B079H2C7WH/
- 1 x Adafruit BH1750 Light Sensor (Product ID: 4681)
- 1 x Sakae / Caldaro S8FLP-10A-10K linear position sensor -> https://store.technimeasure.co.uk/product/s8flp-10a-10k-%c2%b11
- 1 x Adafruit ADS1115 16-Bit ADC - STEMMA QT I2C (Product ID: 1085)
- 1 x Adafruit MPU-6050 6-DoF Accel and Gyro Sensor - STEMMA QT I2C (Product ID: 3886)
- 1 x Adafruit Seesaw Rotary Encoder breakout - STEMMA QT I2C  (Product ID: 4991)
- 1 x PEC11-4215F-S24 Rotary Encoder (6mm D-shaft)
- 2 x Push buttons -> https://www.amazon.co.uk/gp/product/B07S1MNB8C
- 1 x power button -> https://www.amazon.co.uk/gp/product/B0BZJBNLCF
- 1 x 3.7v 820mAh LiPo battery - 30.5 x 44 x 6.8mm (CHECK THE POLARITY OF THE CABLE BEFORE CONNECTING TO THE FEATHER) -> https://www.amazon.co.uk/gp/product/B082152887
- 1 x Adafruit MiniBoost 5V @ 1A - TPS61023 (Product ID: 4654)
- 1 x MRF Daughterboard (optional, but makes things much tidier) -> see PCB/KiCAD folder, you'll need to order this from PCBWay or JLCPCB
- 1 x set of male and female JST-XH 6-pin connectors
- 1 x set of male and female JST-XH 4-pin connectors
- 1 x 30cm STEMMA QT cable and 5 x 10cm STEMMA QT cables

### Hardware
- 11 x M3x4 heat-set inserts
- 5 x M3x4 screws
- 4 x M3x6 screws
- 2 x M3x4 thumb-screw
- 15 x M2x4 heat-set inserts
- 15 x M2x6 screws
- 15 x M2x4 screws
- 2 x 20 x 5 x 2mm magnets for the body (and 2 for each film mask you print)
- 1 x 1.5mm stainless steel or brass rod, cut to 82mm (for the film door)