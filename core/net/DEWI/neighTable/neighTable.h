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
 *         neighTable header file
 *
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#ifndef DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_NEIGHTABLE_NEIGHTABLE_H_
#define DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_NEIGHTABLE_NEIGHTABLE_H_

#include "contiki.h"
//#include "cpu.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
//#include "dev/sys-ctrl.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include "net/linkaddr.h"
#include "net/mac/tsch/tsch-asn.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "net/mac/tsch/tsch.h"

#include "lib/list.h"
#include "lib/memb.h"
#include <stdio.h>

struct neighbour
{
		/* The ->next pointer is needed since we are placing these on a
		 Contiki list. */
		struct neighbour *next;

		/* The ->addr field holds the Rime address of the neighbor, parent = holds the parent address in the cluster tree. */
		linkaddr_t addr, parent;

		/* The ->last_rssi and ->last_lqi fields hold the Received Signal
		 Strength Indicator (RSSI) and CC2420 Link Quality Indicator (LQI)
		 values that are received for the incoming broadcast packets. */
		int16_t last_rssi;

		int txPW;

		/* CIDER/RLL related values, cluster state, indication if parent or cs, is low power device (LPD), tier*/
		uint8_t CIDERState,COLOURINGState,RLLState, myCH, myCS,myChildCH, isLPD;
		int8_t tier;

		/* CIDER related values, node Degree = number of neighbours, cluster degree = number of possible cluster nodes
		 * lpDegree = number of low power devices in neighbourhood*/
		uint16_t nodeDegree, clusterDegree, lpDegree;

		/* CIDER realted value: weight indication */
		float utility;

		/* RLL related values, cluster state, indication if parent or cs, is low power device (LPD)*/

		/* last active ASN */
		struct asn_t last_asn;

		/* Each broadcast packet contains a sequence number (seqno). The
		 ->last_seqno field holds the last sequenuce number we saw from
		 this neighbor. */
//  uint8_t last_seqno;
// Msg counter for ping msg's
		uint8_t msgCounter;

		/**********************************************
		 *
		 * 			Values for coloring
		 *
		 **********************************************/

		int8_t colour;
		uint16_t randNumber;
		uint16_t SDI;
		uint8_t CHDegree;
		uint8_t UCDegree;
		uint8_t CDegree;
		uint8_t dV;
		uint8_t sDegree;

};


struct nodes_status{
		uint8_t activeProtocol; //0=CIDER;1=Colouring;2=RLL;
		uint8_t CIDERState;
		uint8_t COLOURINGState;
		uint8_t RLLState;
		linkaddr_t parent;
		int8_t tier;
		uint8_t isLPD;

		int8_t colour;
		uint16_t randNumber;
		uint16_t SDI;
		uint8_t CHDegree;
		uint8_t UCDegree;
		uint8_t CDegree;
		uint8_t dV;
		uint8_t sDegree;
		uint16_t hSDI;

		float neighDegree;
		float clusterDegree;
		float lpDegree;
		float avgRSSI;
		float utility;

};

void initNeighbourTable();

uint8_t addNeighbour(struct neighbour *neigh);
void getNeighbour(linkaddr_t *addr, struct neighbour *neigh);
int checkIfNeighbourExist(linkaddr_t *addr);
int updateNeighbour(struct neighbour *neigh);
void initNeighbour(struct neighbour *n);
int8_t getReceiveDirection(linkaddr_t);
int8_t getNumChildCH();
int8_t getNumCS();
int getNumNeighbours();
int getNumLPDevices();
int getNumCluster();
float getAvgRSSI();
float getHighestUtility();
void updateNeighListCS(uint16_t *array, int size, linkaddr_t CHaddress);
void resetMSGCounter();
void setChildCH(linkaddr_t child);
uint8_t checkIfReadyForNextState(int currentState);
void updateNeighboursCH(uint16_t addr, linkaddr_t CHaddress, uint8_t tier);
void printTable();
void printNeighbour(struct neighbour *n);
void neighbourTable_reset();
linkaddr_t getCHChildAddress(uint8_t state);
int checkIfCHinNetwork();
void clearTable();
uint8_t checkForPromotion(uint8_t state);
linkaddr_t getChildCHAddress(uint8_t state);
uint8_t checkNodesUnclustered();
uint8_t checkChildCH();
uint8_t getCHDegree();
uint8_t getColoredNodes();
uint8_t getUncoloredNodes();
uint8_t getSDegree();
uint8_t getDV();
uint8_t calcColour();
uint8_t validColour();
int8_t getColourParent(linkaddr_t);





void printNodeStatus();
int8_t getTier();
void setTier(int8_t);
int8_t getColour();
void setColour(int8_t);
int8_t getActiveProtocol();
void setActiveProtocol(int8_t);
int8_t getCIDERState();
void setCIDERState(int8_t);
int8_t getCOLOURINGState();
void setCOLOURINGState(int8_t);
int8_t getRLLState();
void setRLLState(int8_t);
int8_t getLPD();
void setLPD(int8_t);

linkaddr_t getParentStatus();
void setParentStatus(linkaddr_t addr);

uint16_t getRandNumberStatus();
void setRandNumberStatus(uint16_t);
uint16_t getSDIStatus();
void setSDIStatus(uint16_t);
uint8_t getUCDegreeStatus();
void setUCDegreeStatus(uint8_t);
uint8_t getCDegreeStatus();
void setCDegreeStatus(uint8_t);
uint8_t getDVStatus();
void setDVStatus(uint8_t);
uint8_t getSDegreeStatus();
void setSDegreeStatus(uint8_t);

int8_t getCHDegreeStatus();
void setCHDegreeStatus(int8_t act);

void setHSDIStatus(uint16_t);
uint16_t getHSDIStatus();

float getNeighDegree();
void setNeighDegree(float);
float getClusDegree();
void setClusDegree(float);
float getLPDegree();
void setLPDegree(float);
float getAVGRSSI();
void setAVGRSSI(float);
float getUtility();
void setUtility(float);


void calcSDIs();

#endif /* DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_RLL_RLL_H_ */
