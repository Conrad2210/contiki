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

#include "contiki.h"
#include "neighTable.h"
#include "net/DEWI/common/packet_types.h"
static struct etimer NEIGH_Print;
#define NEIGH_PRINT       (CLOCK_SECOND * 10)

#define MAX_NEIGHBOURS CONF_MAX_NEIGHBOURS

#if NEIGHBOURTABLE_LOG_LEVEL >= 1
#define DEBUG DEBUG_PRINT
#else /* TSCH_LOG_LEVEL */
#define DEBUG DEBUG_NONE
#endif /* TSCH_LOG_LEVEL */
#include "net/net-debug.h"

struct nodes_status ownStatus;

MEMB(neighbours_memb, struct neighbour, MAX_NEIGHBOURS);
LIST(neighbours_list);

PROCESS(dewi_neighbourtable_process, "DEWI Neighbour Table PROCESS");

AUTOSTART_PROCESSES();
uint8_t colours[4];
uint16_t lastNeighAddr = 0; //used to keep track which neighbour was used alreadt with CH message
//n1 is exisitng neighbour and n2 is the new data
void copyNeighbour(struct neighbour *n1, struct neighbour *n2)
{
	if (n2->addr.u16 != 0) n1->addr = n2->addr;
	if (n2->last_rssi != 0) n1->last_rssi = n2->last_rssi;
	if (n2->last_lqi != 0) n1->last_lqi = n2->last_lqi;

	if (n2->clusterDegree != 0) n1->clusterDegree = n2->clusterDegree;

	if (n2->lpDegree != 0) n1->lpDegree = n2->lpDegree;

	if (n2->myCH != 0) n1->myCH = n2->myCH;

	if (n2->myCS != 0) n1->myCS = n2->myCS;
	if (n2->myChildCH != 0) n1->myChildCH = n2->myChildCH;
	if (n2->nodeDegree != 0) n1->nodeDegree = n2->nodeDegree;

	if (n2->parent.u16 != 0) n1->parent = n2->parent;

	if (n2->COLOURINGState != 0) n1->COLOURINGState = n2->COLOURINGState;
	if (n2->CIDERState != 0) n1->CIDERState = n2->CIDERState;
	if (n2->RLLState != 0) n1->RLLState = n2->RLLState;

	if (n2->txPW != 0) n1->txPW = n2->txPW;

	if (n2->utility != 0.0) n1->utility = n2->utility;

	if (n2->last_asn.ls4b != 0) n1->last_asn = n2->last_asn;

	if (n2->isLPD != 0) n1->isLPD = n2->isLPD;

	if (n2->tier != -1) n1->tier = n2->tier;
	if (n2->msgCounter != -1) n1->msgCounter = n2->msgCounter;

	if (n2->colour != -1) n1->colour = n2->colour;
	if (n2->randNumber != -1) n1->randNumber = n2->randNumber;
	if (n2->SDI != -1) n1->SDI = n2->SDI;
	if (n2->CHDegree != -1) n1->CHDegree = n2->CHDegree;
	if (n2->UCDegree != -1) n1->UCDegree = n2->UCDegree;
	if (n2->CDegree != -1) n1->CDegree = n2->CDegree;

}

uint8_t addNeighbour(struct neighbour *neigh)
{
	struct neighbour *n;
	uint8_t newNeigh = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, &neigh->addr) == 1)
		{
			break;
		}
	}

	if (n == NULL)
	{
		n = memb_alloc(&neighbours_memb);

		/* If we could not allocate a new neighbor entry, we give up. We
		 could have reused an old neighbor entry, but we do not do this
		 for now. */
		if (n == NULL)
		{
			return newNeigh;
		}
	}
	copyNeighbour(n, neigh);

	list_add(neighbours_list, n);
	return newNeigh;
}

void getNeighbour(linkaddr_t *addr, struct neighbour *neigh)
{
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, addr))
		{
			break;
		}
	}

	if (n != NULL) copyNeighbour(neigh, n);

}
int8_t getColourParent(linkaddr_t parent)
{
	struct neighbour n;
	getNeighbour(&parent, &n);
	return n.colour;
}
uint8_t getCHDegree()
{
	uint8_t numCH = 0;
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */

		if (n->CIDERState == 5)
		{
			numCH = numCH + 1;
		}
	}
	return numCH;

}

