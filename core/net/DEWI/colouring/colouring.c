/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */
#include "colouring.h"

#include "contiki.h"
#include "neighTable.h"
#include "dev/leds.h"
#include "project-conf.h"

#if COLOURING_LOG_LEVEL >= 1
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

/***************************************/
/***************************************/
/*				Variables			   */
/***************************************/
/***************************************/
uint8_t COLOURING_started = 0;
uint8_t COLOURING_delayTimeslot = 0;
uint8_t COLOURING_sendCounter = 0;
uint8_t COLOURING_currentState = COLOUR_UPDATE;
clock_time_t COLOURING_interval = CLOCK_SECOND;
/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/
struct etimer colouringETimer;
struct ctimer sendCBTimer, releaseWaitCBTimer, completeWaitCBTimer;

/***************************************/
/***************************************/
/*			Function Headers 		   */
/***************************************/
/***************************************/
static void colouring_packet_received(struct broadcast_conn *c, const linkaddr_t *from);
uint8_t checkColour();
void COLOURING_updateState();
void COLOURING_startSendTimer();
void COLOURING_callbackSendTimer();
void COLOURING_callbackReleaseWaitTimer();
void COLOURING_callbackCompleteWaitTimer();

/***************************************/
/***************************************/
/*			Channel Variables		   */
/***************************************/
/***************************************/
static const struct broadcast_callbacks colouring_bc_rx = { colouring_packet_received };

static struct broadcast_conn colouring_bc;
/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS(dewi_coluring_process, "DEWI Coluring PROCESS");
PROCESS(dewi_coluring_start_process, "DEWI Coluring PROCESS");

void COLOURING_init()
{
	process_start(&dewi_coluring_start_process, NULL);
}

PROCESS_THREAD(dewi_coluring_start_process, ev, data)
{
	PROCESS_BEGIN()
		;
		PRINTF("[COLOURING]: Open channel\n");
		broadcast_open(&colouring_bc, BROADCAST_CHANNEL_COLOURING, &colouring_bc_rx);
	PROCESS_END();
}

PROCESS_THREAD(dewi_coluring_process, ev, data)
{
PROCESS_EXITHANDLER(broadcast_close(&colouring_bc))

PROCESS_BEGIN()
	;

	setActiveProtocol(1);
	setRandNumberStatus((uint16_t) random_rand());
	etimer_set(&colouringETimer, COLOURING_interval);
	while (1)
	{
		PROCESS_YIELD()
		;
		if (ev == PROCESS_EVENT_TIMER)
		{
			if (&colouringETimer == data)
			{
				if (ctimer_expired(&sendCBTimer) == 1)
				{
					COLOURING_updateState();

				}
				etimer_set(&colouringETimer, COLOURING_interval);

			}
		}

	}

PROCESS_END();
}

/***************************************/
/***************************************/
/*	     Function definitions		   */
/***************************************/
/***************************************/

void COLOURING_start()
{
if (COLOURING_started == 0)
{
process_start(&dewi_coluring_process, NULL);
PRINTF("[COLOURING]: Start Colouring\n");
COLOURING_started = 1;
}
}

static void colouring_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{
struct COLOURING_PACKET *temp = packetbuf_dataptr();
if (COLOURING_started == 0 && getCIDERState() == 5) COLOURING_start();

struct neighbour n;

initNeighbour(&n);

getNeighbour(from, &n);
switch (temp->subType)
{
case COLOUR_UPDATE:
	PRINTF("[COLOURING]: MSG received COLOUR_UPDATE from 0x%4x\n",from->u16);
	n.CHDegree = temp->args[0];
	n.UCDegree = temp->args[1];
	n.CDegree = temp->args[2];
	n.sDegree = temp->args[3];
	n.randNumber = temp->args[4];
	n.COLOURINGState = COLOUR_UPDATE;
	break;
case COLOUR_RELEASE:
	PRINTF("[COLOURING]: MSG received COLOUR_RELEASE from 0x%4x with colour: %d\n",from->u16,temp->args[0]);
	n.colour = temp->args[0];
	n.COLOURINGState = COLOUR_RELEASE;
	linkaddr_t parent = getParentStatus();
	if (linkaddr_cmp(&temp->base.src, &parent) == 1 && getCIDERState() == 7)
		setColour(temp->args[0]);
	break;
case COLOUR_COMPLETE:
	PRINTF("[COLOURING]: MSG received COLOUR_COMPLETE from 0x%4x\n",from->u16);
	if (COLOURING_currentState == COLOUR_WAIT_COMPLETE && checkColour() == 1)
		COLOURING_currentState = COLOUR_COMPLETE;
	else if (checkColour() == 0) COLOURING_currentState = COLOUR_UNCOMPLETE;
	break;
case COLOUR_UNCOMPLETE:
	PRINTF("[COLOURING]: MSG received COLOUR_UNCOMPLETE from 0x%4x\n",from->u16);
	ctimer_stop(&completeWaitCBTimer);
	COLOURING_currentState = COLOUR_WAIT_COMPLETE;
	break;

}

addNeighbour(&n);


}

