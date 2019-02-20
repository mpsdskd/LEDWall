# LEDWall
FastLED-Based LED Wall as Clock/Alarm Clock/Decoration/Mood Light

## HowTo
- Install necessary libraries
- Acquire ESP8266
- Upload Sketch data
- Settings Ardino File (top part of file)
- Flash program
- Connect FasLED-compatible (e.G. WS2812) LED strip to µC
  - See https://www.mschoeffler.de/2017/08/24/how-to-control-an-led-pixel-strip-ws2812b-with-an-arduino-and-the-fastled-library/ for example
  - I had problems with a resistor in the data line, as often suggested, so directly connect data ESP data pin to LED strip data pin
    - Propably caused by signal being only 3V -> ESP
- Power LED strip with 5V
- Power ESP8266 MCU via 5V from LED strip
- ...
- PROFIT

- ESP opens it's own WiFi-AP
  - Connect to it
  - Go to 192.168.4.1
  - Connect to your WiFi
- Now you can connect to the ESP via it's IP, mDNS (ledwall.local) or it's hostname (ledwall.fritz.box) via your home router (Propably best, as Android does not like mDNS and IP is via DHCP)
- Set brightness and speed via sliders on Main Page
- "Alarm Settings" to set up light alarm
- Set Color via "Solid Color"
- Effects with the other buttons

## Explanatory Images

Demo:

![](https://github.com/mpsdskd/LEDWall/blob/master/images/IMG_20190112_133726.jpg)

Alarm Clock:

![](https://github.com/mpsdskd/LEDWall/blob/master/images/IMG_20190112_134422.jpg)
![](https://github.com/mpsdskd/LEDWall/blob/master/images/IMG_20190112_134535.jpg)
![](https://github.com/mpsdskd/LEDWall/blob/master/images/IMG_20190112_134742.jpg)

Wiring:

![](https://github.com/mpsdskd/LEDWall/blob/master/images/Wiring.png)

Basic WebInterface:

![](https://github.com/mpsdskd/LEDWall/blob/master/images/Screenshot_2019-01-12%20LED%20Wall.png)

Alarm Clock Settings:

![](https://github.com/mpsdskd/LEDWall/blob/master/images/Screenshot_2019-01-12%20LED%20Wall(1).png)

Color Picker:

![](https://github.com/mpsdskd/LEDWall/blob/master/images/Screenshot_2019-01-12%20Getting%20colors.png)

## Needs:
- ESP8266 board library (perhaps works with ESP32 as well, don't know)
- Provided Font library in the Arduino library folder
- WiFiManager 
  - https://github.com/tzapu/WiFiManager
- ArduinoJson 5.X 
  - https://github.com/bblanchon/ArduinoJson)
- WebSocketsServer 
  - https://github.com/Links2004/arduinoWebSockets
- ArduinoOTA (iirc provided with ESP libraries)
- Timezone
  - https://github.com/JChristensen/Timezone
- THE GREAT FastLED-Library
  - http://fastled.io/
- Time
  - https://github.com/PaulStoffregen/Time
- Propably more, that I forgot

## Resources (THANK YOU!)
- Fonts: https://www.mikrocontroller.net/topic/54860
- Color Selctor: https://github.com/russp81/LEDLAMP_FASTLEDs
  - Using http://jscolor.com/
- Effects:
  - https://github.com/jasoncoon/SmartMatrix-FastLED-Examples/blob/master/Fire1/Fire1.ino
  - Some more, that I do not remember right now
- Webinterface: https://randomnerdtutorials.com/esp8266-web-server/
  
## ToDo
- Alarm Clock with Weekday settings
  - Possibly more than one alarm
- Via Browser settable AutoBrightness
- Update ArduinoJSON
- AutoBrightness via Ping (Brightness increased, if computer is on or smartphone connected to home network)
- Implementation of PIR-Sensor

- Get Firework to be bitten by WDT
- Text display

### Untested
- Vertical clock layout (Hour over Minute instead of Hour:minute)
- Matrix-Layouts other than serpent layout

## FAQ
- The matrix here is set to 10x20, but I want 30x8 as in the pictures?
  - Set the matrix dimensions to 30x8
  - Take the more fitting font
    - `#include <Font6x8_meins_nodemcu_v.h>`
  - If you want the blinking colon between hours and minutes, as there is enough space for that with a 30x8 matrix
    - `#define drawColon true`
