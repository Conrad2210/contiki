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
 *         CIDER: Clustering protocol for dense environments, working on top of TSCH
 *         see 'CIDER - Clustering In Dense EnviRonments' - XXX - 2016
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */
#include "contiki.h"
#include "cider.h"
#include "neighTable.h"
#include "dev/leds.h"
#include "project-conf.h"
#define CIDER_INTERVAL       (CLOCK_SECOND * 5)
#define DEBUG DEBUG_PRINT

uint8_t CIDER_isActive = 0;
uint8_t isLPD = 0;
uint8_t stageCounter = 0;
enum CIDERsubpackettype CIDER_currentStep = PING, CIDER_nextStep = UNDEFINED,
		CIDER_lastStep = UNDEFINED;
uint8_t CIDER_ND = 0, CIDER_LPD = 0, CIDER_CD = 0;
float CIDER_AvgRSSI = 0;
float CIDER_M1 = 0.5;
float CIDER_M2 = 0.3;
float CIDER_M3 = 0.15;
float CIDER_M4 = 0.05;
int CIDER_WEIGHT = 0;
struct CIDER_PACKET createCCIDERPacket();
static struct etimer CIDER_timer;
static struct ctimer CIDER_send_timer;
int started = 0;
uint16_t delayTimeslot = 0;
linkaddr_t parent = {{0,0}};

PROCESS(dewi_cider_process, "DEWI cider PROCESS");

