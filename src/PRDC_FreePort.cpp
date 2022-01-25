/**
 * PRDC_FreePort.cpp - FreePort communication protocol for VFD
 * FreePort Protocol implemented in Arduino library
 * Implementation based on user manual for
 * Shenzhen Encom Electric Technologies Co. ENC EN600 VFD
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
 */

#include "PRDC_FreePort.h"

// begin() function
// Begin serial communication
// --------------------
void PRDC_FreePort::attach(Stream& stream) {
	_serial = &stream;
}

// getCKS() function
// Calculate checkout sum
// --------------------
void PRDC_FreePort::getCKS(char* cks, char* data, int8_t N) {
  unsigned int sum = 0;
  for (uint16_t i = 0; i < N; i++) {
    sum = sum+(uint8_t)data[i];
  }
  snprintf(cks, 5, "%04X", sum);
}

void PRDC_FreePort::getCKS(char* cks, char* data) {
  int8_t N = 0;
  for (uint16_t i = 0; i < 18; i++) {
    if(data[i] == '\0') {
      N = i;
      break;
    }
  }
  getCKS(cks, data, N);
}

// availableData() function
// Get number of available bytes
// --------------------
uint32_t PRDC_FreePort::availableData() {
  return _serial->available();
}

// readData() function
// Read data
// Inputs: 
// uint16_t DA - Auxilary device address (decimal)
// char DR[2] - Auxilary device response (ASCII)
// uint16_t FI - Failure index (decimal)
// char CI[2] - Command index (ASCII)
// uint32_t RD - Run data (decimal)
// --------------------
bool PRDC_FreePort::readData(uint16_t* DA, char* DR, uint16_t* FI, \
    char* CI, uint32_t* RD) {
  bool flagIn = 0;
  if(_serial->available()) {
    flagIn = decodeData(DA, DR, FI, CI, RD);
  }
  return flagIn;
}

// decodeData() function
// Decode data
// --------------------
bool PRDC_FreePort::decodeData(uint16_t* DA, char* DR, uint16_t* FI, \
    char* CI, uint32_t* RD) {
  bool flagIn = 0;
  char cks[5];
  while(_serial->available()) {
    uint8_t b = _serial->read();
    if(b == PRDC_FreePort_M1) {
      k = 1;
      #ifdef DEBUG_FreePort
        Serial.print("Read data: 1");
      #endif
    } else if(b == PRDC_FreePort_M2) {
      dataInTemp[k-1] = '\0';
      k = k+1;
      #ifdef DEBUG_FreePort
        Serial.print(" ");
        Serial.println(k);
        Serial.print("dataInTemp = ");
        Serial.println(dataInTemp);
      #endif
      if(k == 14 || k == 18) {
        getCKS(cks, dataInTemp, k-6);
        #ifdef DEBUG_FreePort
          Serial.print("CKS = ");
          Serial.println(cks);
          Serial.print("CKS_in = ");
          Serial.print(dataInTemp[k-6]);
          Serial.print(dataInTemp[k-5]);
          Serial.print(dataInTemp[k-4]);
          Serial.println(dataInTemp[k-3]);
          Serial.println();
        #endif
        if(cks[0] == dataInTemp[k-6] && cks[1] == dataInTemp[k-5] && \
            cks[2] == dataInTemp[k-4] && cks[3] == dataInTemp[k-3]) {
          flagIn = 1;
          *DA = h2ui(dataInTemp[0])*16 + h2ui(dataInTemp[1]);
          DR[0] = dataInTemp[2];
          DR[1] = dataInTemp[3];
          DR[2] = '\0';
          *FI = h2ui(dataInTemp[4])*16 + h2ui(dataInTemp[5]);
          CI[0] = dataInTemp[6];
          CI[1] = dataInTemp[7];
          CI[2] = '\0';
          if(k == 18) {
            *RD = h2ui(dataInTemp[8])*16*16*16 + \
              h2ui(dataInTemp[9])*16*16 + \
              h2ui(dataInTemp[10])*16 + h2ui(dataInTemp[11]);
          } else {
            *RD = 0;
          }
          break;
        }
      }
      k = 1;
    } else if(k < 17) {
      dataInTemp[k-1] = b;
      ++k;
      #ifdef DEBUG_FreePort
        Serial.print(" ");
        Serial.print(k);
      #endif
    }
  }

  return flagIn;
}

// sendData() function
// Send packet
// Inputs: 
// uint16_t DA - Auxilary device address (decimal)
// uint16_t DC - Main device command (decimal)
// uint16_t AI - Assistant index (decimal)
// uint16_t CI - Command index (decimal)
// uint32_t SD - Set data (decimal)
// --------------------
void PRDC_FreePort::sendData(char* dataOut) {
  char cks[5];
  getCKS(cks, dataOut);  
  _serial->print(PRDC_FreePort_M1);
  _serial->print(dataOut);
  _serial->print(cks);
  _serial->print(PRDC_FreePort_M2);
  
  #ifdef DEBUG_FreePort
    Serial.print(PRDC_FreePort_M1);
    Serial.print(dataOut);
    Serial.print(cks);
    Serial.println(PRDC_FreePort_M2);
  #endif
}

void PRDC_FreePort::sendData(uint16_t DA, uint16_t DC, uint16_t AI, \
    uint16_t CI) {
  char dataOut[9];
  snprintf(dataOut, 9, "%02X%02X%02X%02X", DA, DC, AI, CI);
  sendData(dataOut);
}

void PRDC_FreePort::sendData(uint16_t DA, uint16_t DC, uint16_t AI, \
    uint16_t CI, uint32_t SD) {
  char dataOut[13];
  snprintf(dataOut, 13, "%02X%02X%02X%02X%04X", DA, DC, AI, CI, (unsigned int)SD);
  sendData(dataOut);
}

// flushInput() function
// Flush input data
// --------------------
void PRDC_FreePort::flushInput() {
  while(_serial->available()) {
    _serial->read();
  }
}

// h2ui() function
// Hex to unsigned integer
// --------------------
uint8_t PRDC_FreePort::h2ui(char h) {
  if(h >= '0' && h <= '9') {
    return h - '0';
  } else if(h >= 'A' && h <= 'F') {
    return h - 'A' + 10;
  } else {
    return 0;
  }
}