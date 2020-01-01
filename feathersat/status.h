// vim: expandtab tabstop=2 shiftwidth=2

// TODO change this to a struct since a class isn't providing any benefits here
class Status {
  public:
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

    Status();
    void reset();
    void updateTime();
    bool needsToBeLogged();
    bool needsToBeTransmitted();
};

Status::Status() {
  this->reset();
}

void Status::reset() {
  this->elapsedTime = 0;
  this->GPStime = 0;
  this->newNMEAreceived = false;
  this->GPSparsed = false;
  this->GPSfix = false;
  this->GPSfixQuality = 0;
  this->GPSfixQuality3d = 0;
  this->GPSsatelliteCount = 0;
  this->latitude = 0.0;
  this->longitude = 0.0;
  this->maxAltitude = 0.0;
  this->altitude = 0.0;
  this->angle = 0.0;
  this->speed = 0.0;
  this->temperatureC = 1000.0;
  this->accelerationX = 10000.0;
  this->accelerationY = 10000.0;
  this->accelerationZ = 10000.0;
  this->voltage = 0.0;
}

unsigned char *Status::logBytes() {
  // TODO generate a byte representation of the struct to log and transmit
}
