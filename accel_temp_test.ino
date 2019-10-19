#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADT7410.h>
#include <Adafruit_ADXL343.h>

float tempC, accelX, accelY, accelZ;
 
// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();
 
// Create the ADXL343 accelerometer sensor object
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

void setup() {
  // start the serial connection
  Serial.begin(115200);
 
  // wait for serial monitor to open
  while (!Serial)
    ;
 
  Serial.println("Adafruit - ADT7410 + ADX343");
 
  /* Initialise the ADXL343 */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
 
  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);
 
  /* Initialise the ADT7410 */
  if (!tempsensor.begin())
  {
    Serial.println("Couldn't find ADT7410!");
    while (1)
      ;
  }
 
  // sensor takes 250 ms to get first readings
  delay(250);
}

void loop() {
  /* Get a new accel. sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
 
  accelX = event.acceleration.x;
  accelY = event.acceleration.y;
  accelZ = event.acceleration.z;
 
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(accelX); Serial.print("  ");
  Serial.print("Y: "); Serial.print(accelY); Serial.print("  ");
  Serial.print("Z: "); Serial.print(accelZ); Serial.print("  ");Serial.println("m/s^2 ");
  
  // Read and print out the temperature
  tempC = tempsensor.readTempC();
  Serial.print("Temperature: "); Serial.print(tempC); Serial.println("C");
  
  delay(2000);
}
