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
uint8_t delayTimeslot = 0;
uint8_t isLPDevice = 0;
uint8_t CIDER_started = 0;
uint8_t clusteringComplete = 0;
uint8_t clusteringUnComplete = 0;

linkaddr_t parent = { { 0, 0 } }, promoteAddr = { { 0, 0 } };
uint8_t csPingCounter = 0;
clock_time_t ciderInterval = (CLOCK_SECOND * 0.5);
clock_time_t ciderKAInterval = (CLOCK_SECOND * 60);
uint32_t ebPeriod;

PROCESS(dewi_cider_process, "DEWI cider PROCESS");

struct CHMSGCounter
{
	uint8_t counter;
	linkaddr_t src;
} CHMSGCounter;

/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/
static struct etimer cider_timer;
static struct ctimer send_timer, cider_competitionWait_timer, cider_competition_timer,/* ka_timer,*/
completeTimer, completeWaitTimer;

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
float featureScale(float value, float min, float max);
uint8_t compareUtilities();
linkaddr_t checkForCSPingMember();
uint8_t checkCHinRange();
static void cider_packet_received(struct broadcast_conn *c, const linkaddr_t *from);
void callbackCompetitionTimer();
void callbackCompetitionWaitTimer();
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

static struct broadcast_conn cider_bc;/***************************************/
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
					printNodeStatus("updateState");
				}
				else
				{
					PRINTF("[CIDER]: Change State to: NEIGHBOUR_UPDATE \n");
					currentState = CIDER_NEIGHBOUR_UPDATE;
					printNodeStatus("updateState");
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
				printNodeStatus("updateState");
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
					printNodeStatus("updateState");
				}
				else
				{
					PRINTF("[CIDER]: Change State to: CS_PING \n");
					currentState = CIDER_CS_PING;
					printNodeStatus("updateState");
				}
				sendCounter = 0;
			}
			startSendTimer();
			break;

		case CIDER_CH_COMPETITION:
			PRINTF("[CIDER]: Current State: CH_COMPETITION \n");
			updateStatusLED();
			if (ctimer_expired(&cider_competitionWait_timer) == 1
					&& ctimer_expired(&cider_competition_timer) == 1)
				ctimer_set(&cider_competitionWait_timer, CLOCK_SECOND * 10,
						callbackCompetitionWaitTimer, NULL);
			startSendTimer();
			break;

		case CIDER_CH:
			PRINTF("[CIDER]: Current State: CH \n");
			updateStatusLED();
			if (clusteringComplete == 0)
			{
				if (getTier() == 0)
				{
					if (checkNodesUnclustered() == 1)
					{
						if (checkChildCH() == 1)
						{
							ctimer_set(&completeTimer, CLOCK_SECOND * 5, callbackCompleteTimer,
							NULL);
						}
						else
						{
							ctimer_set(&completeWaitTimer, CLOCK_SECOND * 10,
									callbackCompleteWaitTimer,
									NULL);
						}
					}
				}
			}

			if (sendCounter > 5)
			{
//			if (getTier() != 0) ciderInterval = CLOCK_SECOND * (random_rand() % (10 + 1 - 2) + 2);
//					else
				ciderInterval = CLOCK_SECOND * 5;

				PRINTF("[CIDER]: CIDER intervall = %d\n", ciderInterval/CLOCK_SECOND);
			}
			if (getTier() == 0 || sendCounter < 5) startSendTimer();
			break;

		case CIDER_CS:
			PRINTF("[CIDER]: Current State: CS \n");
			updateStatusLED();
			if (sendCounter <= 5)
				startSendTimer();
			else ciderInterval = CLOCK_SECOND * 10;

			break;
		case CIDER_CS_PING:
			PRINTF("[CIDER]: Current State: CS_PING \n");
			if (checkCHinRange() == 0 && sendCounter >= 20)
			{
				if (compareUtilities() == 1)
				{
					PRINTF("[CIDER]: Change State to: CH_COMPETITION \n");
					currentState = CIDER_CH_COMPETITION;
					sendCounter = 0;
					printNodeStatus("updateState");
				}
			}
			updateStatusLED();
			if (sendCounter > 2) ciderInterval = CLOCK_SECOND * 4;
			startSendTimer();
			break;

		case CIDER_CH_PROMOTE:
			PRINTF("[CIDER]: Current State: CH_PROMOTE \n");
			if (sendCounter >= 1)
			{
				PRINTF("[CIDER]: Change State to: CH \n");
				currentState = CIDER_CH;
				sendCounter = 0;
				resetMSGCounter();
				promoteAddr = linkaddr_null;
				printNodeStatus("updateState");
			}
			updateStatusLED();
			startSendTimer();
			break;
		case CIDER_CH_PROMOTE_ACK:
			PRINTF("[CIDER]: Current State: CIDER_CH_PROMOTE_ACK \n");
			if (sendCounter >= 2)
			{
				PRINTF("[CIDER]: Change State to: CH \n");
				currentState = CIDER_CH;
				sendCounter = 0;
				resetMSGCounter();
				promoteAddr = linkaddr_null;
				printNodeStatus("updateState");
			}
			startSendTimer();
			break;

	}
	setCIDERState(currentState);

}

