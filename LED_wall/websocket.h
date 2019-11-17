void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("WS: [%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("WS: [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;

    case WStype_TEXT:
      Serial.printf("WS: [%u] get Text: %s\n", num, payload);

      // # ==> Set interval
      if (payload[0] == '?') {
        // decode delay data
        int d = (int) strtol((const char *) &payload[1], NULL, 10);
        LEDRefreshInterval = d;
        Serial.printf("WS: Set Interval: [%u]\n", LEDRefreshInterval);
        webSocket.sendTXT(num, "OK");
      }

      // # ==> Set brightness
      else if (payload[0] == '%') {
        int b = (int) strtol((const char *) &payload[1], NULL, 10);
        manualBrightness = ((b >> 0) & 0xFF);
        Serial.printf("WS: Set brightness to: [%u]\n", manualBrightness);
        FastLED.setBrightness(manualBrightness);
        webSocket.sendTXT(num, "OK");
      }


      // * ==> Set main color and light all LEDs (Shortcut)
      else if (payload[0] == '*') {
        // decode rgb data
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

        uint8_t r = ((rgb >> 16) & 0xFF);
        uint8_t g = ((rgb >> 8) & 0xFF);
        uint8_t b = ((rgb >> 0) & 0xFF);


        fill_solid( leds, NUM_LEDS, CRGB(r, g, b));
        FastLED.show();
        wallMode = 100;
        manualBrightness=255;
        FastLED.setBrightness(255);
        FastLED.show();
        Serial.printf("WS: Set all leds to main color: [%u] [%u] [%u]\n", r, g, b);
        webSocket.sendTXT(num, "OK");
      }
      break;
  }
  delay(10);
}