uint8_t getColoredNodes()
{
	uint8_t numCH = 0;
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (n->CIDERState == 5 && n->colour != -1)
		{
			numCH = numCH + 1;
		}
	}
	return numCH;
}
uint8_t getUncoloredNodes()
{

	uint8_t numCH = 0;
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (n->CIDERState == 5 && n->colour == -1)
		{

			numCH = numCH + 1;
		}
	}
	return numCH;
}
uint8_t getDV()
{
	uint8_t dv = 0;
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */

		if (n->CIDERState == 5)
		{
			if (n->CHDegree > dv) dv = n->CHDegree;
		}
	}
	return dv;
}

void calcSDIs()
{

	uint16_t hSDI = 0;
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (n->CIDERState == 5)
		{
			n->SDI = (getDVStatus() + 1) * n->sDegree + n->UCDegree + n->randNumber;
			if (n->SDI > hSDI && n->colour == -1) hSDI = n->SDI;
		}
	}
	setHSDIStatus(hSDI);
}

uint8_t calcColour()
{

	int8_t colour = -1;
	uint8_t counter = 0;
	uint8_t uni = 1;
	uint8_t isUniqe = 0;
	uint8_t i = 0;
	uint8_t colourMax = sizeof(colours) / sizeof(uint8_t);
	struct neighbour *n;
	n = NULL;

	do
	{
		isUniqe = 0;
		colour = colours[counter];

		for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
		{

			if (n->CIDERState == 5)
			{
				if (n->colour == colour && n->colour != -1)
				{
					isUniqe = 1;
				}

			}
		}
		if (isUniqe == 1)
		{
			counter++;
			if (counter > colourMax)
			{
				colour = colours[random_rand() % colourMax];
				isUniqe = 0;
				printf("[Neigh]: no free Colour available, colourMax: %d, colour:%d\n", colourMax,
						colour);
			}
		}

	} while (isUniqe == 1);

	return colour;

}

uint8_t validColour()
{
	uint8_t colourUniqe = 1;
	uint8_t numNeigh = 0;

	if (getColour() == -1) return 0;
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->CIDERState == 5)
		{
			numNeigh++;
			if (n->colour == getColour() || n->colour == -1)
			{
				colourUniqe = 0;
			}
		}
	}

	if (numNeigh > 4)
		return 1;
	else return colourUniqe;
}
uint8_t getSDegree()
{

	uint8_t colours[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t colourCounter = 0;
	uint8_t itsDifferent = 1;
	struct neighbour *n;
	n = NULL;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (n->CIDERState == 5 && n->colour != -1)
		{
			uint8_t counter2 = 0;
			for (counter2 = 0; counter2 < colourCounter; counter2++)
			{
				if (colours[counter2] == n->colour)
				{
					itsDifferent = 0;
					break;
				}
			}
			if (itsDifferent == 1)
			{
				colours[colourCounter] = n->colour;
				itsDifferent = 1;
				colourCounter++;
			}
		}
	}

	return colourCounter;
}

uint8_t checkIfReadyForNextState(int currentState)
{
	struct neighbour *n;
	uint8_t ready = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (ASN_DIFF(tsch_get_current_asn(),n->last_asn) < 0x1F4) // ASN difference less than 5s; 0x1f4 = 500; 500 * 10ms = 5000ms == 5s
		{
			if (getActiveProtocol() == 0)
			{
				if (currentState > n->CIDERState)
				{
					ready = 0;
					break;
				}
			}
			else if (getActiveProtocol() == 1 && n->CIDERState == 5)
			{
				if (currentState > n->COLOURINGState)
				{
					ready = 0;
					break;
				}
			}
		}
	}

	return ready;
}

void updateNeighboursCH(uint16_t addr, linkaddr_t CHaddress, uint8_t tier)
{
	struct neighbour *n;
	uint8_t neighAdd = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (addr == n->addr.u16)
		{
			n->parent = CHaddress;
			n->CIDERState = 7;
			n->tier = tier;
			neighAdd = 1;
			break;
		}

	}
}

int checkIfNeighbourExist(linkaddr_t *addr)
{
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, addr))
		{
			break;
		}
	}

	if (n != NULL)
		return 1;
	else return 0;
}

int checkIfCHinNetwork(int currentState)
{
	struct neighbour *n;
	int isCH = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (n->CIDERState == currentState)
		{
			isCH = 1;
			break;
		}
	}
	return isCH;
}

