
//status values
int manualBrightness;
int brightness;
int wallMode;
long  effectCounter;
bool  sunrise;
int  sunriseDuration;
int  sunriseBrightness;
int  sunriseMinuteOfDay;
String  tickerString;
String  sensorTag;
bool  sensorEnabled;
int  sensorDuration;
int  sensorBrightness;
int  LEDRefreshInterval;

time_t lastTime;
long sensorTime;


int currentBrightness = 0;

String statusString() {
  DynamicJsonDocument  json(500);
  json["manualBrightness"] = manualBrightness;
  json["brightness"] = FastLED.getBrightness();
  json["wallMode"] = wallMode;
  json["effectCounter"] = effectCounter;
  json["sunrise"] = sunrise;
  json["sunriseDuration"] = sunriseDuration;
  json["sunriseBrightness"] = sunriseBrightness;
  json["sunriseMinuteOfDay"] = sunriseMinuteOfDay;
  json["tickerString"] = tickerString;
  json["sensorEnabled"] = sensorEnabled;
  json["sensorDuration"] = sensorDuration;
  json["sensorBrightness"] = sensorBrightness;
  json["LEDRefreshInterval"] = LEDRefreshInterval;
  json["sensorTime"] = sensorTime;
  json["sensorTag"] = sensorTag;
  noInterrupts();
  #ifdef BME_280
    json["temperature"] = bme.readTemperature();
    json["humidity"] = bme.readHumidity();
    json["pressure"] = bme.readPressure();
  #endif
  interrupts();
  String output;
  serializeJsonPretty(json, output);
  return output;
}

void writeStatus() {
  File configFile = SPIFFS.open("/status.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return;
  }
  configFile.print(statusString());
  configFile.close();
}
void readStatus() {
  DynamicJsonDocument json(1024);
  if (SPIFFS.exists("/status.json")) {
    File statusFile = SPIFFS.open("/status.json", "r");
    if (!statusFile) {
      Serial.println("Failed to open config file");
    }
    else {
      deserializeJson(json, statusFile);
    }
  }

  manualBrightness = json["manualBrightness"];
  brightness = json["brightness"];
  wallMode = json["wallMode"];
  effectCounter = json["effectCounter"];
  sunrise = json["sunrise"];
  sunriseDuration = json["sunriseDuration"];
  sunriseBrightness = json["sunriseBrightness"];
  sunriseMinuteOfDay = json["sunriseMinuteOfDay"];
  tickerString = json["tickerString"].as<String>();
  sensorEnabled = json["sensorEnabled"];
  sensorDuration = json["sensorDuration"];
  sensorBrightness = json["sensorBrightness"];
  LEDRefreshInterval = json["LEDRefreshInterval"];
  if (LEDRefreshInterval < 100) LEDRefreshInterval = 100;
  sensorTime = json["sensorTime"];
  sensorTag = json["sensorTag"].as<String>();
}
