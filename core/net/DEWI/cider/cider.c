/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */
#include "contiki.h"
#include "cider.h"
#include "neighTable.h"
#include "dev/leds.h"
#include "project-conf.h"

#if CIDER_LOG_LEVEL >= 1
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

#ifdef CONF_ND_min
float CIDER_ND_min = CONF_ND_min
#endif
#ifdef CONF_ND_max
float CIDER_ND_max = CONF_ND_max
#endif
#ifdef CONF_CD_min
float CIDER_CD_min = CONF_CD_min
#endif
#ifdef CONF_CD_max
float CIDER_CD_max = CONF_CD_max
#endif
#ifdef CONF_LP_min
float CIDER_LP_min = CONF_LP_min
#endif
#ifdef CONF_LP_max
float CIDER_LP_max = CONF_LP_max
#endif
#ifdef CONF_RSSI_min
float CIDER_RSSI_min = CONF_RSSI_min
#endif
#ifdef CONF_RSSI_max
float CIDER_RSSI_max = CONF_RSSI_max
#endif

#ifdef CONF_M1
float CIDER_M1 = CONF_M1
;
#endif

#ifdef CONF_M2
float CIDER_M2 = CONF_M2
;
#endif

#ifdef CONF_M3
float CIDER_M3 = CONF_M3
;
#endif

#ifdef CONF_M4
float CIDER_M4 = CONF_M4
;
#endif

/***************************************/
/***************************************/
/*				Variables			   */
/***************************************/
/***************************************/
int8_t sendCounter = 0, currentState = CIDER_PING;
float utility = 0;
uint8_t delayTimeslot = 0;
float avgRSSI = 0;
float neighDegree = 0;
float clusterDegree = 0;
float lpDegree = 0;
uint8_t isLPDevice = 0;
uint8_t CIDER_isActive = 0;
uint8_t isLPD = 0;
int started = 0;
uint8_t clusteringComplete = 0;
uint8_t clusteringUnComplete = 0;

linkaddr_t parent = { { 0, 0 } };
uint8_t csPingCounter = 0;
clock_time_t ciderInterval = (CLOCK_SECOND * 0.5);
clock_time_t ciderKAInterval = (CLOCK_SECOND * 30);

/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/
static struct etimer cider_timer, print_timer;
static struct ctimer send_timer, cider_competition_timer, ka_timer, completeTimer,
		completeWaitTimer;

/***************************************/
/***************************************/
/*			Function Headers 		   */
/***************************************/
/***************************************/
void startSendTimer();
void updateState();
void callbackSendTimer();
void callbackKATimer();
void calcUtility();
void printCIDERStatus();
float featureScale(float value, float min, float max);
uint8_t compareUtilities();
linkaddr_t checkForCSPingMember();
uint8_t checkCHinRange();
static void cider_packet_received(struct broadcast_conn *c, const linkaddr_t *from);
void callbackCompetitionTimer();
void callbackCompleteTimer();
void callbackCompleteWaitTimer();
int getTxPowerInt(uint8_t tx);
uint8_t getTxPower8bit(int tx);
void CIDER_createSchedule();
void updateStatusLED();

/***************************************/
/***************************************/
/*			Channel Variables		   */
/***************************************/
/***************************************/
static const struct broadcast_callbacks cider_bc_rx = { cider_packet_received };

