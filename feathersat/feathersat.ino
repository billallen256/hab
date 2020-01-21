// vim: expandtab tabstop=2 shiftwidth=2

#include "serial.h"
#include "status.h"
#include "radio.h"
#include "rtc.h"
#include "gps.h"
#include "logger.h"
#include "power.h"
#include "sensors.h"
#include "sleep.h"

// reuse the same Status instance in each loop
Status iterStatus();

void setup() {
  // We don't want to eat power by constantly
  // transmitting beeps if we're in a fast
  // crash-reboot loop, so adding a delay on
  // startup.
  //delay(30000);  TODO uncomment this line when debugging is done

  setup_serial();  // TODO comment out START_SERIAL when debugging is done
  setup_radio();

  // We want a last-resort way to locate
  // the payload with direction finding
  // if everything else crashes and reboots.
  transmit_beep();

  iterStatus.reset();
  maxAltitude = 0.0;

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
  reset_status(&iterStatus);
  update_rtc_data(&iterStatus);
  update_voltage_data(&iterStatus);
  update_gps_data(&iterStatus);
  update_sensor_data(&iterStatus);
  unsigned char *iterBytes = status_bytes(&iterStatus);
  write_log("status.log", iterBytes);
  transmit_log(iterBytes);
  deep_sleep(45);  // 45 seconds.  Can't be set to more than 60 seconds.
}