void sendRSSI()
{
	uint8_t i = 0;
	struct CIDER_PACKET CIDERPacket;
	CIDERPacket.base.dst = tsch_broadcast_address;
	CIDERPacket.base.src = linkaddr_node_addr;
	CIDERPacket.base.type = CIDER;
	CIDERPacket.subType = CIDER_SETRSSIRADIUS;
	CIDERPacket.args[0] = getRSSIRadius() * -1;

	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);

	broadcast_send(&cider_bc);

}

void sendLQI()
{
	uint8_t i = 0;
	struct CIDER_PACKET CIDERPacket;
	CIDERPacket.base.dst = tsch_broadcast_address;
	CIDERPacket.base.src = linkaddr_node_addr;
	CIDERPacket.base.type = CIDER;
	CIDERPacket.subType = CIDER_SETLQIRADIUS;
	CIDERPacket.args[0] = getLQIRadius();

	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);

	broadcast_send(&cider_bc);

}

void sendTXPower()
{

	uint8_t i = 0;
	radio_value_t temp;
	NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &temp);

	PRINTF("[CIDER]: send TxPower: %d\n",temp);
	struct CIDER_PACKET CIDERPacket;
	CIDERPacket.base.dst = tsch_broadcast_address;
	CIDERPacket.base.src = linkaddr_node_addr;
	CIDERPacket.base.type = CIDER;
	CIDERPacket.subType = CIDER_SETTXPOWER;

	if (temp < 0)
	{
		CIDERPacket.args[1] = 1;
		CIDERPacket.args[0] = (uint16_t) temp * -1;
	}
	else
	{
		CIDERPacket.args[1] = 0;
		CIDERPacket.args[0] = (uint16_t) temp;

	}

	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);

	broadcast_send(&cider_bc);

}

void checkQueueStatus(){

	int packetsInQueue = tsch_queue_packet_count(&tsch_broadcast_address);

	if(packetsInQueue > 3)
		tsch_queue_reset();
}

