/**
 * PRDC_FreePort.h - FreePort communication protocol for VFD
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

//#define DEBUG_FreePort

#ifndef _PRDC_FreePort_H_
#define _PRDC_FreePort_H_
#include "Arduino.h"

#define PRDC_FreePort_M1 '~' // Start bit - marker (~)
#define PRDC_FreePort_M2 '\r' // Stop bit - marker (\r)
#define FreePort_MAX_PACKET_SIZE 16

#include <SoftwareSerial.h>

class PRDC_FreePort {
  public:
    void attach(Stream& stream);
    bool readData(uint16_t*, char*, uint16_t*, char*, uint32_t*);
    void sendData(char*);
    void sendData(uint16_t, uint16_t, uint16_t, uint16_t);
    void sendData(uint16_t, uint16_t, uint16_t, uint16_t, uint32_t);
    uint32_t availableData();
    void flushInput();

  private:
    Stream* _serial;
    char dataInTemp[FreePort_MAX_PACKET_SIZE+1];  // data in buffer
    uint16_t k = 0;
    char cksIn[5];
    void getCKS(char*, char*, int8_t);
    void getCKS(char*, char*);
    bool decodeData(uint16_t*, char*, uint16_t*, char*, uint32_t*);
    static uint8_t h2ui(char);
};
#endif // _PRDC_FreePort_H_