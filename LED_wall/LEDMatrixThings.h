void setBright(int inputhour) {
  FastLED.setBrightness(autobrightness[inputhour]);
}
void fadeBright(uint8_t brightness) {
  if (FastLED.getBrightness()>brightness) {FastLED.setBrightness(FastLED.getBrightness()-1);}
  if (FastLED.getBrightness()<brightness) {FastLED.setBrightness(FastLED.getBrightness()+1);}
//  Serial.print("Current Brightness = "); Serial.println(currentBrightness);
}
void fadeAutoBright(int inputhour) {
  fadeBright(autobrightness[inputhour]);
//  Serial.print("Autobrightness = "); Serial.println(autobrightness[inputhour]);
}
uint16_t XY( uint8_t x, uint8_t y) {
  uint16_t i;
  if ( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }
  if ( kMatrixSerpentineLayout == true) {
    if ( y & 0x01) {                              // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    }
    else {                                        // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}
uint16_t XYsafe( int x, int y) {
  if ( x >= kMatrixWidth) return NUM_LEDS;
  if ( y >= kMatrixHeight) return NUM_LEDS;

  if ( x < 0 ) return NUM_LEDS;
  if ( y < 0 ) return NUM_LEDS;

  return XY((uint8_t) x, (uint8_t) y);
}
void drawLetter(int posx, int posy, char letter, CRGB color) {
  //    Serial.print("Buchstabe:");
  //    Serial.println(letter);
  if ( (posx > -FontWidth) && (posx < kMatrixWidth) )
  {
    for (int x = 0; x < FontWidth; x++)
    {
      for (int y = 0; y < FontHeight; y++)
      {
        if (bitRead(pgm_read_byte(&(Font[letter][x])), y) == 1)
        {
          leds[XYsafe(posx + x, posy + y)] = color;
        }
      }
    }
  }
}
void drawTime(time_t t, int x, int y, CRGB color, bool colon, bool seconds) {
  x -= 0;
  if (hour(t) / 10 > 0) {
    drawLetter(x, y, hour(t) / 10 + 48, color);
  }
  x += FontWidth + 1;
  drawLetter(x, y, hour(t) % 10 + 48, color);
  x += FontWidth;
  if (colon) {
    if (second(t) % 6 == 0) {
      drawLetter(x - 1, y, ':', color);
    }
    x += 4;
  }
  drawLetter(x, y, minute(t) / 10 + 48, color);
  x += FontWidth + 1;
  drawLetter(x, y, minute(t) % 10 + 48, color);
  //  //Bottom bar for seconds
  //  for (int posx = 0; posx < kMatrixWidth; posx++) {
  //    if (posx <= (second(t)*kMatrixWidth / 60))
  //      leds[XYsafe(posx, kMatrixHeight - 1)] = color;
  //  }

  // Bottom Point (with trail for fading) for second, may show problems (-> non-illuminating points) for matrices with width> 60px
  if (seconds) leds[XYsafe(second(t)*kMatrixWidth / 60, kMatrixHeight - 1)] = color;
}
void drawDate(time_t t, int y, CRGB fg, CRGB bg, int del) {
  for (int x = kMatrixWidth; x > -70; x--)
  {
    setBright(t);
    fill_solid(leds, NUM_LEDS, bg);
    int pos = x;
    drawLetter(pos, 0 + y, year(t) / 1000 + 48, fg);
    pos += 7;
    drawLetter(pos, 0 + y, (year(t) % 1000) / 100 + 48, fg);
    pos += 7;
    drawLetter(pos, 0 + y, (year(t) % 100) / 10 + 48, fg);
    pos += 7;
    drawLetter(pos, 0 + y, year(t) % 10 + 48, fg);
    pos += 6;
    drawLetter(pos, 0 + y, '-', fg);
    pos += 7;
    drawLetter(pos, 0 + y, month(t) / 10 + 48, fg);
    pos += 7;
    drawLetter(pos, 0 + y, month(t) % 10 + 48, fg);
    pos += 6;
    drawLetter(pos, 0 + y, '-', fg);
    pos += 7;
    drawLetter(pos, 0 + y, day(t) / 10 + 48, fg);
    pos += 7;
    drawLetter(pos, 0 + y, day(t) % 10 + 48, fg);
    FastLED.show();
    FastLED.delay(del);
    FastLED.clear();
  }
}
void runString(String str, CRGB fg, int del) {
  Serial.println(str);
  int l = str.length();
  Serial.println(l);
  for (int i = kMatrixWidth; i > -(FontWidth + 1)*l; i--) {
    for (int letter = 0; letter < l; letter++) {
      drawLetter(i + (FontWidth + 1)*letter, 0, str[letter], fg);
    }
    FastLED.delay(del);
    FastLED.clear();
  }
}
void drawstring(String str, CRGB fg) {
  for (int letter = 0; letter < str.length(); letter++)  {
    drawLetter((FontWidth + 1)*letter, 0, str[letter], fg);
  }
  FastLED.show();
  FastLED.clear();
}
void ticker(String str, CRGB fg, int counter) {
  int pos = 0;
  if (str.length() * (FontWidth + 1) >= kMatrixWidth)
    pos = counter % ((str.length() * (FontWidth + 1)) + 3*kMatrixWidth/2) - kMatrixWidth;
  for (int letter = 0; letter < str.length(); letter++)  {
    drawLetter((FontWidth + 1)*letter - pos, 0, str[letter], fg);
  }

}
//DEFINE_GRADIENT_PALETTE( sunriseP ) {
//  0,     1,  0,  0,   //black
//  1,     1,  0,  0,   //black
//  70,   50,  0,  0,   //red
//  140,   255, 150,  0,  //bright yellow
//  250,   255, 255, 255
//}; //full white
//CRGBPalette256 sunrisePalette = sunriseP;
