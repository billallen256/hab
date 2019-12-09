// vim: expandtab tabstop=2 shiftwidth=2

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
    char *name;
    uint32_t lastUpdatedTime;
    uint32_t lastLoggedTime;
    uint32_t lastTransmittedTime;
};

Status::Status(char *name) {
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

char *Status::logMessage() {
  // TODO generate a CSV log string from the class members' values
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

void update_maximums(Status *maxStatus, Status *iterStatus) {

}
