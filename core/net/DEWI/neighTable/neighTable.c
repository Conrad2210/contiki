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
static struct etimer NEIGH_timer,NEIGH_Print;
#define NEIGH_INTERVAL       (CLOCK_SECOND * 30)
#define NEIGH_PRINT       (CLOCK_SECOND * 60)
PROCESS(dewi_neighbourtable_process, "DEWI Neighbour Table PROCESS");

AUTOSTART_PROCESSES();
#define MAX_NEIGHBOURS CONF_MAX_NEIGHBOURS
#define DEBUG DEBUG_PRINT

MEMB(neighbours_memb, struct neighbour, MAX_NEIGHBOURS);
LIST(neighbours_list);

void copyNeighbour(struct neighbour *n1, struct neighbour *n2)
{
	n1->addr = n2->addr;
	n1->last_rssi = n2->last_rssi;
	n1->clusterDegree = n2->clusterDegree;
	n1->distance = n2->distance;
	n1->lpDegree = n2->lpDegree;
	n1->myCH = n2->myCH;
	n1->myCS = n2->myCS;
	n1->nodeDegree = n2->nodeDegree;
	n1->parent = n2->parent;
	n1->stage = n2->stage;
	n1->txPW = n2->txPW;
	n1->weight = n2->weight;
	n1->last_asn = n2->last_asn;

}

void addNeighbour(struct neighbour *neigh)
{
	struct neighbour *n;
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
			return;
		}
	}
	copyNeighbour(n, neigh);

	list_add(neighbours_list, n);
}

struct neighbour *getNeighbour(linkaddr_t *addr)
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
	if (n == NULL)
		return NULL;
	else
		return n;

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
	else
		return 0;
}

void printTable()
{
	struct neighbour *n;
	printf("\n");
	printf("**** Print Neighbour Table for Node: 0x%x\n", linkaddr_node_addr.u16);
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{
		printf("Neigh: 0x%x last RSSI: %ddBm last ASN: %x.%lx TxPower: %d dBm\n",n->addr.u16, n->last_rssi, n->last_asn.ms1b, n->last_asn.ls4b,	n->txPW);

	}

	printf("\n");
}

void initNeighbourTable()
{
#if DEBUG
	printf("[NEIGH]: Init Neighbourtable\n");
#endif
	/* This MEMB() definition defines a memory pool from which we allocate
	 neighbor entries. */

	/* The neighbors_list is a Contiki list that holds the neighbors we
	 have seen thus far. */

	process_start(&dewi_neighbourtable_process, NULL);
}

struct neighbour initNeighbour()
{
	struct neighbour n;
	n.addr = linkaddr_null;
	n.clusterDegree = 0;
	n.distance = 0;
	n.last_rssi = 0;
	n.lpDegree = 0;
	n.myCH = 0;
	n.myCS = 0;
	n.nodeDegree = 0;
	n.parent = linkaddr_null;
	n.stage = 0;
	n.txPW = 0;
	n.weight = 0.0;
	ASN_INIT(n.last_asn, 0, 0);
	return n;
}

int deleteNeighbour(struct neighbour *neigh){

	list_remove(neighbours_list,neigh);
return 1;
}

PROCESS_THREAD(dewi_neighbourtable_process, ev, data)
{
	PROCESS_BEGIN();
	etimer_set(&NEIGH_timer, NEIGH_INTERVAL);
	etimer_set(&NEIGH_Print, NEIGH_PRINT);
	printf("[NEIGH]: Neighbour table, start ***\n");
	while (1)
	{

		PROCESS_YIELD()
		;
		if (ev == PROCESS_EVENT_TIMER){
			if(etimer_expired(&NEIGH_timer)){
			struct neighbour *n;
			struct asn_t cur_asn = tsch_get_current_asn();
			printf("[NEIGH]: Neighbour table, check for inactive members ***\n");
			for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
			{

				/* We break out of the loop if the address of the neighbor matches
				 *
				 *
				 the address of the neighbor from which we received this
				 broadcast message. */

				if(ASN_DIFF(cur_asn,n->last_asn) > 3000)
				{
					printf("Remove Neighbour: 0x%x, because of inactivity\n",n->addr.u16);
					deleteNeighbour(n);
					n = list_head(neighbours_list);
				}

			}
			etimer_reset(&NEIGH_timer);
			}
			else if(etimer_expired(&NEIGH_Print)){
				printTable();
				etimer_reset(&NEIGH_Print);
			}
		}

	}
	PROCESS_END();

}