void printTable()
{
	struct neighbour *n;
	printNodeStatus("printTable");
	PRINTF("\n");PRINTF("**** Print Neighbour Table for Node: 0x%x \n", linkaddr_node_addr.u16);
	int i = 1;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->CIDERState == 6)
		{
			PRINTF(
					"[%u] Neigh: 0x%x last RSSI: %ddBm last ASN: %x.%lx TxPower: %d dBm is LPD: %d ND: %d CD: %d LPD: %d "
					"myCH: %d myCS: %d, myChildCH: %d, Parent: 0x%2x Tier: %d, Colour: %d, CIDERState: %d Utility: %d MsgCount: %d\n",
					i, n->addr.u16, n->last_rssi, n->last_asn.ms1b, n->last_asn.ls4b, n->txPW,
					n->isLPD, n->nodeDegree, n->clusterDegree, n->lpDegree, n->myCH, n->myCS,n->myChildCH,
					n->parent, n->tier,n->colour, n->CIDERState, (uint16_t) (n->utility * 1000.0), n->msgCounter);
		}
		else if (getActiveProtocol() == 1)
		{

			PRINTF("[%u] Neigh: 0x%x,CIDERState: %d, Colour: %d, randNumber: %d, SDI: %d, CHDegree: %d, UCDegree: %d, CDegree: %d, sDegree: %d\n",
					i,n->addr.u16,n->CIDERState,n->colour,n->randNumber,n->SDI,n->CHDegree, n->UCDegree,n->CDegree,n->sDegree);
		}
		else
		{
			PRINTF(
					"[%u] Neigh: 0x%x last RSSI: %ddBm last ASN: %x.%lx TxPower: %d dBm is LPD: %d ND: %d CD: %d LPD: %d "
					"myCH: %d myCS: %d, myChildCH: %d, Parent: 0x%2x Tier: %d, Colour: %d, CIDERState: %d Utility: %d\n", i,
					n->addr.u16, n->last_rssi, n->last_asn.ms1b, n->last_asn.ls4b, n->txPW,
					n->isLPD, n->nodeDegree, n->clusterDegree, n->lpDegree, n->myCH, n->myCS,n->myChildCH,
					n->parent, n->tier,n->colour, n->CIDERState, (uint16_t) (n->utility * 1000.0));

		}
		i++;
	}

}

void printNeighbour(struct neighbour *n)
{
	if (getActiveProtocol() == 1)
	{

		PRINTF("[Neigh: 0x%x,CIDERState: %d,COLOURState: %d, Colour: %d, randNumber: %d, SDI: %d, CHDegree: %d, UCDegree: %d, CDegree: %d, sDegree: %d\n",
				n->addr.u16,n->CIDERState,n->COLOURINGState,n->colour,n->randNumber,n->SDI,n->CHDegree, n->UCDegree,n->CDegree,n->sDegree);
	}
	else
	{
		PRINTF(
				"Neigh: 0x%x last RSSI: %ddBm last ASN: %x.%lx TxPower: %d dBm is LPD: %d ND: %d CD: %d LPD: %d "
				"myCH: %d myCS: %d, myChildCH: %d, Parent: 0x%2x Tier: %d CIDERState: %d Utility: %d MsgCount: %d\n",
				n->addr.u16, n->last_rssi, n->last_asn.ms1b, n->last_asn.ls4b, n->txPW, n->isLPD,
				n->nodeDegree, n->clusterDegree, n->lpDegree, n->myCH, n->myCS, n->myChildCH,
				n->parent, n->tier, n->CIDERState, (uint16_t) (n->utility * 1000.0), n->msgCounter);
	}
}

void initNeighbourTable()
{
	/* This MEMB() definition defines a memory pool from which we allocate
	 neighbor entries. */

	/* The neighbors_list is a Contiki list that holds the neighbors we
	 have seen thus far. */

	memcpy(colours, CIDER_CONF_OFFSETS, sizeof(CIDER_CONF_OFFSETS));
	setRSSIRadius(CLUSTER_RADIUS);
	process_start(&dewi_neighbourtable_process, NULL);
}

