#include <hardware.h> // Include the header with extern declarations

// Hardware object definitions
// ---------------------
// Inputs
Adafruit_seesaw encoder;
Adafruit_ADS1115 ads1115;
Adafruit_MPU6050 mpu;

// Initialize NeoPixel on the seesaw chip
seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

// Initialize Bounce2 buttons
Bounce2::Button lbutton = Bounce2::Button();
Bounce2::Button rbutton = Bounce2::Button();

// Battery gauge
Adafruit_MAX17048 maxlipo;

// Lightmeter
BH1750 lightMeter; // Use default address or specify if different

// LiDAR setup
TFMPlus tfminiplus;
HardwareSerial lidarSerial(2); // Define and initialize Serial Port 2 for LiDAR

// Display setup
Adafruit_SH1107 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 1000000, 1000000); // High speed I2C
Adafruit_SSD1306 display_ext(SCREEN_WIDTH, SCREEN_HEIGHT_EXT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2;
U8G2_FOR_ADAFRUIT_GFX u8g2_ext;
// ---------------------