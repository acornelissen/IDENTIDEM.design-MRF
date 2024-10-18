// Hardware init
// ---------------------
// Inputs

Bounce2::Button lbutton = Bounce2::Button();
Bounce2::Button rbutton = Bounce2::Button();

// Battery gauge
Adafruit_MAX17048 maxlipo;

// Lightmeter
BH1750 lightMeter;

// LiDAR setup
TFMPlus tfminiplus;
HardwareSerial lidarSerial(2); // Using serial port 2

// Display setup
Adafruit_SH1107 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 1000000);
U8G2_FOR_ADAFRUIT_GFX u8g2;
// ---------------------