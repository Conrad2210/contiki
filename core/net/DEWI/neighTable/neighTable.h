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
#include "cpu.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include "net/linkaddr.h"
#include "net/mac/tsch/tsch-asn.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "net/mac/tsch/tsch.h"

#include "lib/list.h"
#include "lib/memb.h"
#include <stdio.h>



struct neighbour {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct neighbour *next;

  /* The ->addr field holds the Rime address of the neighbor, parent = holds the parent address in the cluster tree. */
  linkaddr_t addr,parent;

  /* The ->last_rssi and ->last_lqi fields hold the Received Signal
     Strength Indicator (RSSI) and CC2420 Link Quality Indicator (LQI)
     values that are received for the incoming broadcast packets. */
  int8_t last_rssi;

  /* The used transmit power and distance to the neighbour, based on RSSI */
  uint8_t distance;
  int txPW;

  /* CIDER/RLL related values, cluster stage, indication if parent or cs*/
  uint8_t stage, myCH,myCS;

  /* CIDER related values, node Degree = number of neighbours, cluster degree = number of possible cluster nodes
   * lpDegree = number of low power devices in neighbourhood*/
  uint16_t nodeDegree, clusterDegree, lpDegree;

  /* CIDER realted value: weight indication */
  float weight;

  /* last active ASN */
  struct asn_t last_asn;

  /* Each broadcast packet contains a sequence number (seqno). The
     ->last_seqno field holds the last sequenuce number we saw from
     this neighbor. */
//  uint8_t last_seqno;


};

void initNeighbourTable();

void addNeighbour(struct neighbour *neigh);
struct neighbour *getNeighbour(linkaddr_t *addr);
int checkIfNeighbourExist(linkaddr_t *addr);
int updateNeighbour(struct neighbour *neigh);
struct neighbour initNeighbour();


void printTable();

#endif /* DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_RLL_RLL_H_ */
