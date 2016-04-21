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
 *         CIDER: Clustering protocol for dense environments, working on top of TSCH
 *         see 'CIDER - Clustering In Dense EnviRonments' - XXX - 2016
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#include "cider.h"

int CIDER_isActive = 0;

static struct etimer CIDER_ping_timer;
#define CIDER_PING_INTERVAL       (CLOCK_SECOND * 15)
PROCESS(dewi_cider_ping_process, "DEWI cider ping PROCESS");

static void cider_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{
	struct CIDER_PACKET *temp = packetbuf_dataptr();
	printf("*** Received CIDER packet %u bytes from %u:%u: '0x%04x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], *(uint16_t *) packetbuf_dataptr());


	if(temp->subType == CIDER_PING)
	{
		printf("*** Received CIDER PING message ***\n");
	}
}

static const struct broadcast_callbacks cider_bc_rx =
{ cider_packet_received };


static struct broadcast_conn cider_bc;

int CIDER_notify()
{
	//todo: Implement check if RLL;
	if (CIDER_isActive == 0)
		CIDER_isActive = 1;
	else
		CIDER_isActive = 0;

	CIDER_createSchedule();

	return 1;
}

int CIDER_getIsActive()
{
	return CIDER_isActive;
}

void CIDER_createSchedule()
{
	if (CIDER_isActive == 1)
	{
		ScheduleInfo_t temp;
		temp.handle = 0x00;
		temp.slotframeLength = 51;
		int i;
		printf("[CIDER]: Create CIDER schedule\n");
		for (i = 0; i < temp.slotframeLength; i++)
		{
			temp.links[i].addr = &tsch_broadcast_address;
			temp.links[i].channel_offset = 0;
			temp.links[i].isActive = 1;

			if (i == 0)
			{
				temp.links[i].link_type = LINK_TYPE_ADVERTISING;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING;
			}
			else
			{
				temp.links[i].link_type = LINK_TYPE_NORMAL;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX;
			}

			temp.links[i].timeslot = i;
		}

		setSchedule(temp);
	}
	else
	{
		clearSchedule();
	}
}

PROCESS_THREAD(dewi_scheduler_coord_process, ev, data){
	struct CIDER_PACKET CIDERPacket;


	PROCESS_EXITHANDLER(broadcast_close(&cider_bc))
	broadcast_open(&cider_bc, BROADCAST_CHANNEL_CIDER, &cider_bc_rx);
	PROCESS_BEGIN();

	etimer_set(&CIDER_ping_timer, CIDER_PING_INTERVAL);

	while(1)
	{
		PROCESS_YIELD();
		if (ev == PROCESS_EVENT_TIMER)
		{
			CIDERPacket = createScheduleUpdate();
			packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

			broadcast_send(&cider_bc);
			etimer_set(&CIDER_ping_timer, CIDER_PING_INTERVAL);

		}
	}

}
}
