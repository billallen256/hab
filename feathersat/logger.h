// vim: expandtab tabstop=2 shiftwidth=2

#include <SPI.h>
#include <SD.h>

void setup_logger() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);  // TODO might be better to reset so at least the beacon will still work
  }

  Serial.println("initialization done.");
}

void write_log(char *filename, unsigned char *message) {
  log_file = SD.open(filename, FILE_WRITE);

  if (log_file) {
    log_file.println(message);  // TODO want to write bytes here
    log_file.close();
  } else {
    Serial.println("Could not open log file");
  }
}
