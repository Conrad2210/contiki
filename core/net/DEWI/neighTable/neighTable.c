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
 *         neighTable: Contains all neighbours of each node, provides access to all protocols
 *
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#include "neighTable.h"

static struct etimer NEIGH_timer, NEIGH_Print;
#define NEIGH_INTERVAL       (CLOCK_SECOND * 600)
#define NEIGH_PRINT       (CLOCK_SECOND * 10)

#define MAX_NEIGHBOURS CONF_MAX_NEIGHBOURS
#define DEBUG DEBUG_PRINT

MEMB(neighbours_memb, struct neighbour, MAX_NEIGHBOURS);
LIST(neighbours_list);

PROCESS(dewi_neighbourtable_process, "DEWI Neighbour Table PROCESS");

AUTOSTART_PROCESSES();

//n1 is exisitng neighbour and n2 is the new data
void copyNeighbour(struct neighbour *n1, struct neighbour *n2) {
	if (n2->addr.u16 != 0)
		n1->addr = n2->addr;
	if (n2->last_rssi != 0)
		n1->last_rssi = n2->last_rssi;

	if (n2->clusterDegree != 0)
		n1->clusterDegree = n2->clusterDegree;

	if (n2->lpDegree != 0)
		n1->lpDegree = n2->lpDegree;

	if (n2->myCH != 0)
		n1->myCH = n2->myCH;

	if (n2->myCS != 0)
		n1->myCS = n2->myCS;

	if (n2->nodeDegree != 0)
		n1->nodeDegree = n2->nodeDegree;

	if (n2->parent.u16 != 0)
		n1->parent = n2->parent;

	if (n2->stage != 0)
		n1->stage = n2->stage;

	if (n2->txPW != 0)
		n1->txPW = n2->txPW;

	if (n2->utility != 0)
		n1->utility = n2->utility;

	if (n2->last_asn.ls4b != 0)
		n1->last_asn = n2->last_asn;

	if (n2->isLPD != 0)
		n1->isLPD = n2->isLPD;

	if (n2->tier != -1)
		n1->tier = n2->tier;

}

uint8_t addNeighbour(struct neighbour *neigh) {
	struct neighbour *n;
	uint8_t newNeigh = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, &neigh->addr) == 1) {
			break;
		}
	}

	if (n == NULL) {
		n = memb_alloc(&neighbours_memb);

		/* If we could not allocate a new neighbor entry, we give up. We
		 could have reused an old neighbor entry, but we do not do this
		 for now. */
		if (n == NULL) {
			return newNeigh;
		}
	}
	copyNeighbour(n, neigh);

	list_add(neighbours_list, n);
	return newNeigh;
}

struct neighbour *getNeighbour(linkaddr_t *addr) {
	struct neighbour *n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, addr)) {
			break;
		}
	}
	if (n == NULL)
		return NULL;
	else
		return n;

}

uint8_t checkIfReadyForNextStep(int currentStep) {
	struct neighbour *n;
	uint8_t ready = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (currentStep > n->stage) {
			ready = 0;
			break;
		}

	}
	return ready;
}

void updateNeighboursCH(uint16_t addr, linkaddr_t CHaddress) {
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (addr == n->addr.u16) {
			n->parent = CHaddress;
			break;
		}

	}
}

int checkIfNeighbourExist(linkaddr_t *addr) {
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, addr)) {
			break;
		}
	}

	if (n != NULL)
		return 1;
	else
		return 0;
}

int checkIfCHinNetwork(int currentStep) {
	struct neighbour *n;
	int isCH = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (n->stage == currentStep) {
			isCH = 1;
			break;
		}
	}
	return isCH;
}

void printTable() {
	struct neighbour *n;
	printf("\n");
	printf("**** Print Neighbour Table for Node: 0x%x \n",
			linkaddr_node_addr.u16);
	int i = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		printf(
				"[%u] Neigh: 0x%x last RSSI: %ddBm last ASN: %x.%lx TxPower: %d dBm is LPD: %d ND: %d CD: %d LPD: %d "
						"myCH: %d myCS: %d Parent: 0x%2x Tier: %d Stage: %d Utility: %d \n",
				i, n->addr.u16, n->last_rssi, n->last_asn.ms1b,
				n->last_asn.ls4b, n->txPW, n->isLPD, n->nodeDegree,
				n->clusterDegree, n->lpDegree, n->myCH, n->myCS, n->parent,
				n->tier, n->stage, n->utility);
		i++;
	}

	printf("\n");
}