uint8_t checkColour()
{

PRINTF("[COLOURING]: checkColour \n");
uint8_t getCo = validColour();
return getCo;
}
void COLOURING_updateState()
{
switch (COLOURING_currentState)
{
case COLOUR_UPDATE:
	PRINTF("[COLOURING]: Current State: COLOUR_UPDATE \n");
	if (COLOURING_sendCounter >= 2 && checkIfReadyForNextState(COLOURING_currentState) == 1 && getColour() == 0)
	{
		PRINTF("[COLOURING]: Change State to: COLOUR_RELEASE \n");
		COLOURING_currentState = COLOUR_RELEASE;

		COLOURING_sendCounter = 0;



	}
	else if(getColour() != 0)
		COLOURING_currentState = COLOUR_WAIT_COMPLETE;
	COLOURING_startSendTimer();
	break;
case COLOUR_RELEASE:
	PRINTF("[COLOURING]: Current State: COLOUR_RELEASE \n");
	setDVStatus(getDV());
	calcSDIs();
	setSDIStatus(
			(getDVStatus() + 1) * getSDegreeStatus() + getUCDegreeStatus() + getRandNumberStatus());

	PRINTF("[COLOURING]: getHSDIStatus() < getSDIStatus(): %d < %d \n",getHSDIStatus() , getSDIStatus());
	if (getHSDIStatus() < getSDIStatus())
	{

		COLOURING_startSendTimer();
	}
	else
	{
		if (ctimer_expired(&releaseWaitCBTimer))
		{
			ctimer_set(&releaseWaitCBTimer, CLOCK_SECOND * 5, COLOURING_callbackReleaseWaitTimer,
			NULL);
		}
	}

	if (COLOURING_sendCounter >= 2 && getColour() != 0)
	{
		COLOURING_currentState = COLOUR_WAIT_COMPLETE;



	}
	break;
case COLOUR_WAIT_COMPLETE:
	PRINTF("[COLOURING]: Current State: COLOUR_WAIT_COMPLETE \n");

	COLOURING_interval = CLOCK_SECOND * 5;
	uint8_t getCo = checkColour();
	if (getCo == 1 && getTier() == 0)
	{
		COLOURING_currentState = COLOUR_COMPLETE;
	}
	break;
case COLOUR_COMPLETE:
	PRINTF("[COLOURING]: Current State: COLOUR_COMPLETE \n");
	COLOURING_startSendTimer();
	break;
case COLOUR_UNCOMPLETE:
	PRINTF("[COLOURING]: Current State: COLOUR_UNCOMPLETE \n");
	COLOURING_startSendTimer();
	if (COLOURING_sendCounter >= 2)
	{
		COLOURING_currentState = COLOUR_UPDATE;
		setColour(0);
	}
	break;
case COLOUR_FINISHED:
	PRINTF("[COLOURING]: Current State: COLOUR_FINISHED \n");
	break;
}
setCOLOURINGState(COLOURING_currentState);
}
void COLOURING_startSendTimer()
{
/**************************************/
/**************************************/
/*Function to start the send callback
 *timer, first calculate the delay
 *when a message should be sent
 *with:
 *tsr: remaining timeslots
 *tsd: base delay in timeslots
 *total_delaySF: total delay in
 *slotframe durations
 *delayTimeslot: Timeslo in which the
 *message will be send after the delay*/
/**************************************/
/**************************************/

struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
struct tsch_slotframe *tempSF = tsch_schedule_get_slotframe_by_handle(temp->slotframe_handle);
uint16_t tsr = (tempSF->size.val - temp->timeslot); //remaining timeslots in this sf duration
uint16_t tsd = (linkaddr_node_addr.u16 & 0b0000111111111111) / 10;
uint16_t total_delaySF = ((tsd - tsr) / tempSF->size.val);
uint16_t total_delaySF_100 = (((float) (tsd - tsr) / (float) tempSF->size.val) * 100.0);
uint16_t total_delaySF_1001 = (float) total_delaySF * 100.0;
uint16_t fraction = (total_delaySF_100 - total_delaySF_1001);
COLOURING_delayTimeslot = ((float) fraction / 100.0) * (float) tempSF->size.val;
if (COLOURING_delayTimeslot == 0) //message can't be sent in Timeslot 0, slot 0 is exclusivly for beacon messages
COLOURING_delayTimeslot = 1;
uint16_t total_delay = total_delaySF * tempSF->size.val + tsr;

clock_time_t delay = ((total_delay / 100.0)) * CLOCK_SECOND;

ctimer_set(&sendCBTimer, delay, COLOURING_callbackSendTimer, NULL);
}
void COLOURING_callbackSendTimer()
{
struct COLOURING_PACKET COLOURINGPACKET;
uint8_t sendMSG = 0;
radio_value_t chan;
radio_result_t rv;
int *tempTX;
tempTX = 0;
COLOURINGPACKET.base.dst = tsch_broadcast_address;
COLOURINGPACKET.base.src = linkaddr_node_addr;
COLOURINGPACKET.base.type = COLOURING;
switch (COLOURING_currentState)
{
case COLOUR_UPDATE:
	PRINTF("[COLOURING]: Create COLOUR_UPDATE\n");
	setCHDegreeStatus(getCHDegree());
	setUCDegreeStatus(getUncoloredNodes());
	setCDegreeStatus(getColoredNodes());
	setSDegreeStatus(getSDegree());
	COLOURINGPACKET.subType = COLOUR_UPDATE;
	COLOURINGPACKET.args[0] = getCHDegreeStatus();
	COLOURINGPACKET.args[1] = getUCDegreeStatus();
	COLOURINGPACKET.args[2] = getCDegreeStatus();
	COLOURINGPACKET.args[3] = getSDegreeStatus();
	COLOURINGPACKET.args[4] = getRandNumberStatus();
	sendMSG = 1;
	break;
case COLOUR_RELEASE:
	PRINTF("[COLOURING]: Create COLOUR_RELEASE\n");
	if (getHSDIStatus() < getSDIStatus())
	{
		sendMSG = 1;
		if (getColour() == 0)
		{
			setColour(calcColour());

		}
	}

	COLOURINGPACKET.subType = COLOUR_RELEASE;
	COLOURINGPACKET.args[0] = getColour();

	break;
case COLOUR_COMPLETE:

	PRINTF("[COLOURING]: Create COLOUR_COMPLETE\n");
	COLOURINGPACKET.subType = COLOUR_COMPLETE;
	if (ctimer_expired(&completeWaitCBTimer) == 1)
		ctimer_set(&completeWaitCBTimer, CLOCK_SECOND * 10, COLOURING_callbackCompleteWaitTimer,
				NULL);
	sendMSG = 1;
	break;
case COLOUR_UNCOMPLETE:

	PRINTF("[COLOURING]: Create COLOUR_UNCOMPLETE\n");
	COLOURINGPACKET.subType = COLOUR_UNCOMPLETE;
	sendMSG = 1;
	break;
case COLOUR_FINISHED:
	break;
}
if (sendMSG == 1)
{
PRINTF("[COLOURING]: MSG was send\n");
COLOURING_sendCounter++;
packetbuf_copyfrom(&COLOURINGPACKET, sizeof(struct COLOURING_PACKET));

packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, COLOURING_delayTimeslot);

broadcast_send(&colouring_bc);



}
}

void COLOURING_callbackReleaseWaitTimer()
{

setRandNumberStatus((uint16_t) random_rand());
COLOURING_currentState = COLOUR_UPDATE;
COLOURING_sendCounter = 0;

}
void COLOURING_callbackCompleteWaitTimer()
{
COLOURING_currentState = COLOUR_FINISHED;
process_exit(&dewi_coluring_process);
printNodeStatus();
printTable();

PRINTF("[COLOURING]: Start Next protocol\n");

}

void COLOURING_Reset(){
	COLOURING_started = 0;
	COLOURING_delayTimeslot = 0;
	COLOURING_sendCounter = 0;
	COLOURING_currentState = COLOUR_UPDATE;
	COLOURING_interval = CLOCK_SECOND;
}
