/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */
#include "rll.h"

#include "contiki.h"
#include "neighTable.h"
#include "dev/leds.h"
#include "project-conf.h"

#if RLL_LOG_LEVEL >= 1
#define DEBUG DEBUG_PRINT
#else /* TSCH_LOG_LEVEL */
#define DEBUG DEBUG_NONE
#endif /* TSCH_LOG_LEVEL */
#include "net/net-debug.h"

/***************************************/
/***************************************/
/*				Macros 				   */
/***************************************/
/***************************************/

#define RLL_INTERVAL       (CLOCK_SECOND * 10)
/***************************************/
/***************************************/
/*				Variables			   */
/***************************************/
/***************************************/

uint8_t RLLSeqNo = 0;
uint8_t RLL_started = 0;
uint8_t RLL_sendCounter = 0;
uint8_t RLL_schedule_set = 0;
/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/
static struct etimer RLL_timer;
/***************************************/
/***************************************/
/*			Function Headers 		   */
/***************************************/
/***************************************/
void sendRLLPingMessage();
void sendToParent(struct RLL_PACKET packet);
void sendToChild(struct RLL_PACKET packet);
void sendToCS(struct RLL_PACKET packet);
void sendToCH(struct RLL_PACKET packet);

static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from);
/***************************************/
/***************************************/
/*			Channel Variables		   */
/***************************************/
/***************************************/

static struct broadcast_conn rll_broadcast;
static const struct broadcast_callbacks rll_broadcast_rx = { rll_packet_received };

/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS(dewi_rll_init_process, "DEWI Coluring PROCESS");
PROCESS(dewi_rll_process, "DEWI Coluring PROCESS");

PROCESS_THREAD(dewi_rll_init_process,ev,data)
{

	PROCESS_BEGIN()

		PRINTF("[RLL]: RLL open channel\n");
		broadcast_open(&rll_broadcast, BROADCAST_CHANNEL_RLL, &rll_broadcast_rx);
	PROCESS_END();
}

PROCESS_THREAD(dewi_rll_process, ev, data)
{
PROCESS_EXITHANDLER(netflood_close(&rll_broadcast));

PROCESS_BEGIN()
	;
	etimer_set(&RLL_timer, CLOCK_SECOND * 5);
	while (1)
	{
		PROCESS_YIELD()
		;
		if (ev == PROCESS_EVENT_TIMER)
		{
			PRINTF("[RLL]: RLL active\n");
			etimer_set(&RLL_timer, CLOCK_SECOND * 5);
		}

	}
PROCESS_END();
}

/***************************************/
/***************************************/
/*	     Function definitions		   */
/***************************************/
/***************************************/

void RLL_init()
{

process_start(&dewi_rll_init_process, NULL);
}

void RLL_start()
{

if (getTier() == 0 && RLL_started == 0)
{
process_start(&dewi_rll_process, NULL);
sendRLLPingMessage();
setActiveSchedule(2);
setActiveProtocol(2);
}
}
void RLL_notify()
{
PRINTF("[RLL]: RLL_notify()\n");
if (getColour() != -1 || getCIDERState() == 7)
{
if (RLL_started == 0)
{
	PRINTF("[RLL]: RLL_started == 0\n");PRINTF("[RLL]: RLL is active, set schedule\n");
	setActiveProtocol(2);
	process_start(&dewi_rll_process, NULL);
	RLL_started = 1;
	if (RLL_schedule_set == 0)
	{
		setRLLSchedule();
		RLL_schedule_set = 1;
	}

}
else if (RLL_started == 1)
{

	PRINTF("[RLL]: RLL_started == 1\n");
	if (RLL_schedule_set == 1)
	{
		tsch_schedule_print();
		RLL_schedule_set = 2;
	}
}
}
else
{
COLOURING_start();
}

}

static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{

struct RLL_PACKET *temp = packetbuf_dataptr();

switch (temp->subType)
{
case RLL_DATA:
	applicationDataCallback(temp->appData);
	break;
case RLL_PING:
	setActiveSchedule(2);
	sendRLLPingMessage();
	break;

}
}

void sendRLLDataMessage(struct APP_PACKET dataPacket)
{
PRINTF("[RLL]: sendRLLDataMessage, RLL_Started: %d\n",RLL_started);
if (RLL_started == 1)
{
struct RLL_PACKET RLLPacket;
RLLPacket.base.dst = linkaddr_null;
RLLPacket.base.src = linkaddr_node_addr;
RLLPacket.base.type = RLL;
RLLPacket.subType = RLL_DATA;
RLLPacket.appData = dataPacket;

if (getCIDERState() == 7)
{
	sendToCH(RLLPacket);
}
if (getCIDERState() == 5)
{
	if (getNumCS() != 0) sendToCS(RLLPacket);
	if (getTier() != 0) sendToParent(RLLPacket);
	if (getNumChildCH() != 0) sendToChild(RLLPacket);

}
}
}

