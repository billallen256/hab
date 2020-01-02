// vim: expandtab tabstop=2 shiftwidth=2

struct status {
    uint32_t elapsedTime;  // mission elapsed time from the RTC in seconds
    uint32_t GPStime;  // time received from the GPS
    bool newNMEAreceived;
    bool GPSparsed;
    bool GPSfix;
    uint8_t GPSfixQuality;
    uint8_t GPSfixQuality3d;
    uint8_t GPSsatelliteCount;
    float latitude;
    float longitude;
    float maxAltitude;
    float altitude;
    float angle;
    float speed;
    float temperatureC;
    float accelerationX;
    float accelerationY;
    float accelerationZ;
    float voltage;
};

typedef struct status Status;

void reset_status(Status *status) {
  status->elapsedTime = 0;
  status->GPStime = 0;
  status->newNMEAreceived = false;
  status->GPSparsed = false;
  status->GPSfix = false;
  status->GPSfixQuality = 0;
  status->GPSfixQuality3d = 0;
  status->GPSsatelliteCount = 0;
  status->latitude = 0.0;
  status->longitude = 0.0;
  status->maxAltitude = 0.0;
  status->altitude = 0.0;
  status->angle = 0.0;
  status->speed = 0.0;
  status->temperatureC = 1000.0;
  status->accelerationX = 10000.0;
  status->accelerationY = 10000.0;
  status->accelerationZ = 10000.0;
  status->voltage = 0.0;
}

unsigned char *status_bytes(Status *status) {
  // TODO generate a byte representation of the struct to log and transmit
}
