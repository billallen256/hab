// vim: expandtab tabstop=2 shiftwidth=2

#ifndef SERIAL_H
#define SERIAL_H

// We only want serial output enabled when we're hooked up to the computer.

#define START_SERIAL

#ifdef START_SERIAL
#define Println(a) (Serial.println(a))
#define Print(a) (Serial.print(a))
#else
#define Println(a)
#define Print(a)
#endif

#endif

void setup_serial() {
  #ifdef START_SERIAL
    Serial.begin(115200);
    while (!Serial) {
      delay(1);
    }
  #endif
}