void initNeighbour(struct neighbour *n)
{

	n->addr = linkaddr_null;
	n->clusterDegree = 0;
	n->last_rssi = 0;
	n->lpDegree = 0;
	n->myCH = 0;
	n->myCS = 0;
	n->myChildCH = 0;
	n->nodeDegree = 0;
	n->parent = linkaddr_null;
	n->CIDERState = 0;
	n->COLOURINGState = 0;
	n->RLLState = 0;
	n->txPW = 0;
	n->utility = 0.0;
	n->isLPD = 0;
	n->tier = 0;
	n->msgCounter = 0;

	n->colour = -1;
	n->randNumber = 0;
	n->SDI = 0;
	n->CHDegree = 0;
	n->UCDegree = 0;
	n->CDegree = 0;
	n->sDegree = 0;
	ASN_INIT(n->last_asn, 0, 0);

}

int deleteNeighbour(struct neighbour *neigh)
{

	list_remove(neighbours_list, neigh);
	return 1;
}

int getNumNeighbours()
{
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		number++;
	}

	return number;
}

int8_t getReceiveDirection(linkaddr_t src)
{
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (linkaddr_cmp(&src, &n->addr) != 0)
		{
			if (n->myChildCH == 1)
				return -1;
			else if (n->myCH == 1)
				return 1;
			else if (n->myCS == 1) return 0;
		}
	}
	return 0;
}

int8_t getNumChildCH()
{
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->myChildCH == 1) number++;
	}

	return number;
}

int8_t getNumCS()
{
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->myCS == 1) number++;
	}

	return number;
}

int getNumLPDevices()
{
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->isLPD == 1) number++;
	}

	return number;
}

int getNumCluster()
{
	struct neighbour *n;
	int number = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->last_rssi >= getRSSIRadius() && n->last_lqi >= getLQIRadius()) number++;
	}

	return number;
}
void resetMSGCounter()
{
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		n->msgCounter = 0;
	}
}

void updateNeighListCS(uint16_t *array, int size, linkaddr_t CHaddress)
{
	struct neighbour *n;
	struct neighbour *lastNeigh;
	uint8_t i = 0;

	uint8_t maxNeighbour = (uint8_t) list_length(neighbours_list);

	if (maxNeighbour <= size)
	{
		for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
		{
			if (n->last_rssi >= getRSSIRadius() && n->last_lqi >= getLQIRadius())
			{
				if (n->CIDERState == CIDER_CS_PING || n->CIDERState <= CIDER_UTILITY_UPDATE)
				{
					printf("[NEIGH]:new CS\n"
							"last RSSI %d, last LQI: %d CLUSTER radius: %d, child: 0x%4x\n",
							n->last_rssi, n->last_lqi, getRSSIRadius(), n->addr.u16);
					n->myCS = 1;
					if (n->myChildCH == 0) n->CIDERState = 7;
					n->parent = CHaddress;
					array[i] = n->addr.u16;
					if (i < size - 1)
						i++;
					else return;
				}
			}
			else if (n->myCS == 1)
			{
				printf("[NEIGH]: Neigh is already my CS\n"
						"last RSSI %d, last LQI: %d CLUSTER radius: %d, child: 0x%4x\n",
						n->last_rssi, n->last_lqi, getRSSIRadius(), n->addr.u16);
				n->myCS = 1;
				if (n->myChildCH == 0) n->CIDERState = 7;
				n->parent = CHaddress;
				array[i] = n->addr.u16;
				if (i < size)
					i++;
				else return;
			}
		}

	}

	else
	{

		printf("[NEIGH]: numNeighbours %d > size: %d/n", maxNeighbour, size);
		//first find last neighbour
		if (lastNeighAddr != 0)
		{
			for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
			{
				if (lastNeighAddr == n->addr.u16)
				{
					lastNeigh = n;
					break;
				}
			}
		}
		else
		{
			lastNeigh = list_head(neighbours_list);
		}

		uint8_t restartFromBeginning = 0; //for keeping track if end of list was reached;
		//second start from there in list, repeat until array is full
		for (n = lastNeigh; n != NULL; n = list_item_next(n))
		{
			if (restartFromBeginning)
			{
				n = list_head(neighbours_list);
				restartFromBeginning = 0;

			}
			if (n->last_rssi >= getRSSIRadius()&& n->last_lqi >= getLQIRadius())
			{
				if (n->CIDERState == CIDER_CS_PING || n->CIDERState <= CIDER_UTILITY_UPDATE)
				{
					printf("[NEIGH]:new CS\n"
							" last RSSI %d, CLUSTER radius: %d, child: 0x%4x\n", n->last_rssi,
							getRSSIRadius(), n->addr.u16);
					n->myCS = 1;
					if (n->myChildCH == 0) n->CIDERState = 7;
					n->parent = CHaddress;
					array[i] = n->addr.u16;
					if (i < size - 1)
						i++;
					else return;
				}
			}
			else if (n->myCS == 1)
			{
				printf("[NEIGH]: Neigh is already my CS\n"
						"last RSSI %d, CLUSTER radius: %d, child: 0x%4x\n",
						n->last_rssi, getRSSIRadius(), n->addr.u16);
				n->myCS = 1;
				if (n->myChildCH == 0) n->CIDERState = 7;
				n->parent = CHaddress;
				array[i] = n->addr.u16;
				if (i < size)
					i++;
				else return;
			}
		}

		if (list_item_next(n) == NULL)
		{
			n = list_head(neighbours_list);
			restartFromBeginning = 1;
		}
	}
}