static struct broadcast_conn cider_bc;
/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS(dewi_cider_process, "DEWI cider PROCESS");
PROCESS_THREAD(dewi_cider_process, ev, data)
{
	PROCESS_EXITHANDLER(broadcast_close(&cider_bc))
	broadcast_open(&cider_bc, BROADCAST_CHANNEL_CIDER, &cider_bc_rx);
	PROCESS_BEGIN()
		;

		etimer_set(&print_timer, CLOCK_SECOND * 10);
		while (1)
		{
			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER)
			{
				if (&cider_timer == data)
				{
					if (ctimer_expired(&send_timer) == 1)
					{
						updateState();
						etimer_set(&cider_timer, ciderInterval);
					}
					else
					{
						etimer_set(&cider_timer, ciderInterval);
					}
				}
				else if (&print_timer == data)
				{
					printCIDERStatus();
					etimer_set(&print_timer, CLOCK_SECOND * 10);
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

void startSendTimer()
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
delayTimeslot = ((float) fraction / 100.0) * (float) tempSF->size.val;
if (delayTimeslot == 0) //message can't be sent in Timeslot 0, slot 0 is exclusivly for beacon messages
	delayTimeslot = 1;
uint16_t total_delay = total_delaySF * tempSF->size.val + tsr;

clock_time_t delay = ((total_delay / 100.0)) * CLOCK_SECOND;

ctimer_set(&send_timer, delay, callbackSendTimer, NULL);

}

void updateState()
{
PRINTF("[CIDER]: Check CIDER state ASN:  \n");
uint8_t checkNeighboursState = checkIfReadyForNextState(currentState);
switch (currentState)
{
	case CIDER_PING:
		PRINTF("[CIDER]: Current State: Ping \n");

		updateStatusLED();
		if (sendCounter >= 2 && checkNeighboursState == 1)
		{
			if (isLPDevice == 1)
			{
				PRINTF("[CIDER]: Change State to: LP_PING \n");
				currentState = CIDER_LP_PING;
			}
			else
			{
				PRINTF("[CIDER]: Change State to: NEIGHBOUR_UPDATE \n");
				currentState = CIDER_NEIGHBOUR_UPDATE;
			}
			sendCounter = 0;
		}
		startSendTimer();
		break;

	case CIDER_NEIGHBOUR_UPDATE:
		PRINTF("[CIDER]: Current State: NEIGHBOUR_UPDATE \n");

		updateStatusLED();
		if (sendCounter >= 2 && checkNeighboursState == 1)
		{
			PRINTF("[CIDER]: Change State to: UTILITY_UPDATE \n");
			currentState = CIDER_UTILITY_UPDATE;
			sendCounter = 0;
		}
		startSendTimer();
		break;

	case CIDER_UTILITY_UPDATE:
		PRINTF("[CIDER]: Current State: UTILITY_UPDATE \n");
		calcUtility();

		updateStatusLED();
		if (sendCounter >= 4 && checkNeighboursState == 1)
		{

			if (compareUtilities() == 1)
			{
				PRINTF("[CIDER]: Change State to: CH_COMPETITION \n");
				currentState = CIDER_CH_COMPETITION;
			}
			else
			{
				PRINTF("[CIDER]: Change State to: CS_PING \n");
				currentState = CIDER_CS_PING;
			}
			sendCounter = 0;
		}
		startSendTimer();
		break;

	case CIDER_CH_COMPETITION:
		PRINTF("[CIDER]: Current State: CH_COMPETITION \n");
		PRINTF("[CIDER]: sendCounter: %u, checkNeighboursState: %u ctimer_expired: %u\n",sendCounter,checkNeighboursState,ctimer_expired(&cider_competition_timer));

		updateStatusLED();
		if (sendCounter >= 2 && checkNeighboursState == 1
				&& ctimer_expired(&cider_competition_timer) == 1)
		{
			PRINTF("[CIDER]: Change State to: CH \n");
			currentState = CIDER_CH;
			if (getTier() == -1) setTier(0);
			tsch_set_eb_period(TSCH_CONF_EB_PERIOD);
			sendCounter = 0;
		}
		startSendTimer();
		break;

	case CIDER_CH:
		PRINTF("[CIDER]: Current State: CH \n");
		updateStatusLED();

		PRINTF("[CIDER]: Check for Clustering Complete \n");
		if (clusteringComplete == 0)
		{
			PRINTF("[CIDER]: Clustering not Complete \n");
			PRINTF("[CIDER]: Check if I should start it\n");
			if (linkaddr_cmp(&linkaddr_null, &parent) == 1
					|| linkaddr_cmp(&linkaddr_node_addr, &parent) == 1)
			{
				PRINTF("[CIDER]: I should start it\n");
				PRINTF("[CIDER]: Check for unclustered nodes it\n");
				if (checkNodesUnclustered() == 1)
				{
					PRINTF("[CIDER]:no unclsutered nodes\n");
					PRINTF("[CIDER]: check for child ch\n");
					if (checkChildCH() == 1)
					{
						ctimer_set(&completeTimer, CLOCK_SECOND, callbackCompleteTimer, NULL);
					}
					else
					{
						ctimer_set(&completeWaitTimer, CLOCK_SECOND, callbackCompleteWaitTimer, NULL);
					}
				}
			}
		}


		if (sendCounter > 2) ciderInterval = CLOCK_SECOND * 5;
		startSendTimer();
		break;

	case CIDER_CS:
		PRINTF("[CIDER]: Current State: CS \n");
		updateStatusLED();
		if (sendCounter <= 2) startSendTimer();
		if (sendCounter > 2) ciderInterval = CLOCK_SECOND * 5;

		break;
	case CIDER_CS_PING:
		PRINTF("[CIDER]: Current State: CS_PING \n");
		if (checkCHinRange() == 0 && sendCounter >= 20)
		{
			if (compareUtilities() == 1)
			{
				PRINTF("[CIDER]: Change State to: CH_COMPETITION \n");
				currentState = CIDER_CH_COMPETITION;
			}
		}
		updateStatusLED();
		startSendTimer();
		break;

	case CIDER_CH_PROMOTE:
		PRINTF("[CIDER]: Current State: CH_PROMOTE \n");
		if (sendCounter >= 1)
		{
			PRINTF("[CIDER]: Change State to: CH \n");
			currentState = CIDER_CH;
			sendCounter = 0;
			csPingCounter = 0;
		}
		updateStatusLED();
		startSendTimer();
		break;

}

}

void callbackSendTimer()
{
struct CIDER_PACKET CIDERPacket;

radio_value_t chan;
radio_result_t rv;
int *tempTX;
tempTX = 0;
CIDERPacket.base.dst = tsch_broadcast_address;
CIDERPacket.base.src = linkaddr_node_addr;
CIDERPacket.base.type = CIDER;

switch (currentState)
{
	case CIDER_PING: //PING message

		PRINTF("[CIDER]:Create CIDER PING MESSAGE \n");
		CIDERPacket.subType = CIDER_PING;
		rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
		CIDERPacket.args[0] = getTxPower8bit(chan);
		CIDERPacket.args[1] = isLPD;
		break;
	case CIDER_NEIGHBOUR_UPDATE:
		PRINTF("[CIDER]:Create CIDER NEIGHBOUR MESSAGE \n");
		CIDERPacket.subType = CIDER_NEIGHBOUR_UPDATE;
		CIDERPacket.args[0] = getNumNeighbours();
		CIDERPacket.args[1] = getNumLPDevices();
		CIDERPacket.args[2] = getNumCluster();
		break;
	case CIDER_UTILITY_UPDATE:

		PRINTF("[CIDER]:Create CIDER Utility Update \n");
		CIDERPacket.subType = CIDER_UTILITY_UPDATE;
		CIDERPacket.args[0] = (uint16_t) (utility * 1000);
		break;
	case CIDER_CH_COMPETITION:
		PRINTF("[CIDER]:Create CH_COMPETITION message \n");
		CIDERPacket.subType = CIDER_CH_COMPETITION;

		CIDERPacket.args[0] = (uint16_t) (utility * 1000);
		break;
	case CIDER_CH:
		PRINTF("[CIDER]:Create CH MESSAGE \n");
		CIDERPacket.subType = CIDER_CH;
		CIDERPacket.parent = parent;
		CIDERPacket.args[0] = getTier();
		int i;
		for (i = 1; i < 45; i++)
		{
			CIDERPacket.args[i] = 0;
		}
		updateNeighListCS(&CIDERPacket.args, 45, linkaddr_node_addr);
		break;
	case CIDER_LP_PING:
		PRINTF("[CIDER]:Create CIDER LP PING MESSAGE \n");
		CIDERPacket.subType = CIDER_LP_PING;
		rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
		CIDERPacket.args[0] = getTxPower8bit(chan);
		CIDERPacket.args[1] = isLPD;
		break;
	case CIDER_CS_PING:
		PRINTF("[CIDER]:Create CIDER CS PING MESSAGE \n");
		CIDERPacket.subType = CIDER_CS_PING;
		CIDERPacket.args[0] = (uint16_t) (utility * 1000);
		CIDERPacket.args[1] = getNumNeighbours();
		CIDERPacket.args[2] = getNumLPDevices();
		CIDERPacket.args[3] = getNumCluster();
		break;
	case CIDER_CS:
		PRINTF("[CIDER]:Create CIDER CS MESSAGE \n");
		CIDERPacket.subType = CIDER_CS;
		CIDERPacket.args[0] = (uint16_t) (utility * 1000);
		CIDERPacket.args[1] = getNumNeighbours();
		CIDERPacket.args[2] = getNumLPDevices();
		CIDERPacket.args[3] = getNumCluster();
		CIDERPacket.parent = parent;
		break;
	case CIDER_CH_PROMOTE:

		PRINTF("[CIDER]:Create CH_Promote MESSAGE \n");
		CIDERPacket.base.dst = getChildCHAddress(CIDER_CS_PING);
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = CIDER_CH_PROMOTE;
		CIDERPacket.parent = linkaddr_node_addr;
		CIDERPacket.args[0] = getTier();

		PRINTF("[CIDER]: Dst: 0x%4x\n", CIDERPacket.base.dst.u16);
		break;

	default:
		//do nothing
		break;
}

sendCounter = sendCounter + 1;
packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

if (currentState == CIDER_CH)
{
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);
}
else
{
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, delayTimeslot);
}
broadcast_send(&cider_bc);
}

