int vx = 16;
int vy = 8;
int px = 0;
int py = 4;
void parttrail(long counter) {
  // 20 ms frame time is best
  long i = counter;
  px += vx / 8;
  py += vy / 8;

  if (px > kMatrixWidth - 2 || px <= 0) vx = -vx;


  if (py > kMatrixHeight - 2 || py <= 0) vy = -vy;

  if (px < 0) px = 0;
  if (py < 0) py = 0;
  if (px > kMatrixWidth - 1) px = kMatrixWidth - 1;
  if (py > kMatrixHeight - 1) py = kMatrixHeight - 1;

  //leds[XYsafe(px + random8() % 5 - 2 - vx / 4, py + random8() % 5 - 2 - vy / 4)] = CHSV(i / 4, random8(64, 256), random8(32, 200));
  leds[XYsafe(px, py)] = CHSV(i / 4, 255, 255);
  fadeToBlackBy( leds, NUM_LEDS, 16);
  FastLED.show();
  if (i % 3 == 0) {
    vx = vx + random8() % 3 - 1;
    vy = vy + random8() % 3 - 1;
    while (vx / 8 == 0) vx = vx + random8() % 7 - 3;
    while (vy / 8 == 0) vy = vy + random8() % 7 - 3;
    if (vx > 24) vx = 24;
    if (vx < -24) vx = -24;
    if (vy > 16) vy = 16;
    if (vy < -16) vy = -16;
  }
}
void REGENBOGEN(long counter) { //20 ms is best
  int i = counter % 4096;
  if (i < 2048) {
    for (byte x = 0; x < kMatrixWidth; x++) {
      for (byte y = 0; y < kMatrixHeight; y++) {
        leds[XYsafe(x, y)] = CHSV(cos8(y * i / 50) + sin8(x * 5 + i / 10) + i, 255, 255);
      }
    }
  }
  else  {
    i = 4096 - i;
    for (byte x = 0; x < kMatrixWidth; x++) {
      for (byte y = 0; y < kMatrixHeight; y++) {
        leds[XYsafe(x, y)] = CHSV(cos8(y * i / 50) + sin8(x * 5 + i / 10) + i, 255, 255);
      }
    }
  }
}
void gradient(long counter) {
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XYsafe(x, y)] = CHSV((counter + x) % 255, 255, 255);
    }
  }
}
void speckles(long counter) {
  // random colored speckles that blink in and fade smoothly
  if (NUM_LEDS > random16(400)) leds[random16(NUM_LEDS)] += CHSV( (counter / 64 + random8(64)) % 255, random(32, 256), 255);
  fadeToBlackBy( leds, NUM_LEDS, 2);
}
byte heat[kMatrixWidth][kMatrixHeight];
void fire(long counter) {
  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 55, suggested range 20-100
  uint8_t COOLING = 320/kMatrixHeight;

  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  // Default 120, suggested range 50-200.
  uint8_t SPARKING = 50;

  for (int x = 0; x < kMatrixWidth; x++) {
    // Step 1.  Cool down every cell a little
    for (int i = 0; i < kMatrixHeight; i++) {
      heat[x][i] = qsub8(heat[x][i], random(0, ((COOLING * 10) / kMatrixHeight) + 2));
    }
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = kMatrixHeight; k > 1; k--) {
      heat[x][k] = (heat[x][k - 1] + heat[x][k - 2] + heat[x][k - 2]) / 4;
    }
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random(255) < SPARKING) {
      int y = random(kMatrixHeight/3);
      heat[x][y] = qadd8(heat[x][y], random(160, 255));
    }
    // Step 4.  Map from heat cells to LED colors
    for (int y = 0; y < kMatrixHeight; y++) {
      CRGB color;
      //      switch (direction) {
      //        case Right:
      //          color = HeatColor(heat[y][x]);
      //          break;
      //        case Down:
      //          color = HeatColor(heat[x][y]);
      //          break;
      //        case Left:
      //          color = HeatColor(heat[y][(kMatrixWidth - 1) - x]);
      //          break;
      //        case Up:
      color = HeatColor(heat[x][kMatrixHeight-y]);
      //          break;
      //      }
      leds[XY(x, y)] = color;
    }
  }
}
uint8_t kBorderWidth = 1;
//void swirl(long counter) {
//  // Apply some blurring to whatever's already on the matrix
//  // Note that we never actually clear the matrix, we just constantly
//  // blur it repeatedly.  Since the blurring is 'lossy', there's
//  // an automatic trend toward black -- by design.
//  uint8_t blurAmount = beatsin8(2, 10, 255);
//  blur2d( leds, kMatrixWidth, kMatrixHeight, blurAmount);
//
//  // Use two out-of-sync sine waves
//  uint8_t  i = beatsin8( 27, kBorderWidth, kMatrixHeight - kBorderWidth);
//  uint8_t  j = beatsin8( 41, kBorderWidth, kMatrixWidth - kBorderWidth);
//  // Also calculate some reflections
//  uint8_t ni = (kMatrixWidth - 1) - i;
//  uint8_t nj = (kMatrixWidth - 1) - j;
//
//  // The color of each point shifts over time, each at a different speed.
//  leds[XY( i, j)] += CHSV( counter / 11, 200, 255);
//  leds[XY( j, i)] += CHSV( counter / 13, 200, 255);
//  leds[XY(ni, nj)] += CHSV( counter / 17, 200, 255);
//  leds[XY(nj, ni)] += CHSV( counter / 29, 200, 255);
//  leds[XY( i, nj)] += CHSV( counter / 37, 200, 255);
//  leds[XY(ni, j)] += CHSV( counter / 41, 200, 255);
//}
void gradient2d(long counter) {
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XYsafe(x, y)] = CHSV((counter + 2*x + 4*y) % 255, 255, 255);
    }
  }
}