float getAvgRSSI()
{
	struct neighbour *n;
	int number = 0;
	int16_t RSSI = 0;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		number++;

		RSSI = RSSI + n->last_rssi;
	}
	float myFloat = (float) RSSI / (float) number;

	return myFloat;
}

float getHighestUtility()
{
	float highestUtility = 0.0;
	struct neighbour *n;

	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->utility > highestUtility)
		{
			highestUtility = n->utility;
		}
	}

	return highestUtility;
}

void getMinMaxAVGRSSI()
{

	int8_t minRSSI = 0;
	int8_t maxRSSI = -120;
	int avgRSSI = 0;
	uint8_t minLQI = 255;
	uint8_t maxLQI = 0;
	int avgLQI = 0;
	uint16_t minRSSIAddr = 0;
	uint16_t maxRSSIAddr = 0;
	uint16_t minLQIAddr = 0;
	uint16_t maxLQIAddr = 0;
	uint8_t num = 0;
	uint8_t histRSSIarray[7] = { 0 };
	uint8_t histLQIarray[7] = { 0 };
	struct neighbour *n;

	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->last_rssi < minRSSI)
		{
			minRSSIAddr = n->addr.u16;
			minRSSI = n->last_rssi;
		}
		if (n->last_lqi < minLQI && n->last_lqi != 0)
		{
			minLQIAddr = n->addr.u16;
			minLQI = n->last_lqi;
		}

		if (n->last_lqi > maxLQI)
		{
			maxLQIAddr = n->addr.u16;
			maxLQI = n->last_lqi;
		}

		if (n->last_rssi > maxRSSI)
		{
			maxRSSIAddr = n->addr.u16;
			maxRSSI = n->last_rssi;
		}

		if (n->last_rssi >= -39) histRSSIarray[0] = histRSSIarray[0] + 1;
		if (n->last_rssi <= -40 && n->last_rssi >= -49) histRSSIarray[1] = histRSSIarray[1] + 1;
		if (n->last_rssi <= -50 && n->last_rssi >= -59) histRSSIarray[2] = histRSSIarray[2] + 1;
		if (n->last_rssi <= -60 && n->last_rssi >= -69) histRSSIarray[3] = histRSSIarray[3] + 1;
		if (n->last_rssi <= -70 && n->last_rssi >= -79) histRSSIarray[4] = histRSSIarray[4] + 1;
		if (n->last_rssi <= -80 && n->last_rssi >= -89) histRSSIarray[5] = histRSSIarray[5] + 1;
		if (n->last_rssi <= -90) histRSSIarray[6] = histRSSIarray[6] + 1;

		if (n->last_lqi <= 50) histLQIarray[0] = histLQIarray[0] + 1;
		if (n->last_lqi > 50 && n->last_lqi <= 70) histLQIarray[1] = histLQIarray[1] + 1;
		if (n->last_lqi > 70 && n->last_lqi <= 90) histLQIarray[2] = histLQIarray[2] + 1;
		if (n->last_lqi > 90 && n->last_lqi <= 110) histLQIarray[3] = histLQIarray[3] + 1;
		if (n->last_lqi > 110 && n->last_lqi <= 130) histLQIarray[4] = histLQIarray[4] + 1;
		if (n->last_lqi > 130 && n->last_lqi <= 150) histLQIarray[5] = histLQIarray[5] + 1;
		if (n->last_lqi > 150) histLQIarray[6] = histLQIarray[6] + 1;

		avgRSSI = avgRSSI + n->last_rssi;
		avgLQI = avgLQI + n->last_lqi;
		num = num +1;
	}

	printf(
			"[Neigh]: Min RSSI: %d, Min Addr: 0x%4x, Max RSSI: %d,Max Addr: 0x%4x,totalRSSI: %d Avg RSSI: %d, num neighbours: %d\n",
			minRSSI, minRSSIAddr, maxRSSI, maxRSSIAddr,avgRSSI, (int16_t) ((float) avgRSSI / (float) num),
			(uint8_t) list_length(neighbours_list));
	printf("0 < RSSI > -39dBm: %d\n", histRSSIarray[0]);
	printf("-40dBm < RSSI > -49dBm: %d\n", histRSSIarray[1]);
	printf("-50dBm < RSSI > -59dBm: %d\n", histRSSIarray[2]);
	printf("-60dBm < RSSI > -69dBm: %d\n", histRSSIarray[3]);
	printf("-70dBm < RSSI > -79dBm: %d\n", histRSSIarray[4]);
	printf("-80dBm < RSSI > -89dBm: %d\n", histRSSIarray[5]);
	printf("-90dBm < RSSI : %d\n", histRSSIarray[6]);


	printf(
			"\n[Neigh]: Min LQI: %d, Min Addr: 0x%4x, Max LQI: %d,Max Addr: 0x%4x,TOTAL LQI: %d, Avg LQI: %d, num neighbours: %d\n",
			minLQI, minLQIAddr, maxLQI, maxLQIAddr,avgLQI, (uint16_t) ((float) avgLQI / (float) num),
			(uint8_t) list_length(neighbours_list));

	printf("0 < LQI > 50: %d\n", histLQIarray[0]);
	printf("51 < LQI > 70: %d\n", histLQIarray[1]);
	printf("71 < LQI > 90: %d\n", histLQIarray[2]);
	printf("91 < LQI > 110: %d\n", histLQIarray[3]);
	printf("111 < LQI > 130: %d\n", histLQIarray[4]);
	printf("131 < LQI > 150: %d\n", histLQIarray[5]);
	printf("151 < LQI : %d\n", histLQIarray[6]);



}

