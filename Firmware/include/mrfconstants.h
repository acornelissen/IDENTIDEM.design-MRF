#ifndef MRFCONSTANTS_H_
#define MRFCONSTANTS_H_

// Constants
#define FWVERSION "5.0" // From original file
#define SLEEPTIMEOUT 60000

#define RXD2 RX // These were commented out, kept as such
#define TXD2 TX

#define SS_NEOPIX 6
#define SEESAW_ADDR 0x36

#define LENS_ADC_PIN 0

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define SCREEN_HEIGHT_EXT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D
#define SCREEN_ADDRESS_EXT 0x3C

#define SMOOTHING_WINDOW_SIZE 10

#define RETICLE_OFFSET_X -7
#define RETICLE_OFFSET_Y 3
#define CLOSE_FOCUS 100
#define LIDAR_OFFSET 10

#define DISTANCE_MIN 15
#define DISTANCE_MAX 12


extern const int ISOS[];
extern const int ISOS_SIZE; 
extern const float CALIB_DISTANCES[];
extern const int CALIB_DISTANCES_SIZE;

#define K_VALUE 20 

#endif // MRFCONSTANTS_H_