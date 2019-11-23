// vim: expandtab tabstop=2 shiftwidth=2

#include <time.h>      // to convert to epoch time
#include <RTCZero.h>   // for built-in real-time-clock

//
// for RFM9x radio
//
#include <SPI.h>			// for RFM9x radio and logger
#include <RH_RF95.h>			// for RFM9x radio
#include "callsign.h"			// for RFM9x radio

#include <Adafruit_GPS.h>		// for GPS

#include <SD.h>				// for logger

#include <Wire.h>			// for temp and accel sensors
#include <Adafruit_Sensor.h>		// for temp and accel sensors
#include <Adafruit_ADT7410.h>		// for temp sensor
#include <Adafruit_ADXL343.h>		// for accel sensor

// from https://github.com/cavemoa/Feather-M0-Adalogger/blob/master/SimpleSleep/SimpleSleep.ino
RTCZero rtc;

// from https://learn.adafruit.com/radio-featherwing/wiring#feather-m0-3-10
#define RFM95_CS  10   // "B"
#define RFM95_RST 11   // "A"
#define RFM95_INT  6   // "D"

#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Define the battery voltage pin
// from https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();
 
// Create the ADXL343 accelerometer sensor object
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

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

class Status {
  public:
    bool newNMEAreceived;
    bool GPSparsed;
    bool GPSfix;
    uint8_t GPSfixQuality;
    uint8_t GPSfixQuality3d;
    uint8_t GPSsatelliteCount;
    float latitude;
    float longitude;
    float altitude;
    float angle;
    float speed;
    float temperatureC;
    float accelerationX;
    float accelerationY;
    float accelerationZ;
    float voltage;

    Status();
    void reset();
    void updateTime();
    bool needsToBeLogged();
    bool needsToBeTransmitted();

  private:
    String name;
    uint32_t lastUpdatedTime;
    uint32_t lastLoggedTime;
    uint32_t lastTransmittedTime;
};

Status::Status(String name) {
  this->name = name;
  this->lastUpdatedTime = 0;
  this->reset();
}

void Status::reset() {
  this->newNMEAreceived = false;
  this->GPSparsed = false;
  this->GPSfix = false;
  this->GPSfixQuality = 0;
  this->GPSfixQuality3d = 0;
  this->GPSsatelliteCount = 0;
  this->latitude = 0.0;
  this->longitude = 0.0;
  this->altitude = 0.0;
  this->temperatureC = 1000.0;
  this->accelerationX = 10000.0;
  this->accelerationY = 10000.0;
  this->accelerationZ = 10000.0;
  this->voltage = 0.0;
}

void Status::updateTime() {
  this->lastUpdatedTime = now(); // TODO figure out now()
}

void Status::updateLoggedTime() {
  this->lastLoggedTime = now();
}

void Status::updateTransmittedTime() {
  this->lastTransmittedTime = now();
}

bool Status::needsToBeLogged() {
  if (this->lastLoggedTime > this->lastUpdatedTime) {
    return true;
  }

  return false;
}

bool Status::needsToBeTransmitted() {
  if (this->lastUpdatedTime > this->lastTransmittedTime) {
    return true;
  }

  return false;
}

// reuse the same Status instance in each loop
Status iterStatus("Iter");

// another Status instance to hold maximum values encountered
Status maxStatus("Max");

void reset_radio() {
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
}

void setup_radio() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  reset_radio();

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

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

void setup_logger() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }

  Serial.println("initialization done.");
}

void setup_sensors() {
  Serial.println("Adafruit - ADT7410 + ADX343");
 
  /* Initialise the ADXL343 */
  if(!accel.begin()) {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
 
  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);
 
  /* Initialise the ADT7410 */
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find ADT7410!");
    while (1)
      ;
  }
 
  // sensor takes 250 ms to get first readings
  delay(250);
}

void setup_rtc() {
  rtc.begin();
  rtc.setEpoch(0);  // January 1, 1970
}

void setup() {
  // We don't want to eat power by constantly
  // transmitting beeps if we're in a fast
  // crash-reboot loop, so adding a delay on
  // startup.
  //delay(30000);  TODO uncomment this line when debugging is done

  Serial.begin(115200);
  setup_radio();

  // We want a last-resort way to locate
  // the payload with direction finding
  // if everything else crashes and reboots.
  transmit_beep();

  setup_rtc();
  setup_gps();
  update_gps_data(&iterStatus);

  // If we can transmit GPS coordinates,
  // that's way more useful than just a beep.
  transmit_location();

  setup_logger();
  setup_sensors();
}

void transmit_message(char *message, int message_length) {  // TODO should take String, bool
  rf95.send((uint8_t *)message, message_length);
}

// from https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
void update_voltage_data(Status *status) {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  status->voltage = measuredvbat;
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

  // set RTC to GPS time
  if (status->GPSparsed) {
    time_t epoch = get_epoch(GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds);
    rtc.setEpoch(epoch + GPS.secondsSinceTime());
  }
}

void log_message(String filename, String message, bool execute) {
  if execute {
    log_file = SD.open(filename, FILE_WRITE);

    if (log_file) {
      log_file.println(message);
      log_file.close();
    } else {
      Serial.println("Could not open log file");
    }
  }
}

void update_sensor_data() {
  /* Get a new accel. sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
 
  accelerationX = event.acceleration.x;
  accelerationY = event.acceleration.y;
  accelerationZ = event.acceleration.z;
 
  // Read and print out the temperature
  temperatureC = tempsensor.readTempC();
  Serial.print("Temperature: "); Serial.print(temperatureC); Serial.println("C");
}

void update_maximums(Status *maxStatus, Status *iterStatus) {

}

void loop() {
  iterStatus.reset();
  update_voltage_data(&iterStatus);
  update_gps_data(&iterStatus);
  update_sensor_data(&iterStatus);
  update_maximums(&maxStatus, &iterStatus);
  String iterMessage = iterStatus.logMessage();
  String maxMessage = maxStatus.logMessage();
  log_message("status.csv", iterMessage, iterStatus.needsToBeLogged());
  log_message("maximums.csv", maxMessage, maxStatus.needsToBeLogged());
  transmit_message(iterMessage, iterStatus.needsToBeTransmitted());
  transmit_message(maxMessage, maxStatus.needsToBeTransmitted());
  deep_sleep(30);  // TODO figure out deep sleep
}
