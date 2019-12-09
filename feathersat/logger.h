// vim: expandtab tabstop=2 shiftwidth=2

#include <SPI.h>
#include <SD.h>

void setup_logger() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }

  Serial.println("initialization done.");
}

void log_message(char *filename, char *message, bool execute) {
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
