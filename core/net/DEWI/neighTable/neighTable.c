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

#define MAX_NEIGHBOURS CONF_MAX_NEIGHBOURS
#define DEBUG DEBUG_PRINT
void initNeighbourTable()
{
#if DEBUG
	printf("Init Neighbourtable\n");
#endif
	/* This MEMB() definition defines a memory pool from which we allocate
	 neighbor entries. */
	MEMB(neighburs_memb, struct neighbour, MAX_NEIGHBOURS);

	/* The neighbors_list is a Contiki list that holds the neighbors we
	 have seen thus far. */
	LIST(neighbours_list);

}

void addNeighbour(struct neighbour neigh)
{
	list_add(neighbours_list,neigh);
}
struct neighbour *getNeighbour(linkaddr_t addr)
{
	struct neighbor *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, &addr))
		{
			break;
		}
	}

	return n;

}
int checkIfNeighbourExist(linkaddr_t addr)
{
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, &addr))
		{
			break;
		}
	}

	if(n != NULL)
		return 1;
	else
		return 0;
}
int updateNeighbour(struct neighbour neigh)
{
	struct neighbour *n;
	for (n = list_head(neighbours_list); n != NULL; n = list_item_next(n))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (linkaddr_cmp(&n->addr, neigh.addr))
		{
			break;
		}
	}

	if(n == NULL)
		return 0;

	n->addr = neigh.addr;
	n->last_lqi = neigh.last_lqi;
	n->last_rssi = neigh.last_rssi;
	n->last_seqno = neigh.last_seqno;

	list_add(neighbours_list,n);
}

