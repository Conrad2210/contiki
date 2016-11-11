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
 *         RLL: Dissemination protocol for broadcast data, working on top of TSCH
 *         see 'RLL - Reliable Low Latency Data Dissemination Protocol' - ETFA 2016
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */
#include "contiki.h"
#include "rll.h"

#include "project-conf.h"
#define RLL_INTERVAL       (CLOCK_SECOND * 5)
uint8_t RLLSeqNo = 0;

PROCESS(dewi_rll_process, "DEWI rll PROCESS");
static void rll_packet_received(struct netflood_conn *c,
		const linkaddr_t *from) {

	struct RLL_PACKET *temp = packetbuf_dataptr();

	applicationDataCallback(temp->appData,c->last_originator_seqno);
}


static struct etimer RLL_timer;
static struct netflood_conn rll_flood;
static const struct netflood_callbacks rll_netflood_rx = { rll_packet_received };

void sendRLLMessage(struct APP_PACKET dataPacket){
	struct RLL_PACKET RLLPacket;
	RLLPacket.base.dst = linkaddr_null;
	RLLPacket.base.src = linkaddr_node_addr;
	RLLPacket.base.type = RLL;
	RLLPacket.subType = RLL_DATA;
	RLLPacket.appData = dataPacket;
	packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
	netflood_send(&rll_flood,RLLSeqNo++);
}
void rllInit(){

	process_start(&dewi_rll_process,NULL);
}

PROCESS_THREAD(dewi_rll_process, ev, data) {
	PROCESS_EXITHANDLER(netflood_close(&rll_flood));

	PROCESS_BEGIN()
		;
etimer_set(&RLL_timer,CLOCK_SECOND*5);
	netflood_open(&rll_flood,2*CLOCK_SECOND,BROADCAST_CHANNEL_RLL,&rll_netflood_rx);
		while (1) {
			PROCESS_YIELD();
			if (ev == PROCESS_EVENT_TIMER) {
				printf("[RLL]: RLL active\n");
				etimer_set(&RLL_timer,CLOCK_SECOND*5);
			}

		}
		PROCESS_END();
}
