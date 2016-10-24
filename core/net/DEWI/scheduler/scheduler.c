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

int8_t tier = -1;
void setActiveSchedule(uint8_t schedule) {

	if (activeSchedule != schedule) {
		printf(
				"[SCHEDULER]: new schedule received, Schedule %u is active now\n",
				schedule);
		activeSchedule = schedule;
		switch (activeSchedule) {
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
}

void setCoord(int isCoordinator) {
	isCoord = isCoordinator;
}

int getCoord() {
	return isCoord;
}

uint8_t getActiveSchedule() {
	return (uint8_t) activeSchedule;
}

uint16_t setSchedule(ScheduleInfo_t schedule) {

	tsch_schedule_remove_all_slotframes();
	struct tsch_slotframe *tempHandle = tempHandle =
			tsch_schedule_add_slotframe(schedule.handle,
					schedule.slotframeLength);

	int i = 0;
	for (i = 0; i < MAX_NUM_LINKS; i++) {
		if (schedule.links[i].isActive == 1) {
			linkInfo_t temp = schedule.links[i];

			if (tsch_schedule_add_link(tempHandle, temp.link_options,
					temp.link_type, temp.addr, temp.timeslot,
					temp.channel_offset) == NULL) {
				break;
			}
		}
	}
	//tsch_schedule_print();
	return tempHandle->handle;

}

void clearSchedule() {

	int i;
	//remove everyhing except ADV slot
	for (i = 1; i < MAX_NUM_LINKS; i++) {
		tsch_schedule_remove_link(tsch_schedule_get_slotframe_by_handle(0),
				tsch_schedule_get_link_by_handle(i));
	}
}

struct scheduleUpdate_Packet createScheduleUpdate() {
	struct scheduleUpdate_Packet temp;
	temp.base.dst = tsch_broadcast_address;
	temp.base.src = linkaddr_node_addr;

	temp.schedule = 0;
	return temp;
}

int8_t getTier(){
	return tier;
}
void setTier(int8_t tempTier){
	tier = tempTier;
}

PROCESS_THREAD(dewi_scheduler_coord_process, ev, data) {
	PROCESS_EXITHANDLER()
	PROCESS_BEGIN()
		;

		/* Configure the user button */

		while (1) {
			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER) {

			}
		}

	PROCESS_END();
}

PROCESS_THREAD(dewi_scheduler_node_process, ev, data) {
struct scheduleUpdate_Packet scheduleUpdatePacket;
PROCESS_EXITHANDLER()
PROCESS_BEGIN()
	;

//etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);

	while (1) {
		PROCESS_YIELD()
		;
		if (ev == PROCESS_EVENT_TIMER) {

		}
	}

PROCESS_END();
}

int initScheduler() {
if (isCoord) {
#if DEBUG
printf("[SCHEDULER]: Start Scheduler for coordinator\n");
#endif
//if startup set CIDER active
setTier(-1);
setActiveSchedule(0);
process_start(&dewi_scheduler_coord_process, NULL);

} else {
#if DEBUG
printf("[SCHEDULER]: Start Scheduler for node\n");
#endif
setTier(-1);
process_start(&dewi_scheduler_node_process, NULL);
}
return 1;
}

void scheduleMessage(int timeslots, void* callback) {

printf("[SCHEDULER]: Schedule msg in: %d timeslots, callback: %u", timeslots,
	callback);

}

void scheduler_reset() {
etimer_stop(&scheduleUpdate);
isCoord = 0;
activeSchedule = -1;
clearSchedule();
process_exit(&dewi_scheduler_node_process);
process_exit(&dewi_scheduler_coord_process);
}