void sendToParent(struct RLL_PACKET packet)
{
	PRINTF("[RLL]: sendToParent\n");
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
uint8_t timeslot = 0;
uint8_t baseTimeslot = 0;
uint8_t currentTimeslot = 0;
int8_t tx1 = -1, tx2 = -1;
	if (getTier() % 2 == 0)
	{
		tx1 = 2;
		tx2 = 6;

		if (temp->timeslot % 10 == tx1)
		{
			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx1;
			currentTimeslot = timeslot;
		}
		else if (temp->timeslot % 10 == tx2)
		{

			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx2;
			currentTimeslot = timeslot;
		}
		else
		{
			baseTimeslot = (temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			if(currentTimeslot < tx1)
				timeslot = baseTimeslot + tx1;
			else if(currentTimeslot < tx2)
				timeslot = baseTimeslot + tx2;
			else
				timeslot = baseTimeslot + 10 + tx1;

			if(timeslot>50)
				timeslot = tx1;
		}
		PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
	}
	else
	{
		tx1 = 4;
		tx2 = 8;
		if (temp->timeslot % 10 == tx1)
		{
			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx1;
			currentTimeslot = timeslot;
		}
		else if (temp->timeslot % 10 == tx2)
		{

			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx2;
			currentTimeslot = timeslot;
		}
		else
		{
			baseTimeslot = (temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			if(currentTimeslot < tx1)
				timeslot = baseTimeslot + tx1;
			else if(currentTimeslot < tx2)
				timeslot = baseTimeslot + tx2;
			else
				timeslot = baseTimeslot + 10 + tx1;

			if(timeslot > 50)
				timeslot = tx1;
		}
		PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
	}

	packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
	broadcast_send(&rll_broadcast);

}
void sendToChild(struct RLL_PACKET packet)
{
	PRINTF("[RLL]: sendToChild\n");
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
uint8_t timeslot = 0;
uint8_t baseTimeslot = 0;
uint8_t currentTimeslot = 0;
int8_t tx1 = -1, tx2 = -1;
	if (getTier() % 2 == 0)
	{
		tx1 = 5;
		tx2 = 9;

		if (temp->timeslot % 10 == tx1)
		{
			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx1;
			currentTimeslot = timeslot;
		}
		else if (temp->timeslot % 10 == tx2)
		{

			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx2;
			currentTimeslot = timeslot;
		}
		else
		{
			baseTimeslot = (temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			if(currentTimeslot < tx1)
				timeslot = baseTimeslot + tx1;
			else if(currentTimeslot < tx2)
				timeslot = baseTimeslot + tx2;
			else
				timeslot = baseTimeslot + 10 + tx1;

			if(timeslot > 50)
				timeslot = tx1;
		}
		PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
	}
	else
	{
		tx1 = 3;
		tx2 = 7;
		if (temp->timeslot % 10 == tx1)
		{
			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx1;
			currentTimeslot = timeslot;
		}
		else if (temp->timeslot % 10 == tx2)
		{

			timeslot = temp->timeslot;
			baseTimeslot = timeslot - tx2;
			currentTimeslot = timeslot;
		}
		else
		{
			baseTimeslot = (temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			if(currentTimeslot < tx1)
				timeslot = baseTimeslot + tx1;
			else if(currentTimeslot < tx2)
				timeslot = baseTimeslot + tx2;
			else
				timeslot = baseTimeslot + 10 + tx1;

			if(timeslot > 50)
				timeslot = tx1;
		}
		PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
	}
	packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
	broadcast_send(&rll_broadcast);
}
void sendToCS(struct RLL_PACKET packet)
{
	PRINTF("[RLL]: sendToCS\n");
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
uint8_t timeslot = 0;
uint8_t baseTimeslot = 0;
uint8_t currentTimeslot = 0;

if (temp->timeslot % 10 != 0)
{
baseTimeslot = (temp->timeslot / 10) * 10;
currentTimeslot = temp->timeslot - baseTimeslot;
PRINTF("[RLL]: base timeslot: %d, actual timeslot: %d\n",timeslot, temp->timeslot);
timeslot = baseTimeslot + 10;
if (timeslot > 50) timeslot = 10;
}
else if (temp->timeslot == 0)
{
timeslot = 10;
baseTimeslot = 0;
currentTimeslot = 0;
}
else
{
timeslot = temp->timeslot;
baseTimeslot = timeslot - 10;
currentTimeslot = timeslot;
} PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
broadcast_send(&rll_broadcast);
}

void sendToCH(struct RLL_PACKET packet)
{
	PRINTF("[RLL]: sendToCH\n");
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
uint8_t timeslot = 0;
uint8_t baseTimeslot = 0;
uint8_t currentTimeslot = 0;
if (temp->timeslot % 10 != 1)
{
baseTimeslot = (temp->timeslot / 10) * 10;
currentTimeslot = temp->timeslot - baseTimeslot;
PRINTF("[RLL]: base timeslot: %d, actual timeslot: %d\n",timeslot, temp->timeslot);
timeslot = baseTimeslot + 1;
if (timeslot > 50) timeslot = 1;
}
else if (temp->timeslot == 1)
{
timeslot = 1;
baseTimeslot = 0;
currentTimeslot = 1;
}
else
{
timeslot = temp->timeslot;
baseTimeslot = timeslot - 1;
currentTimeslot = timeslot;
}
 //Here insert Send first time

PRINTF("[RLL]: send Parent CH, I'm CS: base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
broadcast_send(&rll_broadcast);

}

void RLL_reset()
{
process_exit(&dewi_rll_process);
RLL_started = 0;
RLL_sendCounter = 0;
RLL_schedule_set = 0;
etimer_stop(&RLL_timer);
}

void sendRLLPingMessage()
{
if (RLL_sendCounter < 2)
{
struct RLL_PACKET RLLPacket;
RLLPacket.base.dst = linkaddr_null;
RLLPacket.base.src = linkaddr_node_addr;
RLLPacket.base.type = RLL;
RLLPacket.subType = RLL_PING;
RLL_sendCounter++;
packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));

packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);

broadcast_send(&rll_broadcast);
}
}
