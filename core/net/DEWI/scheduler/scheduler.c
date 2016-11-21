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
uint8_t isCoord = 0;
int8_t activeSchedule = -1;

int8_t tier = -1;

void setActiveSchedule(uint8_t schedule)
{
	setActiveProtocol(schedule);
	if(getCIDERState() != 5 && getCIDERState() != 7 )
		schedule = 0;
	printf("[SCHEDULER]: activeSchedule %d, schedule %d\n", activeSchedule, schedule);

	if (activeSchedule < schedule)
	{

		printf("[SCHEDULER]: new schedule received, Schedule %u is active now\n", schedule);

		activeSchedule = schedule;
		switch (activeSchedule)
		{
			case 0: //do stuff for CIDER;

				CIDER_notify();
				break;
			case 1:

				break;
			case 2: //todo: stuff for RLL
				RLL_notify();

				break;
			default:
				//just do nothing
				break;
		}

	}
}

void setCoord(uint8_t isCoordinator)
{
	isCoord = isCoordinator;
}

uint8_t getCoord()
{
	return isCoord;
}

uint8_t getActiveSchedule()
{
	return (uint8_t) activeSchedule;
}

uint16_t setSchedule()
{
	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(0x00);

	uint8_t i = 1;
	for (i = 1; i < MAX_NUM_LINKS; i++)
	{

		if (tsch_schedule_add_link(tempHandle,
		LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING,
				LINK_TYPE_NORMAL, &tsch_broadcast_address, i, 0) == NULL)
		{
			break;
		}

	}
	//tsch_schedule_print();
	return tempHandle->handle;

}
uint16_t setCoordSchedule()
{
	struct tsch_slotframe *tempHandle = tsch_schedule_add_slotframe(0x00, 51);

	uint8_t i = 1;
	for (i = 0; i < MAX_NUM_LINKS; i++)
	{
		if (i == 0)
		{
			if (tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING,
					LINK_TYPE_ADVERTISING, &tsch_broadcast_address, i, 0) == NULL)
			{
				break;
			}
		}

		else
		{
			if (tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING,
					LINK_TYPE_NORMAL, &tsch_broadcast_address, i, 0) == NULL)
			{
				break;
			}
		}

	}
	//tsch_schedule_print();
	return tempHandle->handle;

}

void setRLLNode()
{
	uint8_t i = 1;
	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(0x00);
	for (i = 1; i < tempHandle->size.val; i++)
	{
		if (i % 10 != 0 && i % 10 != 1)
		{
			tsch_schedule_remove_link_by_timeslot(tempHandle, i);
		}
		if (i % 10 == 1)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
		}
		if (i % 10 == 0)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
		}
	}

}
void setRLLCH()
{
	uint8_t i = 1;
	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(0x00);
	int8_t colourParent = getColourParent(getParentStatus());
	for (i = 1; i < tempHandle->size.val; i++)
	{
		if (i % 10 == 1)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());

		}
		else if (i % 10 == 2)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
		}
		else if (i % 10 == 3)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
		}
		else if (i % 10 == 4)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
		}
		else if (i % 10 == 5)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
		}
		else if (i % 10 == 6)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
		}
		else if (i % 10 == 7)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
		}
		else if (i % 10 == 8)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
		}
		else if (i % 10 == 9)
		{
			if (getTier() % 2 == 0)
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
			}
			else
			{
				tsch_schedule_add_link(tempHandle,
				LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, colourParent);
			}
		}
		else if (i % 10 == 0)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());

		}
	}

}
void setRLLTier0()
{
	uint8_t i = 1;
	struct tsch_slotframe *tempHandle = tsch_schedule_get_slotframe_by_handle(0x00);
	int8_t colourParent = getColourParent(getParentStatus());
	for (i = 1; i < tempHandle->size.val; i++)
	{
		if (i % 10 == 1)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());

		}
		else if (i % 10 == 2 || i % 10 == 3)
		{
			tsch_schedule_remove_link_by_timeslot(tempHandle, i);
		}
		else if (i % 10 == 4)
		{

			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());

		}
		else if (i % 10 == 5)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
		}
		else if (i % 10 == 6 || i % 10 == 7)
		{
			tsch_schedule_remove_link_by_timeslot(tempHandle, i);
		}
		else if (i % 10 == 8)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_RX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
		}
		else if (i % 10 == 9)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());
		}
		else if (i % 10 == 0)
		{
			tsch_schedule_add_link(tempHandle,
			LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, i, getColour());

		}
	}

}

void setRLLSchedule()
{

	if (getCIDERState() == 7)
	{
		setRLLNode();
	}
	else if (getCIDERState() == 5 && getTier() == 0)
	{
		setRLLTier0();
	}
	else if (getCIDERState() == 5)
	{
		setRLLCH();
	}

}

void clearSchedule()
{

	uint8_t i;
	//remove everyhing except ADV slot
	for (i = 1; i < MAX_NUM_LINKS; i++)
	{
		tsch_schedule_remove_link(tsch_schedule_get_slotframe_by_handle(0),
				tsch_schedule_get_link_by_handle(i));
	}
}

uint8_t initScheduler()
{

	if (isCoord)
	{
#if DEBUG
		printf("[SCHEDULER]: Start Scheduler for coordinator\n");
#endif
//if startup set CIDER active
		setTier(-1);
		setActiveSchedule(0);
		setCoordSchedule();
	}
	else
	{
#if DEBUG
		printf("[SCHEDULER]: Start Scheduler for node\n");
#endif

		setSchedule();
		setTier(-1);
	}

	COLOURING_init();
	RLL_init();
	return 1;
}

void scheduler_reset()
{
	isCoord = 0;
	activeSchedule = -1;
	clearSchedule();
}

