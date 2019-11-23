// vim: expandtab tabstop=2 shiftwidth=2

#include <time.h>      // to convert to epoch time
#include <RTCZero.h>   // for built-in real-time-clock

#include "gps.h"

// from https://github.com/cavemoa/Feather-M0-Adalogger/blob/master/SimpleSleep/SimpleSleep.ino
RTCZero rtc;

void setup_rtc() {
  rtc.begin();
  rtc.setEpoch(0);  // January 1, 1970
}

time_t get_epoch(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  struct tm t = {0};  // zero out the struct
  t.tm_year = year;  // only 8-bits, so actual year minus 1900
  t.tm_mon = month;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  return mkgmtime(&t);
}

void update_rtc(Status *status) {
  // set RTC to GPS time
  if (status->GPSparsed) {
    time_t epoch = get_epoch(GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds);
    rtc.setEpoch(epoch + GPS.secondsSinceTime());
  }
}
