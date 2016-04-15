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


int isCoord = 0;
PROCESS(dewi_scheduler_process, "DEWI scheduler PROCESS");

void setCoord(int isCoordinator){
	isCoord = isCoordinator;
}


uint16_t setSchedule(ScheduleInfo schedule){

	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(schedule.handle);

	tsch_schedule_remove_slotframe(tempHandle);


	tempHandle = tsch_schedule_add_slotframe(schedule.handle,schedule.slotframeLength);

	int i = 0;
	for(i = 0; i < MAX_NUM_LINKS; i++)
	{
		if(schedule.links[i] != NULL)
		{
			linkInfo temp = schedule.links[i];
#if DEBUG
			printf("ADD LINK: %u, %u\n",temp.timeslot, temp.channel_offset);
#endif
			if(tsch_schedule_add_link(tempHandle->handle, 	//slotframe handle
					temp.link_options,					//link option
					temp.link_type,						//liunke type
					0xffff,								//link address
					temp.timeslot,						//timeslot
					temp.channel_offset) == NULL)		//channeloffset
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
	PROCESS_BEGIN();

	/* Configure the user button */
#if DEBUG
	printf("Create Schedule update\n");
#endif

	PROCESS_END();
}