static void cider_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{

struct CIDER_PACKET *temp = packetbuf_dataptr();
if (started == 0) CIDER_start();
int16_t tempRSSI;
uint8_t newNeigh;
radio_result_t rv = (int8_t) NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &tempRSSI);

struct neighbour n;

initNeighbour(&n);

getNeighbour(from, &n);

switch (temp->subType)
{
	case CIDER_PING:
		PRINTF("[CIDER]: Ping Message received from 0x%4x \n", from->u16);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.txPW = getTxPowerInt(temp->args[0]);
		n.isLPD = temp->args[1];
		n.state = CIDER_PING;
		newNeigh = addNeighbour(&n);
		break;
	case CIDER_NEIGHBOUR_UPDATE:
		PRINTF("[CIDER]: Neighbour Update received from 0x%4x \n", from->u16);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.nodeDegree = temp->args[0];
		n.lpDegree = temp->args[1];
		n.clusterDegree = temp->args[2];
		n.state = CIDER_NEIGHBOUR_UPDATE;
		newNeigh = addNeighbour(&n);
		break;
	case CIDER_UTILITY_UPDATE:
		PRINTF("[CIDER]: Utility Update received from 0x%4x \n", from->u16);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.utility = (float) temp->args[0] / 1000.0;
		n.state = CIDER_UTILITY_UPDATE;
		newNeigh = addNeighbour(&n);
		break;
	case CIDER_CS:
		PRINTF("[CIDER]: CS message received from 0x%4x \n", from->u16);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.utility = (float) temp->args[0] / 1000.0;
		n.nodeDegree = temp->args[1];
		n.lpDegree = temp->args[2];
		n.clusterDegree = temp->args[3];
		n.parent = temp->parent;
		n.state = CIDER_CS;
		newNeigh = addNeighbour(&n);
		break;
	case CIDER_CH_COMPETITION:
		PRINTF("[CIDER]: CH Competition received from 0x%4x \n", from->u16);
		if (currentState == CIDER_CH_COMPETITION && ctimer_expired(&cider_competition_timer))
		{
			clock_time_t delayTime = random_rand() % (5 + 1);
			PRINTF("[CIDER]: Delay Competition message by %d s \n", delayTime);
			ctimer_set(&cider_competition_timer, delayTime, callbackCompetitionTimer, NULL);
		}

		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.utility = (float) temp->args[0] / 1000.0;
		n.state = CIDER_CH_COMPETITION;
		newNeigh = addNeighbour(&n);
		break;
	case CIDER_CH:
		PRINTF("[CIDER]: CH message received from 0x%4x \n", from->u16);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.state = CIDER_CH;
		n.myCS = 0;
		n.myCH = 0;
		n.parent = temp->parent;
		n.tier = temp->args[0];
		int i;
		int isCH = 0;
		for (i = 1; i < 45; i++)
		{
			if (temp->args[i] == linkaddr_node_addr.u16)
			{
				isCH = 1;
			}
			updateNeighboursCH(temp->args[i], temp->base.src, temp->args[0]);
		}
		if (currentState != CIDER_CS && currentState != CIDER_CH && isCH == 1)
		{
			ctimer_stop(&cider_competition_timer);
			n.myCH = 1;
			setTier(temp->args[0]);
			parent = temp->base.src;
			currentState = CIDER_CS;

		}
		if (linkaddr_cmp(&parent, &temp->base.src) == 1)
		{
			ctimer_stop(&ka_timer);
			ctimer_set(&ka_timer, ciderKAInterval, callbackKATimer, NULL);
		}
		newNeigh = addNeighbour(&n);
		break;
	case CIDER_CS_PING:
		PRINTF("[CIDER]: CS_PING received from 0x%4x \n", from->u16);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.utility = (float) temp->args[0] / 1000.0;
		n.nodeDegree = temp->args[1];
		n.lpDegree = temp->args[2];
		n.clusterDegree = temp->args[3];
		n.state = CIDER_CS_PING;
		if (currentState == CIDER_CH && sendCounter >= 2)
		{
			n.msgCounter = n.msgCounter + 1;
		}

		newNeigh = addNeighbour(&n);
		if (checkForPromotion(CIDER_CS_PING) == 1)
		{
			currentState = CIDER_CH_PROMOTE;
			sendCounter = 0;
		}
		break;

	case CIDER_CH_PROMOTE:
		PRINTF("[CIDER]: CH_PROMOTE received from 0x%4x \n", from->u16);
		PRINTF("[CIDER]: CH_PROMOTE for 0x%4x, my address: 0x%4x \n", temp->base.dst.u16,linkaddr_node_addr.u16);
		if (temp->base.dst.u16 == linkaddr_node_addr.u16)
		{
			currentState = CIDER_CH_COMPETITION;

			setTier(temp->args[0] + 1);
			sendCounter = 0;
			parent = temp->parent;
			n.addr = temp->base.src;
			n.last_rssi = tempRSSI;
			n.last_asn = tsch_get_current_asn();
			n.state = CIDER_CH;
			n.myCH = 1;
			n.tier = temp->args[0];
			newNeigh = addNeighbour(&n);
		}
		else
		{
			PRINTF("[CIDER]: CH_PROMOTE not meant for me \n");
		}

		if (linkaddr_cmp(&parent, &temp->base.src) == 1)
		{
			ctimer_stop(&ka_timer);
			ctimer_set(&ka_timer, ciderKAInterval, callbackKATimer, NULL);
		}

		break;
	case CIDER_COMPLETE:
		if (currentState == CIDER_CH)
		{
			PRINTF("[CIDER]: CIDER_COMPLETE received from 0x%4x \n", from->u16);
			if (ctimer_expired(&completeWaitTimer) == 1)
			{
				clusteringUnComplete = 0;
				if (checkNodesUnclustered() == 1)
				{
					if (checkChildCH() == 1)
					{
						PRINTF("[CIDER]: CIDER_COMPLETE received; there are Child CH \n");
						ctimer_stop(&completeWaitTimer);
						ctimer_set(&completeWaitTimer, CLOCK_SECOND * 5, callbackCompleteWaitTimer,
						NULL);
					}
					else
					{
						PRINTF("[CIDER]: CIDER_COMPLETE received; there no Child CH, clustering complete \n");
						clusteringComplete = 1;
					}
				}
				else
				{
					ctimer_stop(&completeWaitTimer);
					PRINTF("[CIDER]: CIDER_COMPLETE received; there are unclustered nodes, create uncomplete message \n");

					if (clusteringUnComplete == 0)
					{
						struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0,
						NULL);
						struct CIDER_PACKET CIDERPacket;
						CIDERPacket.base.dst = tsch_broadcast_address;
						CIDERPacket.base.src = linkaddr_node_addr;
						CIDERPacket.base.type = CIDER;
						PRINTF("[CIDER]:Create UNCOMPLETE MESSAGE \n");
						CIDERPacket.subType = CIDER_UNCOMPLETE;
						clusteringUnComplete = 1;
						packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

						packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
						packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, temp->timeslot);

						broadcast_send(&cider_bc);
					}
				}
			}
		}
		break;
	case CIDER_UNCOMPLETE:
		if (currentState == CIDER_CH)
		{
			PRINTF("[CIDER]: CIDER_UNCOMPLETE received from 0x%4x \n", from->u16);
			ctimer_stop(&completeWaitTimer);
			ctimer_stop(&completeTimer);
			if (clusteringUnComplete == 0 && linkaddr_cmp(&linkaddr_null, &parent) != 1
					&& linkaddr_cmp(&linkaddr_node_addr, &parent) != 1)
			{
				struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0,
				NULL);
				struct CIDER_PACKET CIDERPacket;
				CIDERPacket.base.dst = tsch_broadcast_address;
				CIDERPacket.base.src = linkaddr_node_addr;
				CIDERPacket.base.type = CIDER;
				PRINTF("[CIDER]:Create UNCOMPLETE MESSAGE \n");
				CIDERPacket.subType = CIDER_UNCOMPLETE;
				clusteringUnComplete = 1;
				packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, temp->timeslot);

				broadcast_send(&cider_bc);
			}
		}

		break;

}
}


