//120 LEDs, 60 out, 60 in, clockwise outside from the bottom, anticlockwise inside from the bottom
int led_pos(bool out, int pos)
{
  if (pos > 59)
  {
    Serial.println("Non existent LED");
    return 0; ///FEHLER////
  }
  int num;
  if (out == true)
  {
    num = (pos + 30) % 60;
  }
  else
  {
    if (pos < 30)
    {
      num = 89 - pos;
    }
    else
    {
      num = 149 - pos;
    }
  }
  return num;
}

//Clockface/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void clockface()
{
  //five minute markers
  if (lotsoflight == true) {
    for (int i = 0; i < 12; i++) {
      //leds[led_pos(false, 5 * i)] = CHSV(0, 0, 64);
      leds[led_pos(true, 5 * i)] = CHSV(0, 0, 64);
    }
  }
  //3 hour markers
  for (int i = 0; i < 4; i++) {
    leds[led_pos(true, 15 * i)] = CHSV(0, 0, 150);

    if (lotsoflight == true) {
      leds[led_pos(false, 15 * i)] = CHSV(0, 0, 64);
    }
  }
  if (lotsoflight == true) { //12 o'clock marker
    leds[led_pos(true, 59)] = CHSV(0, 0, 20);
    leds[led_pos(true, 1)] = CHSV(0, 0, 20);

    leds[led_pos(false, 59)] = CHSV(0, 0, 20);
    leds[led_pos(false, 1)] = CHSV(0, 0, 20);
  }
}

void DrawClock(time_t local) {
  byte hue = 255 * (minute(now()) * 60 + second(now())) / 3600;
  //Hour bold
  float minutepos = minute(local) * 5 + second(local) / 12;
  int hourpos = hour(local) % 12 * 5 + minutepos / 60;
  //Serial.print("Hourpos=");
  //Serial.println(hourpos);

  //As Dark as possible///////////////////////////
  if (brightness < 4) {
    FastLED.clear();
    leds[led_pos(true, minute(local))] += CHSV(hue, 255, 255);
    leds[led_pos(true, second(local))] += CHSV((hue + 85) % 256, 255, 255);
    leds[led_pos(false, hourpos)] += CHSV((hue - 85) % 256, 255, 255);
    leds[led_pos(false, hourpos + 1)] += CHSV((hue - 85) % 256, 255, 255);
  }
  else {
    clockface();
    //  Serial.print("Hue=");
    //  Serial.println(hue);
    //  Serial.println(second(now()));

    //Minute +-1
    if (lotsoflight == true) {
      leds[led_pos(false, ((minute(local) + 1) % 60))] += CHSV(hue, 255, 64);
      leds[led_pos(false, ((minute(local) - 1) % 60))] += CHSV(hue, 255, 64);
      leds[led_pos(false, minute(local))] += CHSV(hue, 255, 255);
    }
    leds[led_pos(true, minute(local))] += CHSV(hue, 255, 255);
    if (brightness < 20) {
      leds[led_pos(true, ((minute(local) + 1) % 60))] += CHSV(hue, 255, 255);
      leds[led_pos(true, ((minute(local) - 1) % 60))] += CHSV(hue, 255, 255);
    }
    else  {
      leds[led_pos(true, ((minute(local) + 1) % 60))] += CHSV(hue, 255, 64);
      leds[led_pos(true, ((minute(local) - 1) % 60))] += CHSV(hue, 255, 64);
    }

    //Second with trail
    for (byte i = 1; i < 8; i++) {
      if (lotsoflight == true) {
        leds[led_pos(false, ((second(local)) + 1 - i) % 60)] += CHSV((hue + 85) % 256, 255, 255 / pow(2, i));
      }
      leds[led_pos(true, ((second(local)) + 1 - i) % 60)] += CHSV((hue + 85) % 256, 255, 255 / pow(2, i - 1));
    }

    leds[led_pos(false, hourpos)] += CHSV((hue - 85) % 256, 255, 255);
    leds[led_pos(false, (hourpos + 1) % 60)] += CHSV((hue - 85) % 256, 255, 255);

    byte fraction = round(hourpos * 1000) % 1000 * 256 / 1000;
    //  Serial.print("Fraction=");
    //  Serial.println(fraction);
    leds[led_pos(false, hourpos + 2)] += CHSV((hue - 85) % 256, 255, fraction);
    leds[led_pos(false, hourpos - 1)] += CHSV((hue - 85) % 256, 255, 255 - fraction);
  }
}
