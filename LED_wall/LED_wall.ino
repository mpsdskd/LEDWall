extern "C" {
#include "user_interface.h"
}
#include <FS.h>
#include <Font6x8_meins_nodemcu_v.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

os_timer_t refreshTimer;
//os_timer_t ntpTimer;
//#include "config_leddesk.h" //NodeMCU
//#include "config_kitchen.h" //Wemos D1 Mini
//#include "config_living_room.h" //Wemos D1 Mini
#include "config_ledwall_30x8.h" //NodeMCU



#include <FastLED.h>
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

File fsUploadFile;

#include "status.h"
#include "TimeThings.h"
#include "LEDMatrixThings.h" // bright, drawLetter, darwTime, drawDate 
#include "LEDMatrixEffects.h"
#include "FireworkEffect.h"

ESP8266WebServer webserver(80);   //Web server object. Will be listening in port 80 (default for HTTP)
WebSocketsServer webSocket(81);
#include "HTMLPages.h"
#include "websocket.h"

void refreshLEDs (void *pArg) {
  digitalWrite(BOARDLED, LOW);
  //  Serial.println("Start update");
  //  Serial.println(currentBrightness);
  effectCounter = effectCounter + 1;
  if (manualBrightness >= 0 && manualBrightness <= 255) {
    FastLED.setBrightness((uint8_t)manualBrightness);
  }
  else {
    if (sensorEnabled && ((now() - sensorTime) < sensorDuration) && (manualBrightness < 0 || manualBrightness > 255)) {
      fadeBright(sensorBrightness);
    }
    else {
      fadeBright(0);
    }
  }
  switch (wallMode) {
    case 1:
      parttrail(effectCounter);
      break;
    case 2:
      REGENBOGEN(effectCounter);
      break;
    case 3:
      gradient(effectCounter);
      break;
    case 4:
      speckles(effectCounter);
      break;
    case 5:
      fire(effectCounter);
      break;
    //    case 6:
    //      swirl(effectCounter);
    //      delay(50);
    //      break;
    case 7:
      gradient2d(effectCounter);
      break;
    case 8:
      firework(effectCounter);
      break;
    case 9:
      FastLED.clear();
      ticker(tickerString, CHSV(effectCounter / 5, 255, 255), effectCounter);
      break;
    case 10:
      pacifica_loop();
      break;
    case 100:
      break;
    case 0: //(Sunrise-) Clock
    default:
      local = CE.toLocal(now(), &tcr);
      if ((bool)sunrise && (hour(local) * 60 + minute(local) < (int)sunriseMinuteOfDay) && (hour(local) * 60 + minute(local) > (int)sunriseMinuteOfDay - (int)sunriseDuration)) { //DOES NOT WORK AROUND 0:00 -> I DON'T CARE
//        digitalWrite(BOARDLED, LOW);
        Serial.println("Sunrise");
        FastLED.setBrightness(sunriseBrightness);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = HeatColor(map(hour(local) * 3600 + minute(local) * 60 + second(local) - ((int)sunriseMinuteOfDay - (int)sunriseDuration) * 60, 0, 60 * (int)sunriseDuration, 0, 255));
        }
        long a = sq(hour(local) * 3600 + minute(local) * 60 + second(local) - ((int)sunriseMinuteOfDay - (int)sunriseDuration) * 60);
        if (showClock) drawTime(local, 0, 0, CHSV(150, 150, map(a, 0, 3600 * sq((int)sunriseDuration), 10, 255)), drawColon, secondsBar);
      }
      if (manualBrightness < 0 || manualBrightness > 255) {
        fadeAutoBright(hour(local));
      }
      fadeToBlackBy( leds, NUM_LEDS, 16);
      CRGB color = CRGB(255, 0, 0);
      if (FastLED.getBrightness() > 1) {
        CRGB color = CHSV((60 * minute(now()) + second(now())) * 256 / 3600, 255, 255);
      }
      if (showClock) {
        drawTime(local, 0, 0, color, drawColon, secondsBar);
      }
      fadeAutoBright(hour(local));
  }
  digitalWrite(BOARDLED, HIGH);
  FastLED.show();
  os_timer_arm(&refreshTimer, (int)LEDRefreshInterval, false);
  //Serial.println("update finished");
}

