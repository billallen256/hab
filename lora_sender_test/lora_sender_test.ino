// vim: expandtab tabstop=2 shiftwidth=2

#include <SPI.h>
#include <RH_RF95.h>

//#define START_SERIAL

#ifdef START_SERIAL
#define Println(a) (Serial.println(a))
#define Print(a) (Serial.print(a))
#else
#define Println(a)
#define Print(a)
#endif

// Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"

#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  #ifdef START_SERIAL
    Serial.begin(115200);
    while (!Serial) {
      delay(1);
    }
  #endif
 
  delay(100);
 
  Println("Feather LoRa TX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Println("LoRa radio init failed");
    Println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Println("setFrequency failed");
    while (1);
  }
  Print("Set Freq to: "); Println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission
 
void loop()
{
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  digitalWrite(LED_BUILTIN, HIGH);
  Println("Transmitting..."); // Send a message to rf95_server
  
  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket+13, 10);
  Print("Sending "); Println(radiopacket);
  radiopacket[19] = 0;
  
  Println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);
 
  Println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent();
  digitalWrite(LED_BUILTIN, LOW);
}
