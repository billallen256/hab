// vim: expandtab tabstop=2 shiftwidth=2

#ifndef RTC_H
#define RTC_H

#include <time.h>      // to convert to epoch time
#include <RTCZero.h>   // for built-in real-time-clock

// from https://github.com/cavemoa/Feather-M0-Adalogger/blob/master/SimpleSleep/SimpleSleep.ino
RTCZero rtc;

void setup_rtc() {
  rtc.begin();
  rtc.setEpoch(0);  // January 1, 1970
}

void update_rtc_data(Status *status) {
  status->elapsedTime = rtc.getEpoch();
}

#endif
