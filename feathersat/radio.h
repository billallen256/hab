// vim: expandtab tabstop=2 shiftwidth=2

//
// for RFM9x radio
//
#include <SPI.h>			// for RFM9x radio and logger
#include <RH_RF95.h>			// for RFM9x radio
#include "callsign.h"			// for RFM9x radio

// from https://learn.adafruit.com/radio-featherwing/wiring#feather-m0-3-10
#define RFM95_CS  10   // "B"
#define RFM95_RST 11   // "A"
#define RFM95_INT  6   // "D"

#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void reset_radio() {
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
}

void setup_radio() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  reset_radio();

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void transmit_message(char *message, int message_length) {  // TODO should take String, bool
  rf95.send((uint8_t *)message, message_length);
}

