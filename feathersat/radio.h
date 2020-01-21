// vim: expandtab tabstop=2 shiftwidth=2

#ifndef RADIO_H
#define RADIO_H

#include <string.h>
#include <SPI.h>			// for RFM9x radio and logger
#include <RH_RF95.h>			// for RFM9x radio
#include "callsign.h"			// for RFM9x radio
#include "serial.h"

// from https://learn.adafruit.com/radio-featherwing/wiring#feather-m0-3-10
#define RFM95_RST 11   // "A"
#define RFM95_CS  10   // "B"
#define RFM95_INT  6   // "D"

#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void reset_radio() {
  Println("Resetting radio");
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  Println("Radio reset complete");
}

void setup_radio() {
  Println("setup_radio()");
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  reset_radio();

  while (!rf95.init()) {
    Println("Radio init failed");
    Println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  Println("Radio init complete");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Println("setFrequency() failed");
    while (1);
  }

  Print("Freq set to: ");
  Println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void transmit_message(char *message, int message_length) {
  Print("Sending message of length ");
  Println(message_length);
  rf95.send((uint8_t *)message, message_length);
  Println("Waiting for packet to complete");
  delay(10);
  rf95.waitPacketSent();
  Println("Packet complete");
}

#endif
