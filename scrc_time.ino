#include "scrc_time.h"

#include <WiFiUdp.h>
#include <NTPClient.h>
#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) )

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


static uint64_t ntp_epoch_time = 0;
static uint64_t last_update = 0;
static long time_off_set = 0;

// timeoffset for IST is 19800
void sync_time(int timeOffset) {
  time_off_set = timeOffset;

  timeClient.begin();
  timeClient.setTimeOffset(time_off_set);

  //timeClient.update();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
    // TODO: implement timeout
  }

  Serial.print("ntp time: " + timeClient.getFormattedTime());

  ntp_epoch_time = timeClient.getEpochTime();
  last_update = millis();

}

uint64_t get_timestamp() {
  return ntp_epoch_time +
         ((millis() - last_update) / 1000);
}

String get_timestamp_str(uint64_t timestamp) {

  // adapted from https://github.com/arduino-libraries/NTPClient/blob/master/NTPClient.cpp

  unsigned long hours = (timestamp % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (timestamp % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = timestamp % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return hoursStr + ":" + minuteStr + ":" + secondStr;
}



String get_datestamp_str(uint64_t timestamp) {

  // adapted from https://github.com/taranais/NTPClient/blob/master/NTPClient.cpp

  unsigned long rawtime = (timestamp/86400L) ;

    unsigned long days = 0, year = 1970;
  uint8_t month;

  static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawtime)
    year++;
  rawtime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
  days=0;
  for (month=0; month<12; month++) {
    uint8_t monthLength;
    if (month==1) { // february
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    } else {
      monthLength = monthDays[month];
    }
    if (rawtime < monthLength) break;
    rawtime -= monthLength;
  }
  String monthStr = ++month < 10 ? "0" + String(month) : String(month); // jan is month 1  
  String dayStr = ++rawtime < 10 ? "0" + String(rawtime) : String(rawtime); // day of month  
  return String(year) + "-" + monthStr + "-" + dayStr ;

  
}
