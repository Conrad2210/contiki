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

PROCESS(dewi_scheduler_process, "DEWI scheduler PROCESS");

static void schedule_update_received(struct broadcast_conn *c, const linkaddr_t *from)
{
	printf("*** Received Schedule Update %u bytes from %u:%u: '0x%04x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], *(uint16_t *) packetbuf_dataptr());
}

static const struct broadcast_callbacks schedule_bc_rx ={ schedule_update_received };
static struct broadcast_conn schedule_bc;


void setCoord(int isCoordinator){
	isCoord = isCoordinator;
}


uint16_t setSchedule(ScheduleInfo_t schedule){

	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(schedule.handle);
	tsch_schedule_remove_slotframe(tempHandle);


	tempHandle = tsch_schedule_add_slotframe(schedule.handle,schedule.slotframeLength);


#if DEBUG
	printf("start adding links\n");
#endif

	int i = 0;
	for(i = 0; i < MAX_NUM_LINKS; i++)
	{
		if(schedule.links[i].isActive == 1)
		{
			linkInfo temp = schedule.links[i];
#if DEBUG
			printf("ADD LINK: %u, %u\n",temp.timeslot, temp.channel_offset);
#endif
			if(tsch_schedule_add_link(tempHandle,temp.link_options,	temp.link_type,	&temp.addr,temp.timeslot,temp.channel_offset) == NULL)
				{
#if DEBUG
				printf("Add schedule caused an error, EXIT \n");
#endif
				break;
				}
		}
	}

	return tempHandle->handle;

}

PROCESS_THREAD(dewi_scheduler_process, ev, data)
{
	PROCESS_EXITHANDLER(broadcast_close(&schedule_bc))
	PROCESS_BEGIN();
	etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);
	/* Configure the user button */
#if DEBUG
	printf("Create Schedule update\n");
#endif

	while(1){
		PROCESS_YIELD();
		if (ev == PROCESS_EVENT_TIMER)
			{
				printf("create schedule update packet\n");
				etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);
			}
	}
	PROCESS_END();
}


int initScheduler(){
// process_start(&dewi_scheduler_process,NULL);
 return 1;
}
