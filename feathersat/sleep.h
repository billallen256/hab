// vim: expandtab tabstop=2 shiftwidth=2

#ifndef SLEEP_H
#define SLEEP_H

#include "rtc.h"

void alarmMatch() {
    Serial.println("Waking up...");
}

// from https://github.com/cavemoa/Feather-M0-Adalogger/blob/master/SimpleSleep/SimpleSleep.ino
void deep_sleep(int seconds) {
  seconds = (rtc.seconds + seconds) % 60;
  rtc.setAlarmSeconds(seconds);
  rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
  rtc.attachInterrupt(alarmMatch); // Attach function to interupt
  rtc.standbyMode();    // Sleep until next alarm match
}

#endif
