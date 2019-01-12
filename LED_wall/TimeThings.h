//Time Things
#include <Time.h>
#include <Timezone.h>
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

IPAddress localTimeServerIPs[3] =
{
  IPAddress( 192, 168, 178, 1),
  IPAddress( 192, 168, 2, 1 ),
  IPAddress( 192, 168, 1, 1 )
};
//IPAddress *localtimeServerIP = IPAddress(192, 168, 178, 1);
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {
  "CEST", Last, Sun, Mar, 2, 120
};     //Central European Summer Time
TimeChangeRule CET = {
  "CET ", Last, Sun, Oct, 3, 60
};       //Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t utc, local;
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
int connection = 100;

#include <Sunrise.h>
// create a Sunrise object
Sunrise mySunrise(51, 7, 0);
bool night = false;


unsigned long sendNTPpacket(IPAddress & address) { // send an NTP request to the time server at the given address
  Serial.print("Sending NTP packet to ");
  Serial.println(address);
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void printTime(time_t t) {
  Serial.print(hour(t));
  Serial.print(':');
  Serial.print(minute(t) / 10);
  Serial.print(minute(t) % 10);
  Serial.print(':');
  Serial.print(second(t) / 10);
  Serial.print(second(t) % 10);
  Serial.print("  ");
  Serial.print(dayShortStr(weekday(t)));
  Serial.print(", ");
  Serial.print(year(t));
  Serial.print('-');
  Serial.print(month(t) / 10);
  Serial.print(month(t) % 10);
  Serial.print('-');
  Serial.print(day(t) / 10);
  Serial.print(day(t) % 10);
  Serial.println(' ');
}

boolean gettime() {
  digitalWrite(LED_BUILTIN, LOW);
  //once wait for wifi to be connected
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wait for WiFi");
    delay(1000);
    connection -= 1;
    Serial.println(connection);
    if (connection <= 0)
    {
      ESP.restart();
    }
    return false;
  }
  else  {
    udp.begin(localPort);
    Serial.println("WiFi connected");
    //Try NTP on local router
    int cb = udp.parsePacket();
    for (int i = 0; i < ARRAY_SIZE(localTimeServerIPs); i++) {
      if (!cb) {
        Serial.println(localTimeServerIPs[i]);
        sendNTPpacket(localTimeServerIPs[i]);
        delay(200);
      }
    }
    //if nothing received -> try external time server
    if (cb) {
      Serial.println("Local NTP successful");
    }
    else
    { Serial.println("Local NTP not successful");
      WiFi.hostByName(ntpServerName, timeServerIP);
      sendNTPpacket(timeServerIP);
      delay(500);
      cb = udp.parsePacket();
      if (cb)
      {
        Serial.println("Remote NTP successful after short time");
      }
      else
      {
        delay(2000);
        cb = udp.parsePacket();
        if (cb)
        {
          Serial.println("Remote NTP successful after longer time");
        }
      }
      if (!cb)
      {
        Serial.println("Remote NTP not successful");

        return false;
      }
    }
    if (cb >= NTP_PACKET_SIZE) {
      udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      setTime(secsSince1900 - 2208988800UL);
      //setTime(23, 55, 0, 30, 3, 2016); //simulate time for test
      Serial.println("Set time");
      connection = 20;
      local = CE.toLocal(now(), &tcr);
      Serial.println("Local time:");
      printTime(local);

      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(2, HIGH);

      Serial.println("This is in UTC so compare to time:");
      Serial.print(hour());
      Serial.print(":");
      Serial.println(minute());

      mySunrise.Actual();
      time_t t = mySunrise.Rise(month(), day()); // (month,day) - january=1
      Serial.print("Sunrise: ");
      Serial.print(mySunrise.Hour());
      Serial.print(":");
      Serial.println(mySunrise.Minute());
      if (hour() * 60 + minute() < mySunrise.Hour() * 60 + mySunrise.Minute()) {
        night = true;
      }
      t = mySunrise.Set(month(), day());
      if (hour() * 60 + minute() > mySunrise.Hour() * 60 + mySunrise.Minute()) {
        night = true;
      }
      else {
        night = false;
      }
      Serial.print("Sunset: ");
      Serial.print(mySunrise.Hour());
      Serial.print(":");
      Serial.println(mySunrise.Minute());
      lastTime = now();
      Serial.print("It's night: "); Serial.println(night);
      return true;
    }
  }
}

time_t compileTime(void) { //Function to return the compile date and time as a time_t value
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)
  const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char chMon[3], *m;
  int d, y;
  tmElements_t tm;
  time_t t;
  strncpy(chMon, compDate, 3);
  chMon[3] = '\0';
  m = strstr(months, chMon);
  tm.Month = ((m - months) / 3 + 1);
  tm.Day = atoi(compDate + 4);
  tm.Year = atoi(compDate + 7) - 1970;
  tm.Hour = atoi(compTime);
  tm.Minute = atoi(compTime + 3);
  tm.Second = atoi(compTime + 6);
  t = makeTime(tm);
  return t + FUDGE;        //add fudge factor to allow for compile time
}

//void ntpRefresh (void *pArg) {
//  os_timer_disarm(&refreshTimer);
//  if (!gettime()) {
//    os_timer_arm(&ntpTimer, 20000, false);
//    Serial.println("DID NOT GET NEW TIME");
//  }
//  else {
//    os_timer_arm(&ntpTimer, 300000, false);
//    Serial.println("got new time");
//  }
//  os_timer_arm(&refreshTimer, LEDRefreshInterval, false);
//}