linkaddr_t getCHChildAddress(uint8_t CIDERState)
{
	linkaddr_t returnAddr = linkaddr_null;
	float highestUtility = 0;
	struct neighbour *n;

	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->CIDERState == CIDERState)
		{
			if (n->utility > highestUtility)
			{

				highestUtility = n->utility;
				linkaddr_copy(&returnAddr, &n->addr);
			}
		}
	}

	return returnAddr;
}

uint8_t checkForPossibleCS()
{

	uint8_t msgCount = 0;
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->last_rssi >= getRSSIRadius() && n->CIDERState == CIDER_CS_PING)
		{
			PRINTF("[NEIGH]: there are still neighbours who could be mine\n");
			return 0;

		}
	}

	return 1;
}

uint8_t checkForPromotion(uint8_t CIDERState)
{

	uint8_t msgCount = 0;
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->CIDERState == CIDER_CS_PING)
		{
			msgCount = msgCount + n->msgCounter;

			if (msgCount > (20 * (getTier() + 1)))
			{
				resetMSGCounter();
				return 1;
			}

		}
	}

	return 0;

}

linkaddr_t getChildCHAddress(uint8_t CIDERState)
{

	uint8_t msgCount = 0;
	float highestUtility = -1.0;
	linkaddr_t tempAddr = linkaddr_null;
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (n->CIDERState == CIDER_CS_PING)
		{
			if (n->utility > highestUtility)
			{
				highestUtility = n->utility;
				tempAddr = n->addr;
			}

		}
	}
	if (tempAddr.u16 == linkaddr_null.u16)
	{
		for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
		{
			if (n->CIDERState == CIDER_CS && n->myCS == 1)
			{
				if (n->utility > highestUtility)
				{
					highestUtility = n->utility;
					tempAddr = n->addr;
				}

			}
		}
	}
	return tempAddr;

}

void setChildCH(linkaddr_t child)
{
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		if (linkaddr_cmp(&child, &n->addr))
		{
			n->myChildCH = 1;
		}
	}
}