int CIDER_notify()
{
//todo: Implement check if RLL;
if (CIDER_isActive == 0)
{
	CIDER_isActive = 1;
	PRINTF("[CIDER]: Start Cider \n");
	process_start(&dewi_cider_process, NULL);

}
else
{
	CIDER_isActive = 0;
	process_exit(&dewi_cider_process);
}

CIDER_createSchedule();

return 1;
}

void CIDER_start()
{
started = 1;
PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
	;
	etimer_set(&cider_timer, ciderInterval);
	PROCESS_CONTEXT_END(&dewi_cider_process);

}

int CIDER_getIsActive()
{
return CIDER_isActive;
}

void CIDER_setLPD(uint8_t is)
{
isLPD = is;
}
uint8_t CIDER_getLPD()
{
return isLPD;
}

void CIDER_createSchedule()
{
if (CIDER_isActive == 1)
{
	ScheduleInfo_t temp;
	temp.handle = 0x00;
	temp.slotframeLength = 51;
	int i;
	PRINTF("[CIDER]: Create CIDER schedule \n");
	for (i = 0; i < temp.slotframeLength; i++)
	{
		temp.links[i].addr = &tsch_broadcast_address;
		temp.links[i].channel_offset = 0;
		temp.links[i].isActive = 1;

		if (i == 0)
		{
			temp.links[i].link_type = LINK_TYPE_ADVERTISING;
			temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED
					| LINK_OPTION_TIME_KEEPING;
		}
		else
		{
			temp.links[i].link_type = LINK_TYPE_NORMAL;
			temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED
					| LINK_OPTION_TIME_KEEPING;
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

void calcUtility()
{
avgRSSI = getAvgRSSI();
neighDegree = getNumNeighbours();
clusterDegree = getNumCluster();
lpDegree = getNumLPDevices();
if (neighDegree == 0.0 || clusterDegree == 0.0)
{
	utility = 0;
	return;
}
neighDegree = featureScale((float) neighDegree, CIDER_ND_min, CIDER_ND_max);
clusterDegree = featureScale((float) clusterDegree, CIDER_CD_min, CIDER_CD_max);

if (lpDegree == 0)
{

	avgRSSI = featureScale((float) avgRSSI, CIDER_RSSI_min, CIDER_RSSI_max);
	utility = (CIDER_M1 * (float) neighDegree + (CIDER_M2 + CIDER_M3) * (float) clusterDegree
			+ CIDER_M4 * avgRSSI);
}
else
{
	lpDegree = featureScale((float) lpDegree, CIDER_LP_min, CIDER_LP_max);
	utility = (CIDER_M1 * (float) neighDegree + CIDER_M2 * (float) clusterDegree
			+ CIDER_M3 * (float) lpDegree + CIDER_M4 * avgRSSI);
}
}

float featureScale(float value, float min, float max)
{
float featuredValue = 0;
float numerator = value - (min);
float denominator = max - (min);
featuredValue = numerator / denominator;

return featuredValue;
}

uint8_t compareUtilities()
{
float highestNeighbourUtility = (float) getHighestUtility();
if (utility >= highestNeighbourUtility)
	return 1;
else return 0;
}

linkaddr_t checkForCSPingMember()
{
linkaddr_t returnAddr = getChildCHAddress(CIDER_CS_PING);
PRINTF("[CIDER]: Dst: 0x%4x\n", returnAddr.u16);
return returnAddr;
}

uint8_t checkCHinRange()
{
if (checkIfCHinNetwork(CIDER_CH) == 1)
	return 1;
else return 0;
}

uint8_t getTxPower8bit(int tx)
{

uint8_t tx8 = 0x00;
switch (tx)
{
	case 7:
		tx8 = 0xFF;
		break;
	case 5:
		tx8 = 0xed;
		break;
	case 3:
		tx8 = 0xd5;
		break;
	case 1:
		tx8 = 0xc5;
		break;
	case 0:
		tx8 = 0xb6;
		break;
	case -1:
		tx8 = 0xb0;
		break;
	case -3:
		tx8 = 0xa1;
		break;
	case -5:
		tx8 = 0x91;
		break;
	case -7:
		tx8 = 0x88;
		break;
	case -9:
		tx8 = 0x72;
		break;
	case -11:
		tx8 = 0x62;
		break;
	case -13:
		tx8 = 0x58;
		break;
	case -15:
		tx8 = 0x42;
		break;
	case -24:
		tx8 = 0x00;
		break;
}
return tx8;
}

int getTxPowerInt(uint8_t tx)
{
int txInt = -24;
switch (tx)
{
	case 0xFF:
		txInt = 7;
		break;
	case 0xed:
		txInt = 5;
		break;
	case 3:
		txInt = 0xd5;
		break;
	case 0xc5:
		txInt = 1;
		break;
	case 0xb6:
		txInt = 0;
		break;
	case 0xb0:
		txInt = -1;
		break;
	case 0xa1:
		txInt = -3;
		break;
	case 0x91:
		txInt = -5;
		break;
	case 0x88:
		txInt = -7;
		break;
	case 0x72:
		txInt = -9;
		break;
	case 0x62:
		txInt = -11;
		break;
	case 0x58:
		txInt = -13;
		break;
	case 0x42:
		txInt = -15;
		break;
	case 0x0:
		txInt = -24;
		break;
}
return txInt;
}

void updateStatusLED()
{
switch (currentState)
{
	case CIDER_PING: //PING message
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		break;
	case CIDER_NEIGHBOUR_UPDATE:
		leds_off(LEDS_ALL);
		leds_on(LEDS_YELLOW);
		break;
	case CIDER_UTILITY_UPDATE:
		leds_off(LEDS_ALL);
		leds_on(LEDS_BLUE);
		break;
	case CIDER_CH_COMPETITION:
		leds_off(LEDS_ALL);
		leds_on(LEDS_WHITE);
		break;
	case CIDER_LP_PING:
		leds_off(LEDS_ALL);
		leds_on(LEDS_PURPLE);
		break;
	case CIDER_CS:
		leds_off(LEDS_ALL);
		leds_on(LEDS_YELLOW);
		break;
	case CIDER_CH:
		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		break;
	case CIDER_CS_PING:
		leds_off(LEDS_ALL);
		leds_on(LEDS_LIGHT_BLUE);
		break;
	case CIDER_CH_PROMOTE:
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		break;

	default:
		//do nothing
		break;
}
}
void printCIDERStatus()
{

PRINTF("******** CIDER STATUS *********\n");PRINTF("sendCounter: %u, currentState: %u, utility: %u, avgRSSI: %u, neighDegree: %u, clusterDegree: %u, lpDegree: %u, "
		"isActive: %u, started: %u, Parent: 0x%4x, Tier: %d\n",sendCounter, currentState,
		(uint8_t)(utility * 1000), (uint8_t)(avgRSSI * 1000), (uint8_t)(neighDegree * 1000), (uint8_t)(clusterDegree * 1000),
		(uint8_t)(lpDegree * 1000),CIDER_isActive,started, parent.u16, getTier());
}
void CIDER_reset()
{
CIDER_isActive = 0;
isLPD = 0;
started = 0;
currentState = CIDER_PING;
neighDegree = 0, lpDegree = 0, clusterDegree = 0;
avgRSSI = 0.0;
setTier(-1);
utility = 0.0;
sendCounter = -3;
utility = 0;
delayTimeslot = 0;
avgRSSI = 0;
neighDegree = 0;
clusterDegree = 0;
lpDegree = 0;

linkaddr_copy(&parent, &linkaddr_null);
csPingCounter = 0;
ciderInterval = (CLOCK_SECOND * 0.5);
ciderKAInterval = (CLOCK_SECOND * 30);
if (tsch_get_coordinator() == 0) tsch_set_eb_period(0);
etimer_stop(&cider_timer);
ctimer_stop(&cider_competition_timer);
ctimer_stop(&send_timer);
ctimer_stop(&ka_timer);
ciderInterval = CLOCK_SECOND * 0.5;
updateStatusLED();
clearTable();
printCIDERStatus();
printTable();
process_exit(&dewi_cider_process);
}

void callbackCompleteTimer()
{
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
struct CIDER_PACKET CIDERPacket;
CIDERPacket.base.dst = tsch_broadcast_address;
CIDERPacket.base.src = linkaddr_node_addr;
CIDERPacket.base.type = CIDER;
PRINTF("[CIDER]:Create COMPLETE MESSAGE \n");
CIDERPacket.subType = CIDER_COMPLETE;
clusteringUnComplete = 0;
packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, temp->timeslot);

broadcast_send(&cider_bc);

ctimer_set(&completeWaitTimer, CLOCK_SECOND * 5, callbackCompleteWaitTimer, NULL);
}
void callbackCompleteWaitTimer()
{
	PRINTF("[CIDER]: Clustering Complete\n");
clusteringComplete = 1;

//HERE NOTIFY OTHER PROTOCOLS
}

void callbackCompetitionTimer()
{
struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
struct CIDER_PACKET CIDERPacket;

CIDERPacket.base.dst = tsch_broadcast_address;
CIDERPacket.base.src = linkaddr_node_addr;
CIDERPacket.base.type = CIDER;
PRINTF("[CIDER]:Create CH MESSAGE \n");
CIDERPacket.subType = CIDER_CH;
int i;
for (i = 1; i < 45; i++)
{
	CIDERPacket.args[i] = 0;
}
updateNeighListCS(&CIDERPacket.args, 45, linkaddr_node_addr);
sendCounter = sendCounter + 1;
packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, temp->timeslot);

broadcast_send(&cider_bc);

}

void callbackKATimer()
{
PRINTF("[CIDER]: callbackKATimer expired, reset CIDER \n");
sendCounter = -3;
currentState = CIDER_PING;
utility = 0;
delayTimeslot = 0;
avgRSSI = 0;
neighDegree = 0;
clusterDegree = 0;
lpDegree = 0;

linkaddr_copy(&parent, &linkaddr_null);
csPingCounter = 0;
ciderInterval = (CLOCK_SECOND * 0.5);
ciderKAInterval = (CLOCK_SECOND * 30);

if (tsch_get_coordinator() == 0) tsch_set_eb_period(0);
etimer_stop(&cider_timer);
ctimer_stop(&cider_competition_timer);
ctimer_stop(&send_timer);
ctimer_stop(&ka_timer);
ciderInterval = CLOCK_SECOND * 0.5;
updateStatusLED();
clearTable();
printCIDERStatus();
printTable();
PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
	;
	etimer_set(&cider_timer, ciderInterval);
	PROCESS_CONTEXT_END(&dewi_cider_process);
}

