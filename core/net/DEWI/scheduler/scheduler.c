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
 *         Scheduler: scheduler entity for the dewi-app.
 *
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#include "scheduler.h"

#define DEBUG DEBUG_PRINT
#define SCHEDULE_INTERVAL       (CLOCK_SECOND * 5)
static struct etimer scheduleUpdate;
int isCoord = 0;
int activeSchedule = -1;

PROCESS(dewi_scheduler_coord_process, "DEWI scheduler PROCESS for coordinator");
PROCESS(dewi_scheduler_node_process, "DEWI scheduler PROCESS for node");

static void schedule_update_received(struct broadcast_conn *c, const linkaddr_t *from)
{
	struct scheduleUpdate_Packet *temp = packetbuf_dataptr();
	printf("[SCHEDULER]: Received Schedule Update %u bytes from %u:%u: '0x%04x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], *(uint16_t *) packetbuf_dataptr());
	if(activeSchedule != temp->schedule)
	{
		printf("[SCHEDULER]: Update Schedule, active is now %u\n",temp->schedule);
		activeSchedule = temp->schedule;

		switch(activeSchedule){
			case 0: //do stuff for CIDER;
				CIDER_notify();
				break;
			case 1: //todo: stuff for RLL
				break;
			default:
				//just do nothing
				break;
		}
	}
	else
	{
		printf("[SCHEDULER]: No new schedule received, Schedule %u is still active\n",temp->schedule);
	}
}

static const struct broadcast_callbacks schedule_bc_rx =
{ schedule_update_received };
static struct broadcast_conn schedule_bc;

void setCoord(int isCoordinator)
{
	isCoord = isCoordinator;
}

uint16_t setSchedule(ScheduleInfo_t schedule)
{

	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(schedule.handle);
	tsch_schedule_remove_slotframe(tempHandle);

	tempHandle = tsch_schedule_add_slotframe(schedule.handle, schedule.slotframeLength);

	int i = 0;
	for (i = 0; i < MAX_NUM_LINKS; i++)
	{
		if (schedule.links[i].isActive == 1)
		{
			linkInfo_t temp = schedule.links[i];

			if (tsch_schedule_add_link(tempHandle, temp.link_options, temp.link_type, temp.addr, temp.timeslot, temp.channel_offset) == NULL)
			{
				break;
			}
		}
	}

	return tempHandle->handle;

}

void clearSchedule(){

	int i;
	//remove everyhing except ADV slot
	for(i = 1; i < MAX_NUM_LINKS; i++)
	{
		tsch_schedule_remove_link(tsch_schedule_get_slotframe_by_handle(0),tsch_schedule_get_link_by_handle(i));
	}
}

struct scheduleUpdate_Packet createScheduleUpdate()
{
	struct scheduleUpdate_Packet temp;
	temp.base.dst = tsch_broadcast_address;
	temp.base.src = linkaddr_node_addr;

	temp.schedule = 0;
	return temp;
}

PROCESS_THREAD(dewi_scheduler_coord_process, ev, data)
{
	struct scheduleUpdate_Packet scheduleUpdatePacket;
	PROCESS_EXITHANDLER(broadcast_close(&schedule_bc))
	broadcast_open(&schedule_bc, BROADCAST_CHANNEL_SCHEDULE, &schedule_bc_rx);
	PROCESS_BEGIN();


		etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);

		/* Configure the user button */
#if DEBUG
		printf("[SCHEDULER]: Create Schedule update\n");
#endif

		while(1)
		{
			PROCESS_YIELD();
			if (ev == PROCESS_EVENT_TIMER)
			{
				scheduleUpdatePacket = createScheduleUpdate();
				packetbuf_copyfrom(&scheduleUpdatePacket, sizeof(struct scheduleUpdate_Packet));
#if TSCH_WITH_LINK_SELECTOR
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);
#endif
				broadcast_send(&schedule_bc);
				etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);

			}
		}

PROCESS_END();
}

PROCESS_THREAD(dewi_scheduler_node_process, ev, data)
{
	struct scheduleUpdate_Packet scheduleUpdatePacket;
	PROCESS_EXITHANDLER(broadcast_close(&schedule_bc))
	broadcast_open(&schedule_bc, BROADCAST_CHANNEL_SCHEDULE, &schedule_bc_rx);
	PROCESS_BEGIN();

		//etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);


		while(1)
		{
			PROCESS_YIELD();
			if (ev == PROCESS_EVENT_TIMER)
			{



			}
		}

PROCESS_END();
}

int initScheduler()
{
	if(isCoord){
#if DEBUG
		printf("[SCHEDULER]: Start Scheduler for coordinator\n");
#endif
		//if satrtup set CIDER active
		CIDER_notify();
		process_start(&dewi_scheduler_coord_process, NULL);

	}
	else{
#if DEBUG
		printf("[SCHEDULER]: Start Scheduler for node\n");
#endif
		process_start(&dewi_scheduler_node_process, NULL);
	}
return 1;
}

void scheduleMessage(int timeslots, void* callback){

printf("[SCHEDULER]: Schedule msg in: %d timeslots, callback: %u",timeslots,callback);

}
