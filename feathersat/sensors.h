// vim: expandtab tabstop=2 shiftwidth=2

#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>			// for temp and accel sensors
#include <Adafruit_Sensor.h>		// for temp and accel sensors
#include <Adafruit_ADT7410.h>		// for temp sensor
#include <Adafruit_ADXL343.h>		// for accel sensor

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();
 
// Create the ADXL343 accelerometer sensor object
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

void setup_sensors() {
  Println("Adafruit - ADT7410 + ADX343");
 
  /* Initialise the ADXL343 */
  if(!accel.begin()) {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
 
  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);
 
  /* Initialise the ADT7410 */
  if (!tempsensor.begin()) {
    Println("Couldn't find ADT7410!");
    while (1)
      ;
  }
 
  // sensor takes 250 ms to get first readings
  delay(250);
}

void update_sensor_data() {
  /* Get a new accel. sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
 
  accelerationX = event.acceleration.x;
  accelerationY = event.acceleration.y;
  accelerationZ = event.acceleration.z;
 
  // Read and print out the temperature
  temperatureC = tempsensor.readTempC();
  Print("Temperature: "); Print(temperatureC); Println("C");
}

#endif
