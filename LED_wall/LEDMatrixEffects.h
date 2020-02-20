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
  int COOLING = 320/kMatrixHeight;

  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  // Default 120, suggested range 50-200.
  int SPARKING = 50;

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

CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff){
// Add one layer of waves into the led array
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

void pacifica_add_whitecaps() {
// Add extra 'white' to areas where the four layers of light have lined up brightly
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}
void pacifica_deepen_colors() {
  // Deepen the blues and greens
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}
void pacifica_loop() {

// https://gist.github.com/kriegsman/36a1e277f5b4084258d9af1eae29bac4

  //  "Pacifica"
  //  Gentle, blue-green ocean waves.
  //  December 2019, Mark Kriegsman and Mary Corey March.
  //  For Dan.
  //



//////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and 
// while it is "ready to run", and documented in general, it is probably not 
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.  
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then 
// another filter is applied that adds "whitecaps" of brightness where the 
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent 
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//

  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
//  static uint32_t sLastms = 0;
//  uint32_t ms = GET_MILLIS();
//  uint32_t deltams = ms - sLastms;
//  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (10 * speedfactor1) / 256;
  uint32_t deltams2 = (10 * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}
