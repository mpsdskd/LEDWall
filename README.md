# LEDWall
FastLED-Based LED Wall as Clock/Alarm Clock/Decoration/Mood Light

## HowTo
- Install necessary libraries
- Acquire ESP8266
- Upload Sketch data
- Flash program
- Connect FasLED-compatible (e.G. WS2812) LED strip to µC
- Power LED strip with 5V
- Power ESP8266 MCU via 5V from LED strip
- ...
- PROFIT

## Explanatory Images

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
  
## ToDo
- Alarm Clock with Weekday settings
  - Possibly more than one alarm
- Via Browser settable AutoBrightness
- Update ArduinoJSON
- AutoBrightness via Ping (Brightness increased, if computer is on or smartphone connected to home network)
- Implementation of PIR-Sensor
- 

### Untested
- Vertical clock layout (Hour over Minute instead of Hour:minute)
- Matrix-Layouts other than serpent layout
