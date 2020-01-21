// vim: expandtab tabstop=2 shiftwidth=2

#ifndef LOGGER_H
#define LOGGER_H

#include <SPI.h>
#include <SD.h>

void setup_logger() {
  Print("Initializing SD card...");

  if (!SD.begin(4)) {
   Println("initialization failed!");
    while (1);  // TODO might be better to reset so at least the beacon will still work
  }

  Println("initialization done.");
}

void write_log(char *filename, unsigned char *message) {
  log_file = SD.open(filename, FILE_WRITE);

  if (log_file) {
    log_file.println(message);  // TODO want to write bytes here
    log_file.close();
  } else {
    Println("Could not open log file");
  }
}

#endif
