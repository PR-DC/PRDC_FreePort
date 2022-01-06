/**
 * test_FreePort.ino - Test for FreePort Protocol implemented in Arduino library
 * Author: Milos Petrasinovic <mpetrasinovic@pr-dc.com>
 * PR-DC, Republic of Serbia
 * info@pr-dc.com
 * 
 * --------------------
 * Copyright (C) 2021 PR-DC <info@pr-dc.com>
 *
 * This file is part of PRDC_FreePort.
 *
 * PRDC_FreePort is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 * 
 * PRDC_FreePort is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with PRDC_FreePort.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
 
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
#define DE_PIN PB4 // RS485 transceiver DE pin
#define RE_PIN PB3 // RS485 transceiver RE pin

// Serial communications
#define SERIAL_BAUDRATE 1000000
PRDC_RS485HD_STM32 VFD_HS(USART1);
PRDC_FreePort VFD;
uint16_t FP_DA; // Auxilary device address (decimal)
char FP_DR[3]; // Auxilary device response (ASCII)
uint16_t FP_FI; // Failure index (decimal)
char FP_CI[3]; // Command index (ASCII)
uint32_t FP_RD; // Run data (decimal)

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