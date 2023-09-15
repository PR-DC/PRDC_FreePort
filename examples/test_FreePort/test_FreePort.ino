// Test FreePort
// Author: Milos Petrasinovic <mpetrasinovic@pr-dc.com>
// PR-DC, Republic of Serbia
// info@pr-dc.com
// --------------------

// Library
// --------------------
// PRDC RS485 Half Duplex
// Author: PRDC
#include <PRDC_RS485HD_STM32.h>

// PRDC FreePort
// Author: PRDC
#include <PRDC_FreePort.h>

// Define variables
// --------------------
#define DE_PIN PB4 // MAX485 DE pin
#define RE_PIN PB3 // MAX485 RE pin

// Serial communications
#define protocolMax 65534
#define SERIAL_BAUDRATE 1000000
PRDC_RS485HD_STM32 VFD_HS(USART1);
PRDC_FreePort VFD;
uint8_t FP_DA; // Auxilary device address (decimal)
char FP_DR[3]; // Auxilary device response (ASCII)
uint8_t FP_FI; // Failure index (decimal)
char FP_CI[3]; // Command index (ASCII)
uint16_t FP_RD; // Run data (decimal)

// LED control variables
unsigned long t_LED; // [us] time
unsigned long dt_LED = 1000; // [us] time difference
bool s_LED = true; // [-] LED state flag

// setup function
// --------------------
void setup() {
  // Define pin modes
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Communication ettings
  Serial.begin(SERIAL_BAUDRATE);
  VFD_HS.setPins(DE_PIN, RE_PIN);
  VFD_HS.begin(57600, SERIAL_7E1);
  VFD.attach(VFD_HS);
}

// loop function
// --------------------
void loop(){
  Serial.println("010A0000");
  VFD.sendData(1, 10, 0, 0); // Look up auxiliary motor state
  //VFD.sendData((char*)"010A0000\0");
  //VFD.sendData(1, 12, 0, 1, 4000); // Set  frequency to 40.00 Hz
  //VFD.sendData((char*)"010C00010FA0\0");
  delay(10);
  while(VFD.availableData() > 2) {
    bool flagIn = VFD.readData(&FP_DA, FP_DR, &FP_FI, FP_CI, &FP_RD);
    if(flagIn) {
      Serial.print("Address: ");
      Serial.println(FP_DA);
      Serial.print("Response: ");
      Serial.println(FP_DR);
      Serial.print("Failure index: ");
      Serial.println(FP_FI);
      Serial.print("Command index: ");
      Serial.println(FP_CI);
      Serial.print("Run data: ");
      Serial.println(FP_RD);
      Serial.println();
    }
  }
}