void callbackSendTimer()
{

	//if (getActiveProtocol() == 0) tsch_queue_reset();

	checkQueueStatus(); //checks queue status, flushes queue if needed;


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
			CIDERPacket.args[1] = getLPD();
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
			CIDERPacket.args[0] = (uint16_t) (getUtility() * 1000);
			break;
		case CIDER_CH_COMPETITION:
			PRINTF("[CIDER]:Create CH_COMPETITION message \n");
			CIDERPacket.subType = CIDER_CH_COMPETITION;

			CIDERPacket.args[0] = (uint16_t) (getUtility() * 1000);
			break;
		case CIDER_CH:
			PRINTF("[CIDER]:Create CH MESSAGE \n");
			CIDERPacket.subType = CIDER_CH;
			CIDERPacket.parent = parent;
			CIDERPacket.tier = getTier();
			CIDERPacket.colour = getColour();
			int i;
			for (i = 0; i < 50; i++)
			{
				CIDERPacket.args[i] = 0;
			}
			updateNeighListCS(&CIDERPacket.args, 50, linkaddr_node_addr);
			break;
		case CIDER_LP_PING:
			PRINTF("[CIDER]:Create CIDER LP PING MESSAGE \n");
			CIDERPacket.subType = CIDER_LP_PING;
			rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
			CIDERPacket.args[0] = getTxPower8bit(chan);
			CIDERPacket.args[1] = getLPD();
			break;
		case CIDER_CS_PING:
			PRINTF("[CIDER]:Create CIDER CS PING MESSAGE \n");
			CIDERPacket.subType = CIDER_CS_PING;
			CIDERPacket.args[0] = (uint16_t) (getUtility() * 1000);
			CIDERPacket.args[1] = getNumNeighbours();
			CIDERPacket.args[2] = getNumLPDevices();
			CIDERPacket.args[3] = getNumCluster();
			break;
		case CIDER_CS:
			PRINTF("[CIDER]:Create CIDER CS MESSAGE \n");
			CIDERPacket.subType = CIDER_CS;
			CIDERPacket.args[0] = (uint16_t) (getUtility() * 1000);
			CIDERPacket.args[1] = getNumNeighbours();
			CIDERPacket.args[2] = getNumLPDevices();
			CIDERPacket.args[3] = getNumCluster();
			CIDERPacket.parent = parent;
			break;
		case CIDER_CH_PROMOTE:

			PRINTF("[CIDER]:Create CH_Promote MESSAGE \n");
			CIDERPacket.base.dst = promoteAddr;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = CIDER_CH_PROMOTE;
			CIDERPacket.parent = linkaddr_node_addr;
			CIDERPacket.args[0] = getTier();
			PRINTF("[CIDER] Promote MSG for: 0x%4x\n",CIDERPacket.base.dst.u16);

			break;

		case CIDER_CH_PROMOTE_ACK:
			PRINTF("[CIDER]:Create CIDER_CH_PROMOTE_ACK MESSAGE \n");
			CIDERPacket.base.dst = parent;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = CIDER_CH_PROMOTE_ACK;

			break;

		default:
			//do nothing
			break;
	}

	sendCounter = sendCounter + 1;
	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

	if (currentState == CIDER_CH || getActiveProtocol() == 2)
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
	if (CIDER_started == 0 && temp->subType != CIDER_SETRSSIRADIUS
			&& temp->subType != CIDER_SETLQIRADIUS && temp->subType != CIDER_SETTXPOWER)
	{
		CIDER_start();
	}
	int16_t tempRSSI;
	uint16_t tempLQI;
	radio_result_t rv = NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_LINK_QUALITY, &tempLQI);
	uint8_t newNeigh;
	tempRSSI = (signed short) packetbuf_attr(PACKETBUF_ATTR_RSSI);
	printf("[cider]: last rssi: %d, last lqi: %d\n", tempRSSI, tempLQI);

	struct neighbour n;

	initNeighbour(&n);

	getNeighbour(from, &n);

	switch (temp->subType)
	{
		case CIDER_SETRSSIRADIUS:
			PRINTF("[CIDER]: CIDER_SETRSSIRADIUS Message received from 0x%4x \n", from->u16);
			setRSSIRadius(temp->args[0] * -1);
			break;

		case CIDER_SETLQIRADIUS:
			PRINTF("[CIDER]: CIDER_SETLQIRADIUS Message received from 0x%4x \n", from->u16);
			setLQIRadius(temp->args[0]);
			break;
		case CIDER_SETTXPOWER:

			if (temp->args[1] == 1)
			{
				radio_value_t test = (radio_value_t) temp->args[0];
				radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, test * -1);
				PRINTF("[APP] Set Tx Power to %d, result: %d\n",test * -1,rv);
			}
			else
			{
				radio_value_t test = (radio_value_t) temp->args[0];

				radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, test);
				PRINTF("[APP] Set Tx Power to %d, result: %d\n",test,rv);
			}

			radio_value_t tempRes;
			NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &tempRes);
			PRINTF("[CIDER]: CIDER_SETTXPOWER Message received  from 0x%4x with TXPower: %d\n", from->u16, tempRes);

			break;

		case CIDER_PING:
			PRINTF("[CIDER]: Ping Message received from 0x%4x \n", from->u16);
			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.txPW = getTxPowerInt(temp->args[0]);
			n.isLPD = temp->args[1];
			n.CIDERState = CIDER_PING;
			newNeigh = addNeighbour(&n);
			break;
		case CIDER_NEIGHBOUR_UPDATE:
			PRINTF("[CIDER]: Neighbour Update received from 0x%4x \n", from->u16);
			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.nodeDegree = temp->args[0];
			n.lpDegree = temp->args[1];
			n.clusterDegree = temp->args[2];
			n.CIDERState = CIDER_NEIGHBOUR_UPDATE;
			newNeigh = addNeighbour(&n);
			break;
		case CIDER_UTILITY_UPDATE:
			PRINTF("[CIDER]: Utility Update received from 0x%4x \n", from->u16);
			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.utility = (float) temp->args[0] / 1000.0;
			n.CIDERState = CIDER_UTILITY_UPDATE;
			newNeigh = addNeighbour(&n);
			break;
		case CIDER_CS:
			PRINTF("[CIDER]: CS message received from 0x%4x \n", from->u16);
			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.utility = (float) temp->args[0] / 1000.0;
			n.nodeDegree = temp->args[1];
			n.lpDegree = temp->args[2];
			n.clusterDegree = temp->args[3];
			n.parent = temp->parent;
			if (temp->parent.u16 == linkaddr_node_addr.u16)
			{
				n.myCS = 1;
				n.myChildCH = 0;
			}
			n.CIDERState = CIDER_CS;
			n.msgCounter = 0;
			newNeigh = addNeighbour(&n);
			break;
		case CIDER_CH_COMPETITION:
			PRINTF("[CIDER]: CH Competition received from 0x%4x \n", from->u16);
			if (currentState == CIDER_CH_COMPETITION)
			{
				clock_time_t delayTime = random_rand() % (5 + 1);
				PRINTF("[CIDER]: Delay Competition message by %d s \n", delayTime);
				ctimer_stop(&cider_competitionWait_timer);
				ctimer_stop(&cider_competition_timer);
				ctimer_set(&cider_competition_timer, delayTime, callbackCompetitionTimer, NULL);
			}

			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.utility = (float) temp->args[0] / 1000.0;
			n.CIDERState = CIDER_CH_COMPETITION;
			newNeigh = addNeighbour(&n);
			break;
		case CIDER_CH:

			if (currentState == CIDER_CH_COMPETITION && n.myCH == 0)
			{
				ctimer_stop(&cider_competition_timer);
				ctimer_stop(&cider_competitionWait_timer);
				currentState = CIDER_CS_PING;
				sendCounter = 0;
			}
			PRINTF("[CIDER]: CH message received from 0x%4x \n", from->u16);
			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.CIDERState = CIDER_CH;
			n.myCS = 0;
			n.myCH = 0;
			n.parent = temp->parent;
			n.tier = temp->tier;
			n.colour = temp->colour;
			int i;
			int isCH = 0;
			for (i = 0; i < 50; i++)
			{
				if (temp->args[i] == linkaddr_node_addr.u16)
				{
					isCH = 1;
				}
				updateNeighboursCH(temp->args[i], temp->base.src, temp->args[0]);
			}

			if (isCH == 1)
			{
				if (temp->base.src.u16 == CHMSGCounter.src.u16)
				{
					CHMSGCounter.counter++;
					CHMSGCounter.src = temp->base.src;
				}
				else
				{
					CHMSGCounter.counter = 1;
					CHMSGCounter.src = temp->base.src;
				}
			}
			if (currentState == CIDER_CS_PING && isCH == 1 && n.myCH == 0
					&& CHMSGCounter.counter > 2)
			{
				ctimer_stop(&cider_competition_timer);
				n.myCH = 1;
				setTier(temp->tier);
				setCOLOURINGState(temp->colour);
				parent = temp->base.src;
				setParentStatus(parent);
				currentState = CIDER_CS;
				sendCounter = 0;

			}

			if (linkaddr_cmp(&parent, &temp->base.src) == 1)
			{

				PRINTF("[CIDER]: CH message received from 0x%4x, my Parent \n", from->u16);
				if (currentState == CIDER_CS) setColour(temp->colour);
				if (currentState == CIDER_CH && sendCounter >= 2 && getTier() != 0)
					callbackSendTimer();
//			ctimer_stop(&ka_timer);
//			ctimer_set(&ka_timer, ciderKAInterval, callbackKATimer, NULL);
			}
			newNeigh = addNeighbour(&n);
			break;
		case CIDER_CS_PING:
			PRINTF("[CIDER]: CS_PING received from 0x%4x \n", from->u16);
			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.utility = (float) temp->args[0] / 1000.0;
			n.nodeDegree = temp->args[1];
			n.lpDegree = temp->args[2];
			n.clusterDegree = temp->args[3];
			n.CIDERState = CIDER_CS_PING;
			n.myCS = 0;
			n.myChildCH = 0;
			n.myCH = 0;
			n.parent = linkaddr_null;
			uint8_t possibleNeigh = checkForPossibleCS();
			if (possibleNeigh == 1 && currentState == CIDER_CH)
			{
				n.msgCounter = n.msgCounter + 1;
				if (sendCounter > 6 && checkForPromotion(CIDER_CS_PING) == 1)
				{
					promoteAddr = getChildCHAddress(CIDER_CS_PING);
					currentState = CIDER_CH_PROMOTE;
					sendCounter = 0;
				}
			}
			else
			{
				resetMSGCounter();
			}

			newNeigh = addNeighbour(&n);

			break;

		case CIDER_CH_PROMOTE:
			PRINTF("[CIDER]: CH_PROMOTE received from 0x%4x \n", from->u16);

			if (temp->base.dst.u16 == linkaddr_node_addr.u16)
			{
				if (currentState == CIDER_CS || currentState == CIDER_CS_PING)
				{
					currentState = CIDER_CH_PROMOTE_ACK;

					setTier(temp->args[0] + 1);
					sendCounter = 0;
					parent = temp->parent;
					setParentStatus(parent);
					n.addr = temp->base.src;
					n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
					n.last_lqi = tempLQI;
					n.last_asn = tsch_get_current_asn();
					n.CIDERState = CIDER_CH;
					n.myCH = 1;
					n.tier = temp->args[0];
					newNeigh = addNeighbour(&n);
				}
			}
			else
			{
				PRINTF("[CIDER]: CH_PROMOTE not meant for me \n");
			}

			break;
		case CIDER_CH_PROMOTE_ACK:
			PRINTF("[CIDER]: CIDER_CH_PROMOTE_ACK received from 0x%4x \n", from->u16);

			n.addr = temp->base.src;
			n.last_rssi = (int16_t) ((float) (n.last_rssi + tempRSSI) / 2.0);
			n.last_lqi = tempLQI;
			n.last_asn = tsch_get_current_asn();
			n.parent = temp->base.dst;
			n.CIDERState = CIDER_CH;
			if (linkaddr_cmp(&temp->base.dst, &linkaddr_node_addr) == 1)
			{
				n.myChildCH = 1;
			}

			newNeigh = addNeighbour(&n);
			break;
		case CIDER_COMPLETE:
			PRINTF("[CIDER]: CIDER_COMPLETE received from 0x%4x \n", from->u16);
			if (currentState == CIDER_CH)
			{
				//first check if I have any childCH or CS
				if (checkChildCH() == 0 && checkCS() == 0)
				{
					PRINTF("[CIDER]:I have to demote myself, no CS nor CH's \n");
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
					setTier(getTier() - 1);

					//because there are no cs or childCH I demote myself
					currentState = CIDER_CS;
					sendCounter = 0;
					return;
				}

				if (ctimer_expired(&completeWaitTimer) == 1)
				{
					clusteringUnComplete = 0;
					if (checkNodesUnclustered() == 1)
					{
						if (checkChildCH() == 1)
						{
							ctimer_stop(&completeWaitTimer);
							ctimer_set(&completeWaitTimer, CLOCK_SECOND * 5,
									callbackCompleteWaitTimer,
									NULL);
						}
						else
						{
							clusteringComplete = 1;
						}
					}
					else
					{
						ctimer_stop(&completeWaitTimer);

						if (clusteringUnComplete == 0)
						{
							struct tsch_link *temp = tsch_schedule_get_next_active_link(
									&current_asn, 0,
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
	printf("[CIDER]: CIDER_notify\n");
//todo: Implement check if RLL;
//if (CIDER_getIsActive() == 1)
//{
//setActiveProtocol(0);
	PRINTF("[CIDER]: Start Cider \n");
	process_start(&dewi_cider_process, NULL);

//}

	return 1;
}

void CIDER_start()
{
	CIDER_started = 1;
	PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
		;
		//ebPeriod = CLOCK_SECOND * (random_rand() % (15 + 1 - 5) + 5);
		//printf("[CIDER]: EB Period: %d\n", ebPeriod);
		printf("[CIDER]: cider start\n");
		etimer_set(&cider_timer, ciderInterval);
		PROCESS_CONTEXT_END(&dewi_cider_process);

}

int CIDER_getIsActive()
{
	return getActiveProtocol() == 0 ? 1 : 0;
}

void CIDER_setLPD(uint8_t is)
{
	setLPD(is);
}
uint8_t CIDER_getLPD()
{
	return getLPD();
}

void CIDER_createSchedule()
{
	if (CIDER_getIsActive() == 1)
	{

	}
	else
	{
		clearSchedule();
	}
}

void calcUtility()
{
	setAVGRSSI(getAvgRSSI());
	setNeighDegree(getNumNeighbours());
	setClusDegree(getNumCluster());
	setLPDegree(getNumLPDevices());
	if (getNeighDegree() == 0.0 || getClusDegree() == 0.0)
	{
		setUtility(0);
		return;
	}
	setNeighDegree(featureScale((float) getNeighDegree(), CIDER_ND_min, CIDER_ND_max));
	setClusDegree(featureScale((float) getClusDegree(), CIDER_CD_min, CIDER_CD_max));

	if (getLPDegree() == 0)
	{

		setAVGRSSI(featureScale((float) getAVGRSSI(), CIDER_RSSI_min, CIDER_RSSI_max));
		setUtility(
				(CIDER_M1 * (float) getNeighDegree()
						+ (CIDER_M2 + CIDER_M3) * (float) getClusDegree() + CIDER_M4 * getAVGRSSI()));
	}
	else
	{
		setLPDegree(featureScale((float) getLPDegree(), CIDER_LP_min, CIDER_LP_max));
		setUtility(
				(CIDER_M1 * (float) getNeighDegree() + CIDER_M2 * (float) getClusDegree()
						+ CIDER_M3 * (float) getLPDegree() + CIDER_M4 * getAVGRSSI()));
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
	if (getUtility() >= highestNeighbourUtility)
		return 1;
	else return 0;
}

linkaddr_t checkForCSPingMember()
{
	linkaddr_t returnAddr = getChildCHAddress(CIDER_CS_PING);
	return returnAddr;
}

uint8_t checkCHinRange()
{
	if (checkIfCHinNetwork(CIDER_CH) == 1 || checkIfCHinNetwork(CIDER_CH_COMPETITION) == 1)
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
	PRINTF("[CIDER]: Change LED Colour to: %d\n",currentState);
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
void CIDER_reset()
{
	setActiveProtocol(0);
	setLPD(0);
	CIDER_started = 0;
	currentState = CIDER_PING;
	setNeighDegree(0), setLPDegree(0), setClusDegree(0);
	setAVGRSSI(0);
	setTier(-1);
	setUtility(0.0);
	sendCounter = -3;
	delayTimeslot = 0;

	linkaddr_copy(&parent, &linkaddr_null);
	csPingCounter = 0;
	ciderInterval = (CLOCK_SECOND * 0.5);
	ciderKAInterval = (CLOCK_SECOND * 30);
	if (tsch_get_coordinator() == 0) tsch_set_eb_period(0);
	etimer_stop(&cider_timer);
	ctimer_stop(&cider_competition_timer);
	ctimer_stop(&send_timer);
//ctimer_stop(&ka_timer);
	ciderInterval = CLOCK_SECOND * 0.5;
	updateStatusLED();
	clearTable();
	printTable();
	process_exit(&dewi_cider_process);
	COLOURING_Reset();
	RLL_reset();
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
	COLOURING_start();
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
	for (i = 1; i < 50; i++)
	{
		CIDERPacket.args[i] = 0;
	}
	updateNeighListCS(&CIDERPacket.args, 50, linkaddr_node_addr);
	sendCounter = sendCounter + 1;
	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
	packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, temp->timeslot);

	broadcast_send(&cider_bc);

}

void callbackCompetitionWaitTimer()
{
	PRINTF("[CIDER]: Change State to: CH \n");
	currentState = CIDER_CH;
	sendCounter = 0;
	printNodeStatus("callbackCompetitionWaitTimer");
	if (getTier() == -1) setTier(0);
//if (getCoord() == 0) tsch_set_eb_period(ebPeriod);

}

void callbackKATimer()
{
	PRINTF("[CIDER]: callbackKATimer expired \n");

	sendCounter = -3;
	currentState = CIDER_PING;
	setUtility(0);
	delayTimeslot = 0;
	setAVGRSSI(0);
	setNeighDegree(0);
	setClusDegree(0);
	setLPDegree(0);

	linkaddr_copy(&parent, &linkaddr_null);
	csPingCounter = 0;
	ciderInterval = (CLOCK_SECOND * 0.5);
	ciderKAInterval = (CLOCK_SECOND * 30);

	if (tsch_get_coordinator() == 0) tsch_set_eb_period(0);
	etimer_stop(&cider_timer);
	ctimer_stop(&cider_competition_timer);
	ctimer_stop(&send_timer);
//ctimer_stop(&ka_timer);
	ciderInterval = CLOCK_SECOND * 0.5;
	updateStatusLED();
	clearTable();
	printTable();
	PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
		;
		etimer_set(&cider_timer, ciderInterval);
		PROCESS_CONTEXT_END(&dewi_cider_process);

	COLOURING_Reset();
	RLL_reset();
}

/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS_THREAD(dewi_cider_process, ev, data)
{
	PROCESS_EXITHANDLER(broadcast_close(&cider_bc));

	broadcast_open(&cider_bc, BROADCAST_CHANNEL_CIDER, &cider_bc_rx);
	PROCESS_BEGIN()
		;

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

			}

		}

	PROCESS_END();
}
