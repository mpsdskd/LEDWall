#define SENSORPIN A0 //USE SOMETHING OTHER THAND D0, 2, 8 - stop ESP from booting
#define BOARDLED D3
#define sensor false

char boardname[] = "SmallLEDWall";

#define LED_PIN         D1
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B


//LED
// Params for width and height
const uint8_t kMatrixWidth = 30;
const uint8_t kMatrixHeight = 8;

#define showClock true
#define drawColon true
#define secondsBar true

#define NUM_LEDS kMatrixWidth*kMatrixHeight
#define FASTLED_ALLOW_INTERRUPTS 0

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;

int volts = 5;
int milliamps = 4000;


uint8_t autobrightness[24] = { 1, 1, 1, 1,
                               1, 1, 1, 1,
                               1, 1, 4, 4,
                               4, 4, 4, 4,
                               4, 4, 10, 10,
                               10, 8, 3, 1
                             };

#define ENVIRONMENT_SENSOR
#ifdef ENVIRONMENT_SENSOR
  #define BME_280
#endif
