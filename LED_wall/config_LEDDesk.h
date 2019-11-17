#define SENSORPIN D5 //USE SOMETHING OTHER THAND D0, 2, 8 - stop ESP from booting
#define BOARDLED 16
#define sensor true

char boardname[] = "LEDDesk";

#define LED_PIN         D3
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812B


//LED
// Params for width and height
const uint8_t kMatrixWidth = 50;
const uint8_t kMatrixHeight = 1;

#define showClock false
#define drawColon false
#define secondsBar false

#define NUM_LEDS kMatrixWidth*kMatrixHeight
#define FASTLED_ALLOW_INTERRUPTS 0

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;

int volts = 5;
int milliamps = 500;

uint8_t autobrightness[24] = { 1, 1, 1, 1,
                               1, 1, 1, 1,
                               1, 1, 1, 1,
                               1, 1, 1, 1,
                               1, 1, 1, 1,
                               1, 1, 1, 1
                             };