PROCESS_THREAD(dewi_neighbourtable_process, ev, data)
{
	PROCESS_BEGIN()
		;
		etimer_set(&NEIGH_Print, NEIGH_PRINT);
		ownStatus.activeProtocol = 0;
		ownStatus.colour = -1;
		ownStatus.tier = 0;
		ownStatus.activeProtocol = 0; //0=CIDER;1=Colouring;2=RLL;
		ownStatus.CIDERState = 0;
		ownStatus.COLOURINGState = 0;
		ownStatus.RLLState = 0;
		ownStatus.isLPD = 0;

		ownStatus.randNumber = 0;
		ownStatus.SDI = 0;
		ownStatus.CHDegree = 0;
		ownStatus.UCDegree = 0;
		ownStatus.CDegree = 0;

		ownStatus.neighDegree = 0;
		ownStatus.clusterDegree = 0;
		ownStatus.lpDegree = 0;
		ownStatus.avgRSSI = 0;
		ownStatus.utility = 0;
		while (1)
		{

			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER)
			{

				if (etimer_expired(&NEIGH_Print))
				{
					printTable();
					etimer_reset(&NEIGH_Print);
				}
			}

		}
	PROCESS_END();

}
void clearTable()
{
while (list_head(neighbours_list) != NULL)
{
	memb_free(&neighbours_memb, list_head(neighbours_list));
	list_remove(neighbours_list, list_head(neighbours_list));
}
}

void neighbourTable_reset()
{
process_exit(&dewi_neighbourtable_process);

etimer_stop(&NEIGH_Print);
}

uint8_t checkNodesUnclustered()
{

struct neighbour *n;
for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
{
	if (n->CIDERState != CIDER_CH && n->CIDERState != CIDER_CS/*CS*/)
	{
		return 0;
	}
}

return 1;
}
uint8_t checkChildCH()
{
struct neighbour *n;
for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
{
	if (n->CIDERState == CIDER_CH && n->myChildCH == 1)
	{
		return 1;
	}
}

return 0;
}
uint8_t checkCS()
{
struct neighbour *n;
for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
{
	if (n->CIDERState == CIDER_CS && n->myCS == 1)
	{
		return 1;
	}
}

return 0;
}

int8_t getTier()
{
return ownStatus.tier;
}
void setTier(int8_t tempTier)
{
ownStatus.tier = tempTier;
}
int16_t getRSSIRadius()
{
return ownStatus.RSSIradius;
}
void setRSSIRadius(int16_t radius)
{
ownStatus.RSSIradius = radius;
}
uint8_t getLQIRadius()
{
return ownStatus.LQIradius;
}
void setLQIRadius(uint8_t radius)
{
ownStatus.LQIradius = radius;
}

int8_t getColour()
{
return ownStatus.colour;
}
void setColour(int8_t col)
{
ownStatus.colour = col;
}
int8_t getActiveProtocol()
{
return ownStatus.activeProtocol;
}
void setActiveProtocol(int8_t act)
{
ownStatus.activeProtocol = act;
}
int8_t getCHDegreeStatus()
{
return ownStatus.CHDegree;
}
void setCHDegreeStatus(int8_t act)
{
ownStatus.CHDegree = act;
}
int8_t getCIDERState()
{
return ownStatus.CIDERState;
}
void setCIDERState(int8_t CIDERState)
{
ownStatus.CIDERState = CIDERState;
}
int8_t getCOLOURINGState()
{
return ownStatus.COLOURINGState;
}
void setCOLOURINGState(int8_t state)
{
ownStatus.COLOURINGState = state;
}
int8_t getRLLState()
{
return ownStatus.RLLState;
}
void setRLLState(int8_t state)
{
ownStatus.RLLState = state;
}
int8_t getLPD()
{
return ownStatus.isLPD;
}
void setLPD(int8_t LPD)
{
ownStatus.isLPD = LPD;
}

linkaddr_t getParentStatus()
{
return ownStatus.parent;
}
void setParentStatus(linkaddr_t addr)
{
ownStatus.parent = addr;
}

uint16_t getRandNumberStatus()
{
return ownStatus.randNumber;
}
void setRandNumberStatus(uint16_t randNumber)
{
ownStatus.randNumber = randNumber;
}
uint16_t getSDIStatus()
{
return ownStatus.SDI;
}
void setSDIStatus(uint16_t sdi)
{
ownStatus.SDI = sdi;
}

uint16_t getHSDIStatus()
{
return ownStatus.hSDI;
}
void setHSDIStatus(uint16_t sdi)
{
ownStatus.hSDI = sdi;
}
uint8_t getUCDegreeStatus()
{
return ownStatus.UCDegree;
}
void setUCDegreeStatus(uint8_t sdi)
{
ownStatus.UCDegree = sdi;
}
uint8_t getCDegreeStatus()
{
return ownStatus.CDegree;
}
void setCDegreeStatus(uint8_t sdi)
{
ownStatus.CDegree = sdi;
}
uint8_t getSDegreeStatus()
{
return ownStatus.sDegree;
}

