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
uint8_t lastRxSeqNo = 0;
uint8_t RLL_started = 0;
uint8_t RLL_sendCounter = 0;
uint8_t RLL_schedule_set = 0;

struct RLL_PACKET RLLPacket;
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
			if (RLL_schedule_set == 0)
			{
				setRLLSchedule();
				RLL_schedule_set = 1;
			}

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
else if (getCIDERState() == 5 && RLL_started == 0)
{
	process_start(&dewi_rll_process, NULL);
	RLL_started = 1;
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

int8_t direction = -2;
switch (temp->subType)
{
case RLL_DATA:
	if (temp->seqNo != lastRxSeqNo)
	{
		if (getCIDERState() == 5)
		{
			struct RLL_PACKET forwardPKT;
			forwardPKT.base.src = linkaddr_node_addr;
			forwardPKT.base.dst = temp->base.dst;
			forwardPKT.base.type = temp->base.type;
			forwardPKT.appData = temp->appData;
			forwardPKT.seqNo = temp->seqNo;
			forwardPKT.subType = temp->subType;
			lastRxSeqNo = temp->seqNo ;
			PRINTF("[RLL]: received RLL Data MSG at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);

			direction = getReceiveDirection(temp->base.src);
			switch (direction)
			{
				case -1:
					PRINTF("[RLL]: received from childCH, forward at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);

					if (getTier() != 0) sendToParent(forwardPKT);
					if (getNumChildCH() > 1) sendToChild(forwardPKT);
					if (getNumCS() != 0) sendToCS(forwardPKT);
					break;
				case 0:
					PRINTF("[RLL]: received from CS, forward at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);
					if (getTier() != 0) sendToParent(forwardPKT);
					if (getNumChildCH() != 0) sendToChild(forwardPKT);
					if (getNumCS() > 1) sendToCS(forwardPKT);
					break;
				case 1:
					PRINTF("[RLL]: received from Parent, forward at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);
					if (getNumChildCH() != 0) sendToChild(forwardPKT);
					if (getNumCS() != 0) sendToCS(forwardPKT);
					break;
			}
		}
		applicationDataCallback(temp->appData);
	}
	else
	{
		PRINTF("[RLL]: Packet received, but outdated. Current SeqNo: %d, received SeqNo: %d\n",RLLSeqNo,temp->seqNo);
	}
	break;
case RLL_PING:
	setActiveSchedule(2);
	sendRLLPingMessage();
	break;

}
}

void sendRLLDataMessage(struct APP_PACKET dataPacket)
{
PRINTF("[RLL]: sendRLLDataMessage, RLL_Started: %d, msg received from APP at: asn-%x.%lx\n",RLL_started,current_asn.ms1b,current_asn.ls4b);
if (RLL_started == 1)
{
RLLPacket.base.dst = linkaddr_null;
RLLPacket.base.src = linkaddr_node_addr;
RLLPacket.base.type = RLL;
RLLPacket.subType = RLL_DATA;
RLLPacket.appData = dataPacket;
RLLPacket.seqNo = RLLSeqNo++;
lastRxSeqNo = RLLSeqNo - 1;
if (getCIDERState() == 7)
{
	sendToCH(RLLPacket);
}
if (getCIDERState() == 5)
{
	if (getTier() != 0) sendToParent(RLLPacket);
	if (getNumChildCH() != 0) sendToChild(RLLPacket);
	if (getNumCS() != 0) sendToCS(RLLPacket);

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
uint8_t currentCalcTimeslot;
if (getTier() % 2 == 0)
{
tx1 = 2;
tx2 = 6;

currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
if (currentCalcTimeslot == tx1)
{
	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx1;
	currentTimeslot = timeslot;
}
else if (currentCalcTimeslot == tx2)
{

	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx2;
	currentTimeslot = timeslot;
}
else
{
	baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
	currentTimeslot = temp->timeslot - baseTimeslot;
	if (currentTimeslot < tx1)
		timeslot = baseTimeslot + tx1;
	else if (currentTimeslot < tx2)
		timeslot = baseTimeslot + tx2;
	else timeslot = baseTimeslot + 10 + tx1;

	if (timeslot > 50) timeslot = tx1;
}PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
}
else
{
tx1 = 4;
tx2 = 8;
currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
if (currentCalcTimeslot == tx1)
{
	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx1;
	currentTimeslot = timeslot;
}
else if (currentCalcTimeslot == tx2)
{

	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx2;
	currentTimeslot = timeslot;
}
else
{
	baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
	currentTimeslot = temp->timeslot - baseTimeslot;
	if (currentTimeslot < tx1)
		timeslot = baseTimeslot + tx1;
	else if (currentTimeslot < tx2)
		timeslot = baseTimeslot + tx2;
	else timeslot = baseTimeslot + 10 + tx1;

	if (timeslot > 50) timeslot = tx1;
}PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
}

packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
broadcast_send(&rll_broadcast);

}
void sendToChild(struct RLL_PACKET packet)
{
PRINTF("[RLL]: sendToChild,  msg received sent at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
uint8_t timeslot = 0;
uint8_t baseTimeslot = 0;
uint8_t currentTimeslot = 0;
int8_t tx1 = -1, tx2 = -1;
uint8_t currentCalcTimeslot;
if (getTier() % 2 == 0)
{
tx1 = 5;
tx2 = 9;
currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
if (currentCalcTimeslot == tx1)
{
	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx1;
	currentTimeslot = timeslot;
}
else if (currentCalcTimeslot == tx2)
{

	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx2;
	currentTimeslot = timeslot;
}
else
{
	baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
	currentTimeslot = temp->timeslot - baseTimeslot;
	if (currentTimeslot < tx1)
		timeslot = baseTimeslot + tx1;
	else if (currentTimeslot < tx2)
		timeslot = baseTimeslot + tx2;
	else timeslot = baseTimeslot + 10 + tx1;

	if (timeslot > 50) timeslot = tx1;
}PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
}
else
{

currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
tx1 = 3;
tx2 = 7;
if (currentCalcTimeslot == tx1)
{
	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx1;
	currentTimeslot = timeslot;
}
else if (currentCalcTimeslot == tx2)
{

	timeslot = temp->timeslot;
	baseTimeslot = timeslot - tx2;
	currentTimeslot = timeslot;
}
else
{
	baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
	currentTimeslot = temp->timeslot - baseTimeslot;
	if (currentTimeslot < tx1)
		timeslot = baseTimeslot + tx1;
	else if (currentTimeslot < tx2)
		timeslot = baseTimeslot + tx2;
	else timeslot = baseTimeslot + 10 + tx1;

	if (timeslot > 50) timeslot = tx1;
}PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
}
packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
broadcast_send(&rll_broadcast);
}
void sendToCS(struct RLL_PACKET packet)
{
PRINTF("[RLL]: sendToCS,  msg received sent at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
uint8_t timeslot = 0;
uint8_t baseTimeslot = 0;
uint8_t currentTimeslot = 0;
uint8_t currentCalcTimeslot;
printf("[APP]: send toCS current timeslot: %d\n", temp->timeslot);
currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

if (currentCalcTimeslot != 0)
{
baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
currentTimeslot = temp->timeslot - baseTimeslot;
timeslot = baseTimeslot + 10;
if (timeslot > 50) timeslot = 10;
}
else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
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
}PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d\n",baseTimeslot,currentTimeslot, timeslot);
packetbuf_copyfrom(&packet, sizeof(struct RLL_PACKET));
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
broadcast_send(&rll_broadcast);
}

void sendToCH(struct RLL_PACKET packet)
{
PRINTF("[RLL]: sendToCH,  msg received sent at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);
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
