// Constants
#define FWVERSION "4.5"
#define SLEEPTIMEOUT 60000
#define DEEPSLEEP_ENABLED false

#define RXD2 RX
#define TXD2 TX

#define SS_SWITCH 24
#define SS_NEOPIX 6
#define SEESAW_ADDR 0x36 // Set this to the address of the seesaw

#define LENS_ADC_PIN = 0; // Set this to the pin you've soldered the lens position sensor to on the ADS1115

#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 128        // OLED display height, in pixels
#define SCREEN_HEIGHT_EXT 32    // OLED display height, in pixels
#define OLED_RESET -1           // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D     ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS_EXT 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

const int SMOOTHING_WINDOW_SIZE = 10;

#define CIRCLE_X 33
#define CIRCLE_Y 82
#define CIRCLE_X_MAX 48
#define CIRCLE_Y_MAX 92
#define CIRCLE_X_CAP 43
#define CIRCLE_Y_CAP 84
#define CIRCLE_MAX_DIST 800
#define CLOSE_FOCUS 100
#define LIDAR_OFFSET 10

#define DISTANCE_MIN 15
#define DISTANCE_MAX 12

const int ISOS[] = {50, 80, 100, 125, 200, 400, 500, 640, 800, 1600, 3200, 6400};
const float CALIB_DISTANCES[] = {1, 1.2, 1.5, 2, 3, 5, 10};

const int K = 20;