uint8_t getDVStatus()
{
return ownStatus.dV;
}
void setDVStatus(uint8_t dv)
{
ownStatus.dV = dv;
}
void setSDegreeStatus(uint8_t sdi)
{
ownStatus.sDegree = sdi;
}
float getNeighDegree()
{
return ownStatus.neighDegree;
}
void setNeighDegree(float nd)
{
ownStatus.neighDegree = nd;
}
float getClusDegree()
{
return ownStatus.clusterDegree;
}
void setClusDegree(float cd)
{
ownStatus.clusterDegree = cd;
}
float getLPDegree()
{
return ownStatus.lpDegree;
}
void setLPDegree(float lpd)
{
ownStatus.lpDegree = lpd;
}
float getAVGRSSI()
{
return ownStatus.avgRSSI;
}
void setAVGRSSI(float rssi)
{
ownStatus.avgRSSI = rssi;
}
float getUtility()
{
return ownStatus.utility;
}
void setUtility(float ut)
{
ownStatus.utility = ut;
}
void printNodeStatus(char fromFunction[])
{
PRINTF("\n\n******************************************************\n"
		"\t\tNode Status from %s"
		"\n******************************************************\n",fromFunction);PRINTF("Active Protocol: %d, CIDERState: %d, COLOURINGState: %d, RLLState: %d,Tier: %d, Colour: %d, isLPD: %d\n"
		"randNum: %d, SDI: %d, CHDegree: %d, UCDegree: %d, CDegree: %d, dV: %d, sDegree: %d, hSDI: %d\n"
		"NeighDegree: %d, clusterDegree: %d, lpDegree: %d, avgRSSI: %d. utility: %d\n\n\n",
		ownStatus.activeProtocol,ownStatus.CIDERState,ownStatus.COLOURINGState,ownStatus.RLLState, ownStatus.tier, ownStatus.colour,
		ownStatus.isLPD,ownStatus.randNumber,ownStatus.SDI,ownStatus.CHDegree,ownStatus.UCDegree,ownStatus.CDegree,
		ownStatus.dV, ownStatus.sDegree, ownStatus.hSDI,
		(uint16_t)(ownStatus.neighDegree * 1000), (uint16_t)(ownStatus.clusterDegree * 1000),
		(uint16_t)(ownStatus.lpDegree * 1000),(uint16_t)(ownStatus.avgRSSI * 1000)
		,(uint16_t)(ownStatus.utility * 1000));
}

void printNodeStatusForced()
{
printf("\n\n******************************************************\n"
		"\t\tNode Status from Button pressed"
		"\n******************************************************\n");
printf(
		"Active Protocol: %d, CIDERState: %d, COLOURINGState: %d, RLLState: %d,Tier: %d, Colour: %d, isLPD: %d\n"
				"randNum: %d, SDI: %d, CHDegree: %d, UCDegree: %d, CDegree: %d, dV: %d, sDegree: %d, hSDI: %d\n"
				"NeighDegree: %d, clusterDegree: %d, lpDegree: %d, avgRSSI: %d. utility: %d"
				"LQIRadius: %d, RSSIRadius %d\n\n\n",
		ownStatus.activeProtocol, ownStatus.CIDERState, ownStatus.COLOURINGState,
		ownStatus.RLLState, ownStatus.tier, ownStatus.colour, ownStatus.isLPD, ownStatus.randNumber,
		ownStatus.SDI, ownStatus.CHDegree, ownStatus.UCDegree, ownStatus.CDegree, ownStatus.dV,
		ownStatus.sDegree, ownStatus.hSDI, (uint16_t) (ownStatus.neighDegree * 1000),
		(uint16_t) (ownStatus.clusterDegree * 1000), (uint16_t) (ownStatus.lpDegree * 1000),
		(uint16_t) (ownStatus.avgRSSI * 1000), (uint16_t) (ownStatus.utility * 1000),
		getLQIRadius(),getRSSIRadius());
}

uint8_t getChildAddresses(linkaddr_t array[])
{
uint8_t i = 0;
struct neighbour *n;
for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
{
	if ((n->myCS == 1) || (n->myChildCH == 1))
	{
		array[i] = n->addr;
		i++;
	}
}
return i;
}