ICACHE_RAM_ATTR void interruptRoutine() {
  Serial.print("Sensor ");
  Serial.println(digitalRead(SENSORPIN));
  sensorTime = (long)now();
}
void setup() {
  Serial.begin(1000000);
  Serial.println("Start!");
  readStatus();
  if (sensor == true) {
    attachInterrupt(digitalPinToInterrupt(SENSORPIN), interruptRoutine, CHANGE);
  }

  delay(500);
  if (true) { //LEDs  //LEDs  //LEDs  //LEDs
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(volts, milliamps);
    // FastLED.setDither(0);
    Serial.println("LEDs set up");
    fill_solid( leds, NUM_LEDS, CRGB(20, 0, 0));
    FastLED.show();
  }
  FastLED.clear();
  drawstring("WiFi", CRGB(50, 50, 50));
  //  mySunrise.Actual();
  if (true) { //<- if clause for code folding in arduino //WiFi Manager  //WiFi Manager  //WiFi Manager  //WiFi Manager
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    WiFi.hostname(boardname);
    wifiManager.setConnectTimeout(10);
    wifiManager.setTimeout(180);

    //reset settings - for testing
    //wifiManager.resetSettings();

    //tries to connect to last known settings
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP" with password "password"
    //and goes into a blocking loop awaiting configuration
    if (!wifiManager.autoConnect()) {
      Serial.println("failed to connect, resetting to retry");
      ESP.restart();
    }

    //if you get here you have connected to the WiFi
    Serial.println("Connected :)");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());


    drawstring("mDNS", CRGB(50, 50, 50));
    if (!MDNS.begin(boardname)) {             // Start the mDNS responder for esp8266.local
      Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");
  }
  drawstring("Pins", CRGB(50, 50, 50));
  if (true) { //PINS  //PINS  //PINS  //PINS
    pinMode(SENSORPIN, INPUT);
    pinMode(BOARDLED, OUTPUT);
    digitalWrite(BOARDLED, LOW);
//    pinMode(BOARDLED, OUTPUT);
//    digitalWrite(BOARDLED, LOW);
    Serial.println("Pins set up");
  }

  ESP.wdtDisable();
  ESP.wdtEnable(10);
  drawstring("Time", CRGB(50, 50, 50));
  while (!gettime()) {}

  SPIFFS.begin();
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");

  drawstring("OTA", CRGB(50, 50, 50));
#include "ArduinoOTASetup.h";

  drawstring("Web", CRGB(50, 50, 50));

  readStatus();

  Serial.println("LightClock");
  Serial.println(sunrise);
  Serial.println("Duration");
  Serial.println(sunriseDuration);
  Serial.println("Brightness");
  Serial.println(sunriseBrightness);
  Serial.println("Minute of the Day");
  Serial.println(sunriseMinuteOfDay);
  if (true) { //WEBSERVER//WEBSERVER//WEBSERVER//WEBSERVER//WEBSERVER//WEBSERVER//WEBSERVER//WEBSERVER//WEBSERVER
    webserver.begin();

    webserver.on("/settings", HTTP_POST, []() {
      Serial.println("I got:");
      Serial.print("Time: ");
      Serial.println(webserver.arg("time"));
      Serial.print("Duration: ");
      Serial.println(webserver.arg("duration"));
      Serial.print("Brightness: ");
      Serial.println(webserver.arg("brightness"));
      Serial.print("Enabled: ");
      Serial.println(webserver.hasArg("enablesunrise"));
      Serial.print("Sensor Brightness: ");
      Serial.println(webserver.arg("sensorbrightness"));
      //
      //
      if (webserver.hasArg("enablesunrise")) sunrise = true;
      else sunrise = false;
      if (webserver.hasArg("enablesensor")) sensorEnabled = true;
      else sensorEnabled = false;
      sunriseDuration = webserver.arg("sunriseduration").toInt();
      sunriseBrightness = webserver.arg("sunrisebrightness").toInt();
      sensorBrightness = webserver.arg("sensorbrightness").toInt();
      sensorDuration = webserver.arg("sensorduration").toInt();
      Serial.println(webserver.arg("time").substring(0, 2));
      Serial.println(webserver.arg("time").substring(3, 5));
      sunriseMinuteOfDay = webserver.arg("time").substring(0, 2).toInt() * 60 + webserver.arg("time").substring(3, 5).toInt();

      writeStatus();
      String output = statusString();
      Serial.println(output);

      sendRoot();
    });

    webserver.on("/upload", HTTP_GET, []() {                 // if the client requests the upload page
      webserver.send(200, "text/html  ", uploadForm);
    });
    webserver.on("/upload",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
      webserver.send(200, "text/plain", "");
    }, handleFileUpload);                       // go to 'handleFileUpload'

    webserver.onNotFound(handleNotFound);

    webserver.on("/", HTTP_GET, []() {
      sendRoot();
    });
    //    webserver.on("/root.js", HTTP_GET, []() {
    //      webserver.send(200, "text/javascript", rootjs);
    //    });
    webserver.on("", HTTP_GET, []() {
      sendRoot();
    });
    webserver.on("/settings", HTTP_GET, []() {
      sendSunrisePage();
      Serial.println("Sending Sunrise Page (GET)");
    });
    webserver.on("/solid", HTTP_GET, []() {
      Serial.println("Send Color Selector");
      File page = SPIFFS.open("/color-selector.html", "r");
      webserver.streamFile(page, "text/html");
    });
    webserver.on("/0", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 50;
      wallMode = 0;
      manualBrightness = -1;
      writeStatus();
      Serial.println("(Sunrise) Clock");
    });
    webserver.on("/1", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 20;
      wallMode = 1;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Particle Trail");
    });
    webserver.on("/2", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 20;
      wallMode = 2;
      manualBrightness = 255;
      writeStatus();
      Serial.println("MathRainbow");
    });
    webserver.on("/3", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 500;
      wallMode = 3;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Gradient");
    });
    webserver.on("/4", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 50;
      wallMode = 4;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Speckles");
    });
    webserver.on("/5", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 80;
      wallMode = 5;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Fire");
    });
    //    webserver.on("/6", HTTP_GET, []() {
    //      sendRoot();
    //      LEDRefreshInterval=50;
    //      wallMode = 6;
    //      manualBrightness = 255;
    //      Serial.println("Swirl");
    //    });
    webserver.on("/7", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 50;
      wallMode = 7;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Gradient 2D");
    });
    webserver.on("/8", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 50;
      wallMode = 8;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Firework");
    });
    webserver.on("/9", HTTP_GET, []() {
      File page = SPIFFS.open("/textinput.html", "r");
      webserver.streamFile(page, "text/html");
      page.close();
      Serial.println("Ticker Settings");
    });
    webserver.on("/9", HTTP_POST, []() {
      LEDRefreshInterval = 150;
      wallMode = 9;
      manualBrightness = 255;
      writeStatus();
      tickerString = webserver.arg("ticker");
      sendRoot();
      //webserver.send(200, "text/html", "Displaying \"" + tickerString + "\"</h1><p></p><a href=\"/\">Home</a>");
    });
    webserver.on("/10", HTTP_GET, []() {
      sendRoot();
      LEDRefreshInterval = 30;
      wallMode = 10;
      manualBrightness = 255;
      writeStatus();
      Serial.println("Firework");
    });
    webserver.on("/cc", HTTP_GET, []() {
      FastLED.clear();
      if (webserver.hasArg("pixels")) {
        String val = webserver.arg("pixels");
        int i = 0;
        for (int y = 0; y < kMatrixHeight; y++) {
          for (int x = 0; x < kMatrixWidth; x++) {
            leds[XYsafe(x, y)] = strtol(val.substring(i * 6, i * 6 + 6).c_str(), NULL, 16);
            i++;
          }
        }
        wallMode = 100;
        FastLED.show();
      }
      webserver.send(200, "text/html", matrixInput());
    });
    webserver.on("/status", HTTP_GET, []() {
      String output = statusString();
      webserver.send(200, "application/json", output);
    });
    webserver.on("/restart", HTTP_GET, []() {
      webserver.send(200, "text/html", "RESTART");
      ESP.restart();
    });
  }
  //Software interrupt - in ms and not necessarily accurate timing
  os_timer_setfn(&refreshTimer, refreshLEDs, NULL);
  os_timer_arm(&refreshTimer, 5000, false);

  if (LEDRefreshInterval < (int)50) LEDRefreshInterval = (int)50;
  drawstring(" :)", CRGB(50, 50, 50));
  Serial.println("Finished Setup");
}
void loop() {
  webserver.handleClient();
  webSocket.loop();
  ArduinoOTA.handle();
  delay(20);
  ESP.wdtFeed();
  if (now() - lastTime > 300) {
    Serial.println("Getting new time");
    Serial.println(gettime()); //gettime does not seem to work with a timer, as it takes too long and the watchdog bites
    delay(100);
    Serial.println("getFreeHeap:");
    Serial.println(ESP.getFreeHeap());
    writeStatus();
  }
}
