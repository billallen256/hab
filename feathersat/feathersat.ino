// vim: expandtab tabstop=2 shiftwidth=2

#include "status.h"
#include "radio.h"
#include "rtc.h"
#include "gps.h"
#include "logger.h"
#include "power.h"
#include "sensors.h"
#include "sleep.h"

// reuse the same Status instance in each loop
Status iterStatus("Iter");

// another Status instance to hold maximum values encountered
Status maxStatus("Max");

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

  iterStatus.reset();
  maxStatus.reset();

  setup_rtc();
  setup_gps();

  // If we can transmit GPS coordinates,
  // that's way more useful than just a beep.
  update_gps_data(&iterStatus);
  transmit_location();

  setup_logger();
  setup_sensors();
}

void loop() {
  iterStatus.reset();
  update_voltage_data(&iterStatus);
  update_gps_data(&iterStatus);
  update_rtc(&iterStatus);  // must happen after update_gps()
  update_sensor_data(&iterStatus);
  update_maximums(&maxStatus, &iterStatus);
  String iterMessage = iterStatus.logMessage();
  String maxMessage = maxStatus.logMessage();
  log_message("status.csv", iterMessage, iterStatus.needsToBeLogged());
  log_message("maximums.csv", maxMessage, maxStatus.needsToBeLogged());
  transmit_message(iterMessage, iterStatus.needsToBeTransmitted());
  transmit_message(maxMessage, maxStatus.needsToBeTransmitted());
  deep_sleep(45);  // 45 seconds.  Can't be set to more than 60 seconds.
}
