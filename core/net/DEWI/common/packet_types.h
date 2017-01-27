/*
 * Copyright (c) 2015, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *			Packet Types header files
 *
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#ifndef DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_COMMON_PACKET_TYPES_H_
#define DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_COMMON_PACKET_TYPES_H_

#include "mac/tsch/tsch-asn.h"

enum packettype
{
    SCHEDULE_UPDATE, CIDER, RLL, COLOURING, APP
};
enum CIDERsubpackettype
{
    CIDER_PING = 1,
    CIDER_NEIGHBOUR_UPDATE = 2,
    CIDER_UTILITY_UPDATE = 3,
    CIDER_CH_COMPETITION = 4,
    CIDER_CH = 5,
    CIDER_CS_PING = 6,
    CIDER_CS = 7,
    CIDER_CH_PROMOTE = 10,
    CIDER_CH_PROMOTE_ACK = 11,
    CIDER_LP_PING = 12,
    CIDER_COMPLETE = 100,
    CIDER_UNCOMPLETE = 101,
    CIDER_SETRSSIRADIUS = 150,
    CIDER_SETLQIRADIUS = 151,
    CIDER_SETTXPOWER = 152,
    CIDER_UNDEFINED = 200
};
enum COLOURINGsubpackttype
{
    COLOUR_UPDATE = 1,
    COLOUR_RELEASE = 2,
    COLOUR_WAIT_COMPLETE = 100,
    COLOUR_COMPLETE = 101,
    COLOUR_UNCOMPLETE = 102,
    COLOUR_FINISHED = 103
};

enum RLLsubpackettype
{
    RLL_PING, RLL_DATA
};
enum APPsubpackettype
{
    APP_RESET,
    APP_COLOR,
    APP_BRIGHTNESS,
    APP_SENSORDATA,
    APP_TOPOLOGYREQUEST,
    APP_TOPOLOGYREPLY,
    APP_STATSRESET,
    APP_EXPERIMENT,
    APP_RESULTREQUEST,
    APP_RESULTREPLY,
    APP_ACK
};

struct BasePacket
{
    linkaddr_t src;
    linkaddr_t dst;
    enum packettype type;
};

struct COLOURING_PACKET
{
    struct BasePacket base;
    enum COLOURINGsubpackttype subType;
    uint16_t args[5];
};

struct APP_PACKET
{
    enum APPsubpackettype subType;
    linkaddr_t src;
    linkaddr_t dst;
    uint16_t seqNo;
    struct asn_t timeSend;
    uint8_t timeslot[23];
    uint16_t values[23];
    uint8_t remainingData;
    uint8_t battery;
    uint8_t temperature;
    uint16_t count;
};

struct CIDER_PACKET
{
    struct BasePacket base;
    enum CIDERsubpackettype subType;
    uint16_t args[50];
    uint8_t tier, colour;
    linkaddr_t parent;
};
struct RLL_PACKET
{
    struct BasePacket base;
    enum RLLsubpackettype subType;
    linkaddr_t initalSRC;
    struct APP_PACKET appData;
    uint16_t seqNo;
    uint8_t priority;

};

#endif
