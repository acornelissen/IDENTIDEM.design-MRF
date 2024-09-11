// Constants
#define FWVERSION "1.5"
#define SLEEPTIMEOUT 60000
#define DEEPSLEEP_ENABLED false

#define RXD2 RX
#define TXD2 TX


#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 128        // OLED display height, in pixels
#define OLED_RESET -1           // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D     ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS_EXT 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

const int SMOOTHING_WINDOW_SIZE = 25;

#define RETICLE_OFFSET_X -7
#define RETICLE_OFFSET_Y 3
#define CLOSE_FOCUS 100
#define LIDAR_OFFSET 10

#define DISTANCE_MIN 15
#define DISTANCE_MAX 12

const int ISOS[] = {800};
const float CALIB_DISTANCES[] = {1, 1.2, 1.5, 2, 3, 5, 10};

const int K = 20;