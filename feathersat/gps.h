// vim: expandtab tabstop=2 shiftwidth=2

#include <Adafruit_GPS.h>		// for GPS

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

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
  status->GPSfixQuality = GPS.fixquality;
  status->GPSfixQuality3d = GPS.fixquality_3d;
  status->GPSsatelliteCount = GPS.satellites;
  status->latitude = GPS.latitude;
  status->longitude = GPS.longitude;
  status->altitude = GPS.altitude;
  status->speed = GPS.speed;
  status->angle = GPS.angle;

}
