#define SENSORPIN D5 //USE SOMETHING OTHER THAND D0, 2, 8 - stop ESP from booting
#define BOARDLED 16
#define sensor false

char boardname[] = "ledwall";

#define LED_PIN         D1
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B


//LED
// Params for width and height
const uint8_t kMatrixWidth = 20;
const uint8_t kMatrixHeight = 10;

#define showClock true
#define drawColon false
#define secondsBar false

#define NUM_LEDS kMatrixWidth*kMatrixHeight
#define FASTLED_ALLOW_INTERRUPTS 0

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;

int volts = 12;
int milliamps = 10000;


uint8_t autobrightness[24] = { 1, 1, 1, 1,
                               1, 1, 1, 1,
                               1, 1, 4, 4,
                               4, 4, 4, 4,
                               4, 4, 10, 10,
                               10, 8, 4, 2
                             };
