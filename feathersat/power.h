// vim: expandtab tabstop=2 shiftwidth=2

#ifndef POWER_H
#define POWER_H

// Define the battery voltage pin
// from https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
#define VBATPIN A7

// from https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
void update_voltage_data(Status *status) {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  status->voltage = measuredvbat;
}

#endif
