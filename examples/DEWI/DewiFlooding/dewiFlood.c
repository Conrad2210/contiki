/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Testing the broadcast layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include "project-config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include "timesynch.h"
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/


uint16_t rxPackets = 0, txPackets = 0;
linkaddr_t coord = { { 1, 0 } };

struct FloodPacket
{
	uint16_t seqNo;
	uint16_t hops;
	rtimer_clock_t timestamp;
};

struct FloodPacket packet;
int numBurst = 200, numMessages = 20;
int countBurst = 0, countMessages = 0;
uint16_t seq = 0, lastSeq = 0;
static struct broadcast_conn broadcast;
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
	struct FloodPacket tempPacket;
	memcpy(&tempPacket, packetbuf_dataptr(), sizeof(struct FloodPacket));
	if (lastSeq < tempPacket.seqNo && (tempPacket.seqNo - lastSeq) < 10)
	{
		rxPackets = rxPackets +1;
		rtimer_clock_t lat = timesynch_time() - tempPacket.timestamp;
		unsigned long latency = (unsigned long)(lat) * 1e6 / RTIMER_SECOND;
		printf("[APP]: timesynch_time: %u, timestap: %u\n",timesynch_time(),tempPacket.timestamp);
		printf("[APP]: MSG received: hopcount: %d, seq: %d, rxPackets: %d, latency %lu.%03lu ms\n", tempPacket.hops, tempPacket.seqNo,rxPackets,latency / 1000, latency % 1000);

		lastSeq = tempPacket.seqNo;
		tempPacket.hops = tempPacket.hops + 1;
		packetbuf_copyfrom(&tempPacket, sizeof(struct FloodPacket));
		broadcast_send(&broadcast);
	}
}
static const struct broadcast_callbacks broadcast_call = { broadcast_recv };


;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
	static struct etimer burst, message;

	PROCESS_EXITHANDLER(broadcast_close(&broadcast)
	;
	)

	PROCESS_BEGIN()
		;

		broadcast_open(&broadcast, 129, &broadcast_call);
		if (linkaddr_cmp(&coord, &linkaddr_node_addr))
			timesynch_set_authority_level(0);
		etimer_set(&burst, CLOCK_SECOND * 60);
		while (1)
		{
			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER)
			{
				if (linkaddr_cmp(&coord, &linkaddr_node_addr))
				{
					if (data == &message)
					{
						if (countMessages < numMessages)
						{
							packet.hops = 0;
							seq = seq + 1;
							packet.seqNo = seq;
							packet.timestamp = timesynch_time();
							printf("broadcast message sent with seqNo: %d, hopCount: %d\n", packet.seqNo, packet.hops);
							packetbuf_copyfrom(&packet, sizeof(struct FloodPacket));
							broadcast_send(&broadcast);
							countMessages = countMessages + 1;
							etimer_set(&message, CLOCK_SECOND * 0.1);
							txPackets = txPackets + 1;
						} else
						{
							if (countBurst < numBurst)
							{
								etimer_set(&burst, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
								countBurst = countBurst + 1;
								countMessages = 0;
								printf("Burst finished: %d, next burst starts in a few seconds\n", countBurst);
							}
							else
							{
								printf("[APP]: Experiments finsihed, txpackets: %d\n",txPackets);
							}

						}
					}
					if (data == &burst)
					{
						packet.hops = 0;
						seq = seq + 1;
						printf("seqNo: %d\n", seq);
						packet.seqNo = seq;
						packet.timestamp = timesynch_time();
						printf("broadcast message sent with seqNo: %d, hopCount: %d\n", packet.seqNo, packet.hops);
						packetbuf_copyfrom(&packet, sizeof(struct FloodPacket));
						broadcast_send(&broadcast);
						countMessages = countMessages + 1;
						etimer_set(&message, CLOCK_SECOND * 0.1);
						txPackets = txPackets +1;
					}
				}
			}

		}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