uint8_t getTxPower8bit(int tx) {

	uint8_t tx8 = 0x00;
	switch (tx) {
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

int getTxPowerInt(uint8_t tx) {
	int txInt = -24;
	switch (tx) {
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

static void cider_packet_received(struct broadcast_conn *c,
		const linkaddr_t *from) {

	struct CIDER_PACKET *temp = packetbuf_dataptr();
	CIDER_start();
	int16_t tempRSSI;
	uint8_t newNeigh;
	radio_result_t rv = (int8_t) NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI,
			&tempRSSI);



	struct neighbour n = initNeighbour();

	switch (temp->subType) {

	case PING:
		printf("[CIDER]: Received CIDER Message, type: PING RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.txPW = getTxPowerInt(temp->args[0]);
		n.isLPD = temp->args[1];
		n.stage = PING;
		newNeigh = addNeighbour(&n);


		break;
	case NEIGHBOUR_UPDATE:
		printf("[CIDER]: Received CIDER Message, type: NEIGHBOUR_UPDATE RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.nodeDegree = temp->args[0];
		n.lpDegree = temp->args[1];
		n.clusterDegree = temp->args[2];
		n.stage = NEIGHBOUR_UPDATE;
		newNeigh = addNeighbour(&n);


		break;
	case WEIGHT_UPDATE:
		printf("[CIDER]: Received CIDER Message, type: WEIGHT_UPDATE RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.weight = temp->args[0];
		n.nodeDegree = temp->args[1];
		n.lpDegree = temp->args[2];
		n.clusterDegree = temp->args[3];
		n.stage = WEIGHT_UPDATE;
		newNeigh = addNeighbour(&n);


		break;
	case CH_COMPETITION:
		printf("[CIDER]: Received CIDER Message, type: CH_COMPETITION RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.weight = temp->args[0];
		n.nodeDegree = temp->args[1];
		n.lpDegree = temp->args[2];
		n.clusterDegree = temp->args[3];
		n.stage = CH_COMPETITION;
		newNeigh = addNeighbour(&n);


		break;
	case CH:
		printf("[CIDER]: Received CIDER Message, type: CH RSSI: %d\n",
				 tempRSSI);
		if (CIDER_currentStep != PING || stageCounter > 5) {
			n.addr = temp->base.src;
			n.last_rssi = tempRSSI;
			n.last_asn = tsch_get_current_asn();
			n.parent = temp->base.src;
			n.myCS = 0;
			n.stage = CH;
			int i;
			int isCH = 0;
			for (i = 1; i < 45; i++) {
				if (temp->args[i] == linkaddr_node_addr.u16) {
					isCH = 1;
				}
				updateNeighboursCH(temp->args[i], temp->base.src);
			}

			if(isCH == 1)
			{
				n.myCH = 1;
				parent = temp->base.src;
				CIDER_nextStep = CS;
				CIDER_currentStep = CS;
				CIDER_lastStep = CS;

			}
			else
			{
				n.myCH = 0;
				parent = linkaddr_null;
				CIDER_nextStep = CS_PING;
				CIDER_currentStep = CS_PING;
				CIDER_lastStep = CS_PING;
			}

			newNeigh = addNeighbour(&n);


			updateStatusLED();
		}

		break;
	case LP_PING:
		printf("[CIDER]: Received CIDER Message, type: LP_PING RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.txPW = getTxPowerInt(temp->args[0]);
		n.isLPD = temp->args[1];
		n.stage = LP_PING;
		newNeigh = addNeighbour(&n);


		break;
	case CS_PING:
		printf("[CIDER]: Received CIDER Message, type: CS_PING RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.weight = temp->args[0];
		n.nodeDegree = temp->args[1];
		n.lpDegree = temp->args[2];
		n.clusterDegree = temp->args[3];
		n.stage = CS_PING;
		newNeigh = addNeighbour(&n);


		break;
	case CS:
		printf("[CIDER]: Received CIDER Message, type: CS RSSI: %d\n",
				 tempRSSI);
		n.addr = temp->base.src;
		n.last_rssi = tempRSSI;
		n.last_asn = tsch_get_current_asn();
		n.weight = temp->args[0];
		n.nodeDegree = temp->args[1];
		n.lpDegree = temp->args[2];
		n.clusterDegree = temp->args[3];
		n.stage = CS;
		n.parent.u16 = temp->args[4];
		newNeigh = addNeighbour(&n);


		break;
	case COVERAGE_UPDATE:
		break;
	default:
		printf("[CIDER]: Received CIDER Message, type: unknown = %d\n",
				temp->subType);

		updateStatusLED();

	}

	PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
		;
		if (CIDER_currentStep < CH && stageCounter > 3) {
			if (!isLPD && checkIfReadyForNextStep(CIDER_currentStep) == 1) {
				CIDER_currentStep = CIDER_currentStep + 1;
			} else if (isLPD) {
				CIDER_currentStep = LP_PING;
			}
			stageCounter = 0;
		}
		stageCounter++;
		PROCESS_CONTEXT_END(&dewi_cider_process);
}

static const struct broadcast_callbacks cider_bc_rx = { cider_packet_received };

static struct broadcast_conn cider_bc;

int CIDER_notify() {
//todo: Implement check if RLL;
	if (CIDER_isActive == 0) {
		CIDER_isActive = 1;

		process_start(&dewi_cider_process, NULL);

	} else {
		CIDER_isActive = 0;
		process_exit(&dewi_cider_process);
	}

	CIDER_createSchedule();

	return 1;
}

void CIDER_start() {
	if (started == 0) {
		started = 1;
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
		;
		etimer_set(&CIDER_timer, CIDER_INTERVAL);
		PROCESS_CONTEXT_END(&dewi_cider_process);
	}
}

int CIDER_getIsActive() {
	return CIDER_isActive;
}

void CIDER_setLPD(uint8_t is) {
	isLPD = is;
}
uint8_t CIDER_getLPD() {
	return isLPD;
}

void CIDER_createSchedule() {
	if (CIDER_isActive == 1) {
		ScheduleInfo_t temp;
		temp.handle = 0x00;
		temp.slotframeLength = 51;
		int i;
		printf("[CIDER]: Create CIDER schedule\n");
		for (i = 0; i < temp.slotframeLength; i++) {
			temp.links[i].addr = &tsch_broadcast_address;
			temp.links[i].channel_offset = 0;
			temp.links[i].isActive = 1;

			if (i == 0) {
				temp.links[i].link_type = LINK_TYPE_ADVERTISING;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX
						| LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING;
			} else {
				temp.links[i].link_type = LINK_TYPE_NORMAL;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX
						| LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING;
			}

			temp.links[i].timeslot = i;
		}

		setSchedule(temp);
	} else {
		clearSchedule();
	}
}

void CIDER_calcWeight() {
	CIDER_ND = getNumNeighbours();
	CIDER_LPD = getNumLPDevices();
	CIDER_CD = getNumCluster();
	CIDER_AvgRSSI = getAvgRSSI();

	CIDER_WEIGHT = (CIDER_M1 * (float) CIDER_ND + CIDER_M2 * (float) CIDER_CD
			+ CIDER_M3 * (float) CIDER_LPD + CIDER_M4 * CIDER_AvgRSSI) * 1000;

}

uint8_t checkIfCHCompetition() {
	uint8_t shouldCH = 0;
	printf("[CIDER]: Check if I should become CH, my Weigth: %d \n",
			CIDER_WEIGHT);
	if (CIDER_WEIGHT >= getHighestWeight()) {
		shouldCH = 1;

		printf("[CIDER]: I should become CH, my Weigth: %d \n", CIDER_WEIGHT);
	} else {

		printf("[CIDER]: I should NOT become CH, my Weigth: %d \n",
				CIDER_WEIGHT);
	}

	return shouldCH;

}
void updateMyCluster() {

}

struct CIDER_PACKET createCIDERPacket() {
	struct CIDER_PACKET CIDERPacket;

	radio_value_t chan;
	radio_result_t rv;
	int *tempTX;
	tempTX = 0;

	switch (CIDER_currentStep) {
	case PING: //PING message

		printf("[CIDER]:Create CIDER PING MESSAGE\n");

		CIDERPacket.base.dst = tsch_broadcast_address;
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = PING;
		rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
		CIDERPacket.args[0] = getTxPower8bit(chan);
		CIDERPacket.args[1] = isLPD;
		CIDER_lastStep = CIDER_currentStep;
		if (CIDER_lastStep == PING && CIDER_nextStep >= NEIGHBOUR_UPDATE)
			CIDER_currentStep = NEIGHBOUR_UPDATE;
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;
		break;
	case NEIGHBOUR_UPDATE:

		printf("[CIDER]:Create CIDER NEIGHBOUR MESSAGE\n");

		CIDERPacket.base.dst = tsch_broadcast_address;
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = NEIGHBOUR_UPDATE;
		CIDERPacket.args[0] = getNumNeighbours();
		CIDERPacket.args[1] = getNumLPDevices();
		CIDERPacket.args[2] = getNumCluster();
		CIDER_lastStep = CIDER_currentStep;
		if (CIDER_lastStep == NEIGHBOUR_UPDATE
				&& CIDER_nextStep >= WEIGHT_UPDATE || stageCounter == 5) {
			CIDER_currentStep = WEIGHT_UPDATE;
			stageCounter = 0;
		} else
			stageCounter++;
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;
		break;
	case WEIGHT_UPDATE:

		printf("[CIDER]:Create CIDER Weight Update\n");
		CIDER_calcWeight();
		CIDERPacket.base.dst = tsch_broadcast_address;
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = WEIGHT_UPDATE;
		CIDERPacket.args[0] = CIDER_WEIGHT;
		CIDERPacket.args[1] = getNumNeighbours();
		CIDERPacket.args[2] = getNumLPDevices();
		CIDERPacket.args[3] = getNumCluster();
		CIDER_lastStep = CIDER_currentStep;
		if (CIDER_lastStep == WEIGHT_UPDATE && CIDER_nextStep >= CH_COMPETITION
				|| stageCounter == 5) {
			CIDER_currentStep = CH_COMPETITION;
			stageCounter = 0;
		} else
			stageCounter++;
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;
		break;
	case CH_COMPETITION:
		printf("[CIDER]:Create CIDER Weight Update\n");
		if (checkIfCHCompetition() == 1) {
			printf("[CIDER]:Create CIDER CH Competition\n");
			printf("[CIDER]:I should become a CH\n");
			CIDERPacket.base.dst = tsch_broadcast_address;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = CH_COMPETITION;
			CIDERPacket.args[0] = CIDER_WEIGHT;
			CIDER_lastStep = CIDER_currentStep;
			CIDER_currentStep = CH;
			PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
				;
				etimer_set(&CIDER_timer, CIDER_INTERVAL);
				PROCESS_CONTEXT_END(&dewi_cider_process);
		} else {

			CIDER_lastStep = CIDER_currentStep;
			CIDER_currentStep = CS_PING;
			printf("[CIDER]:I should become a CS, start sending CS_PINGS\n");
			PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
				;
				etimer_set(&CIDER_timer, CIDER_INTERVAL);
				PROCESS_CONTEXT_END(&dewi_cider_process);
		}
		break;
	case CH:
		CIDERPacket.base.dst = tsch_broadcast_address;
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = CH;
		int i;
		for (i = 0; i < 45; i++)
			CIDERPacket.args[i] = 0;

		updateNeighListCS(&CIDERPacket.args, 45, linkaddr_node_addr);
		CIDER_lastStep = CIDER_currentStep;
		CIDER_currentStep = CH;
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL * 5);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;

		break;
	case LP_PING:
		printf("[CIDER]:Create CIDER LP PING MESSAGE\n");

		CIDERPacket.base.dst = tsch_broadcast_address;
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = LP_PING;
		rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
		CIDERPacket.args[0] = getTxPower8bit(chan);
		CIDERPacket.args[1] = isLPD;
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;
		break;
	case CS_PING:
		printf("[CIDER]: Check if there is an CH\n");
		if(checkIfCHinNetwork(CH) == 0 && checkIfCHCompetition() == 1){
			printf("[CIDER]:Create CIDER CH Competition\n");
			printf("[CIDER]:I should become a CH\n");
			CIDERPacket.base.dst = tsch_broadcast_address;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = CH_COMPETITION;
			CIDERPacket.args[0] = CIDER_WEIGHT;
			CIDERPacket.args[1] = getNumNeighbours();
			CIDERPacket.args[2] = getNumLPDevices();
			CIDERPacket.args[3] = getNumCluster();
			CIDER_lastStep = CIDER_currentStep;
			CIDER_currentStep = CH;
		}else{
			printf("[CIDER]:Create CIDER CS PING MESSAGE\n");

			CIDERPacket.base.dst = tsch_broadcast_address;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = CS_PING;
			CIDERPacket.args[0] = CIDER_WEIGHT;
			CIDERPacket.args[1] = getNumNeighbours();
			CIDERPacket.args[2] = getNumLPDevices();
			CIDERPacket.args[3] = getNumCluster();
			rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
		}


		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;
		break;
	case CS:
		printf("[CIDER]:Create CIDER CS MESSAGE\n");

		CIDERPacket.base.dst = tsch_broadcast_address;
		CIDERPacket.base.src = linkaddr_node_addr;
		CIDERPacket.base.type = CIDER;
		CIDERPacket.subType = CS;
		CIDERPacket.args[0] = CIDER_WEIGHT;
		CIDERPacket.args[1] = getNumNeighbours();
		CIDERPacket.args[2] = getNumLPDevices();
		CIDERPacket.args[3] = getNumCluster();
		CIDERPacket.args[4] = parent.u16;
		rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
		if(linkaddr_cmp(&parent,&linkaddr_null) == 1)
		{
		PROCESS_CONTEXT_BEGIN(&dewi_cider_process)
			;
			etimer_set(&CIDER_timer, CIDER_INTERVAL);
			PROCESS_CONTEXT_END(&dewi_cider_process)
		;
		}
		break;

	case COVERAGE_UPDATE:
		break;

	default:
		//do nothing
		break;
	}

	printf("[CIDER]: CIDER_PACKET size %u\n", sizeof(struct CIDER_PACKET));
	return CIDERPacket;

}

void sendCIDERPacket() {
	struct CIDER_PACKET CIDERPacket;
	CIDERPacket = createCIDERPacket();
	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));

	if (CIDER_currentStep == CH) {
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);
	} else {
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, delayTimeslot);
	}

	broadcast_send(&cider_bc);
}

void updateStatusLED() {
	switch (CIDER_currentStep) {
	case PING: //PING message
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		break;
	case NEIGHBOUR_UPDATE:
		leds_off(LEDS_ALL);
		leds_on(LEDS_BLUE);
		break;
	case WEIGHT_UPDATE:
		leds_off(LEDS_ALL);
		leds_on(LEDS_YELLOW);
		break;
	case CH_COMPETITION:
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		break;
	case LP_PING:
		leds_off(LEDS_ALL);
		leds_on(LEDS_ALL);
		break;
	case COVERAGE_UPDATE:
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		break;
	case CS:
		leds_off(LEDS_ALL);
		leds_on(LEDS_YELLOW);
		break;
	case CH:
		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		break;
	case CS_PING:
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		break;

	default:
		//do nothing
		break;
	}
}

PROCESS_THREAD(dewi_cider_process, ev, data) {

	uint16_t msgDelay;
	clock_time_t delay;
	PROCESS_EXITHANDLER(broadcast_close(&cider_bc))
	broadcast_open(&cider_bc, BROADCAST_CHANNEL_CIDER, &cider_bc_rx);
	PROCESS_BEGIN()
		;

		while (1) {
			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER) {
				struct tsch_link *temp = tsch_schedule_get_next_active_link(
						&current_asn, 0, NULL);
				struct tsch_slotframe *tempSF =
						tsch_schedule_get_slotframe_by_handle(
								temp->slotframe_handle);
				uint16_t tsr = (tempSF->size.val - temp->timeslot); //remaining timeslots in this sf duration
				uint16_t tsd = (linkaddr_node_addr.u16 & 0b0000111111111111)
						/ 10;
				uint16_t total_delaySF = ((tsd - tsr) / tempSF->size.val);
				uint16_t total_delaySF_1000 = (((float) (tsd - tsr)
						/ (float) tempSF->size.val) * 100.0);
				uint16_t total_delaySF_10001 = (float) total_delaySF * 100.0;
				uint16_t fraction = (total_delaySF_1000 - total_delaySF_10001);
				delayTimeslot = ((float) fraction / 100.0)
						* (float) tempSF->size.val;
				uint16_t total_delay = total_delaySF * tempSF->size.val + tsr;

				delay = ((total_delay / 100.0)) * CLOCK_SECOND;
				if (ctimer_expired(&CIDER_send_timer) == 1
						&& CIDER_currentStep != CH)
					ctimer_set(&CIDER_send_timer, delay, sendCIDERPacket, NULL);
				else
					sendCIDERPacket();

				updateStatusLED();

			}
		}

	PROCESS_END();
}

void CIDER_reset() {
CIDER_isActive = 0;
isLPD = 0;
started = 0;
CIDER_currentStep = PING;
CIDER_nextStep = UNDEFINED;
CIDER_lastStep = UNDEFINED;
CIDER_ND = 0, CIDER_LPD = 0, CIDER_CD = 0;
CIDER_AvgRSSI = 0;

CIDER_WEIGHT = 0;
etimer_stop(&CIDER_timer);
ctimer_stop(&CIDER_send_timer);
process_exit(&dewi_cider_process);

updateStatusLED();
}
