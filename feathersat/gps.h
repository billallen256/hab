// vim: expandtab tabstop=2 shiftwidth=2

#ifndef GPS_H
#define GPS_H

#include <Adafruit_GPS.h>		// for GPS

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// record max altitude
float maxAltitude;

void setup_gps() {
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

// TODO return a uint32_t instead of a time_t
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

void update_gps_data(Status *status) {
  char c = GPS.read();

  // We're updating everything, even if
  // no new message was received,
  // even if it couldn't parse, and even
  // if there was no fix.  This info can
  // still be logged and transmitted to
  // give some sense of system status
  // (eg. it's still telemetry).
  status->newNMEAreceived = GPS.newNMEAreceived();
  status->GPSparsed = GPS.parse(GPS.lastNMEA()); // lastNMEA() also sets newNMEAreceived() to false
  status->GPSfix = GPS.fix;
  status->GPStime = get_epoch(GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds);
  status->GPSfixQuality = GPS.fixquality;
  status->GPSfixQuality3d = GPS.fixquality_3d;
  status->GPSsatelliteCount = GPS.satellites;
  status->latitude = GPS.latitudeDegrees;  // in decimal degrees
  status->longitude = GPS.longitudeDegrees;  // in decimal degrees
  status->altitude = GPS.altitude;  // in meters above MSL
  status->speed = GPS.speed;  // in knots over the ground
  status->angle = GPS.angle;

  if (GPS.altitude > maxAltitude) {
    maxAltitude = GPS.altitude;
  }

  status->maxAltitude = maxAltitude;
}

#endif
