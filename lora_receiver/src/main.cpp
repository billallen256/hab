#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

// for feather m0 RFM9x (m0 with built-in radio)
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void configure_led() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void configure_radio() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
}

void configure_serial_monitor() {
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
}

void initialize_radio() {
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
}

void set_radio_frequency(float freq) {
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(freq)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(freq);
}

void set_radio_power(int8_t power) {
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void setup()
{
  configure_led();
  configure_radio();
  configure_serial_monitor();
  Serial.println("Feather LoRa RX Test!");
  initialize_radio();
  set_radio_frequency(434.0);
  set_radio_power(23);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
 
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED_BUILTIN, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}