void initNeighbourTable() {
	/* This MEMB() definition defines a memory pool from which we allocate
	 neighbor entries. */

	/* The neighbors_list is a Contiki list that holds the neighbors we
	 have seen thus far. */

	process_start(&dewi_neighbourtable_process, NULL);
}

struct neighbour initNeighbour() {
	struct neighbour n;
	n.addr = linkaddr_null;
	n.clusterDegree = 0;
	n.last_rssi = 0;
	n.lpDegree = 0;
	n.myCH = 0;
	n.myCS = 0;
	n.nodeDegree = 0;
	n.parent = linkaddr_null;
	n.stage = 0;
	n.txPW = 0;
	n.utility = 0;
	n.isLPD = 0;
	n.tier = -1;
	ASN_INIT(n.last_asn, 0, 0);
	return n;
}

int deleteNeighbour(struct neighbour *neigh) {

	list_remove(neighbours_list, neigh);
	return 1;
}

int getNumNeighbours() {
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		number++;
	}

	return number;
}

int getNumLPDevices() {
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		if (n->isLPD == 1)
			number++;
	}

	return number;
}

int getNumCluster() {
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		if (n->last_rssi >= -90 * 0.8)
			number++;
	}

	return number;
}

void updateNeighListCS(uint16_t *array, int size, linkaddr_t CHaddress) {
	struct neighbour *n;
	int i = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		if (n->last_rssi >= -90 * 0.8) {
			n->myCS = 1;
			n->parent = CHaddress;
			array[i] = n->addr.u16;
			if (i < size - 1)
				i++;
		}
	}

}

float getAvgRSSI() {
	struct neighbour *n;
	int number = 0;
	int16_t RSSI = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		number++;

		RSSI = RSSI + n->last_rssi;
	}
	float myFloat = (float) RSSI / (float) number;

	return myFloat;
}

int getHighestUtility() {
	int highestUtility = 0;
	struct neighbour *n;

	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		printf("Highes utility> %d, current utility: %d\n", highestUtility,
				n->utility);
		if (n->utility > highestUtility) {
			highestUtility = n->utility;
		}
	}

	return highestUtility;
}

linkaddr_t getCHChildAddress(int stage) {

	int highestUtility = 0;
	struct neighbour *n, tempN = initNeighbour();

	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n)) {
		if (n->stage == stage) {
			if (n->utility > tempN.utility) {

				tempN.utility = n->utility;
				tempN.addr = n->addr;
			}
		}
	}

	return tempN.addr;
}

PROCESS_THREAD(dewi_neighbourtable_process, ev, data) {
	PROCESS_BEGIN()
		;
		etimer_set(&NEIGH_timer, NEIGH_INTERVAL);
		etimer_set(&NEIGH_Print, NEIGH_PRINT);
		while (1) {

			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER) {
				if (etimer_expired(&NEIGH_timer)) {
					struct neighbour *n;
					struct asn_t cur_asn = tsch_get_current_asn();
					for (n = list_head(neighbours_list); n != NULL; n =
							list_item_next(n)) {

						/* We break out of the loop if the address of the neighbor matches
						 *
						 *
						 the address of the neighbor from which we received this
						 broadcast message. */

						if (ASN_DIFF(cur_asn,n->last_asn) > 3000 && n->myCS == 0
								&& n->parent.u16 == 0) {
							printf(
									"Remove Neighbour: 0x%x, because of inactivity\n",
									n->addr.u16);
							deleteNeighbour(n);
							n = list_head(neighbours_list);
						}

					}
					etimer_reset(&NEIGH_timer);
				} else if (etimer_expired(&NEIGH_Print)) {
					printTable();
					etimer_reset(&NEIGH_Print);
				}
			}

		}
	PROCESS_END();

}

void neighbourTable_reset() {
process_exit(&dewi_neighbourtable_process);
while (list_head(neighbours_list) != NULL)
	list_remove(neighbours_list, list_head(neighbours_list));

etimer_stop(&NEIGH_timer);
etimer_stop(&NEIGH_Print);
}
